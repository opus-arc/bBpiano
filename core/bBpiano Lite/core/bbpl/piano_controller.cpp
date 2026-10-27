#include <algorithm>
#include <cmath>
#include <stop_token>
#include <atomic>
#include <cstdint>
#include <memory>
#include <stdexcept>
#include "./configuration/configuration.hpp"


#include "./piano/piano_model.hpp"

#include "./piano_controller.hpp"
#include "./hardware/soundcard_helper.hpp"
#include "./eval/engine_rate/engine_rate_eval.hpp"

#include "./hardware/service/piano_command_queue.hpp"

std::unique_ptr<PianoModel> bBpiano;
std::unique_ptr<Soundcard> soundcard;
std::unique_ptr<EngineEval> engine_eval;

// ======================== ======================== ========================
// Commands
// 基础命令流模块
// ======================== ======================== ========================
namespace {

constexpr int k_lowest_midi_note = 21;
constexpr int k_highest_midi_note = 108;

PianoCommandBus piano_commands;

std::atomic<bool> audio_render_failure{false};
std::atomic<std::uint32_t> engine_event_generation{0};

bool is_playable_note(int midi_note) noexcept {
    return midi_note >= k_lowest_midi_note &&
    midi_note <= k_highest_midi_note;
}

void notify_engine_event() noexcept {
    engine_event_generation.fetch_add(1, std::memory_order_release);
    engine_event_generation.notify_all();
}

void post_command(PianoCommandType type, int midi_note, double value) noexcept {
    static_cast<void>(piano_commands.post({type, midi_note, value}));
}

void apply_piano_command(const PianoCommand& command) {
    if (!bBpiano) {
        throw std::logic_error("Piano engine is not initialized.");
    }

    switch (command.type) {
        case PianoCommandType::note_on: {
            if (!is_playable_note(command.note))
                break;
            bBpiano->note_on(command.note, command.value);
            break;
        }

        case PianoCommandType::note_off: {
            if (!is_playable_note(command.note))
                break;
            bBpiano->note_off(command.note);
            break;
        }

        case PianoCommandType::sustain_pedal:
            bBpiano->sustain_pedal_control(command.value);
            break;

        case PianoCommandType::all_silence:
            bBpiano->system_reset();
            break;

        case PianoCommandType::note_aftertouch:
            break;
        case PianoCommandType::soft_pedal:
//            bBpiano->soft_pedal_control(command.value);
            break;
        case PianoCommandType::harmonic_pedal:
            break;
        case PianoCommandType::sostenuto_pedal:
            bBpiano->sostenuto_pedal_control(command.value);
            break;
    }
}

}  // namespace


// ======================== ======================== ========================
// Initialize
// 初始化
// ======================== ======================== ========================
void bbpiano_init(double sample_rate) {
    if (sample_rate != configuration.basic_presets.get_sample_rate())
        throw std::invalid_argument("Piano sample rate differs from configuration");
    bBpiano = std::make_unique<PianoModel>();
}
void bbpiano_shutdown() noexcept {
    bBpiano.reset();
}
void eval_init(double sample_rate) {
    engine_eval = std::make_unique<EngineEval>(sample_rate);
}
void eval_shutdown() noexcept {
    engine_eval.reset();
}
void soundcard_init(double sample_rate) {
    soundcard = std::make_unique<Soundcard>(sample_rate);
    soundcard->start();
}
void soundcard_shutdown() noexcept {
    soundcard.reset();
}

// ======================== ======================== ========================
// Hardware callback
// 硬件回调
// ======================== ======================== ========================
void get_next_buffer(float* out, int frameCount, double amplitudeLimiter) {
    
    if (out == nullptr || frameCount < 0) {
        throw std::invalid_argument("Invalid audio output buffer.");
    }

    if (!bBpiano || !engine_eval) {
        throw std::logic_error("Piano engine is not initialized.");
    }
    
    // 每个音频 buffer 只检查一次队列，不在每个 sample 上做原子操作。
    piano_commands.drain([](const PianoCommand& command) {
        apply_piano_command(command);
    });

    engine_eval->start_timing();
    
    for (int i = 0; i < frameCount; ++i) {
        bBpiano->piano_movement();
      
        float x = bBpiano->get_sample();
        
        if (!std::isfinite(x)) {
            // No synchronous I/O on the real-time render thread.
            // Stop processing a corrupted engine state and clear this buffer.
            std::fill(out + i, out + frameCount, 0.0f);
            report_audio_render_failure();
            engine_eval->end_timing(frameCount);
            return;
        }

        out[i] = x * amplitudeLimiter;
    }
    
    engine_eval->end_timing(frameCount);
    
}


// ======================== ======================== ========================
// Piano gesture
// 钢琴手势
// ======================== ======================== ========================
void note_on(int midi_n, double velocity) noexcept {
    if(is_playable_note(midi_n)) {
        post_command(PianoCommandType::note_on, midi_n, velocity);
    }
}
void note_off(int midi_n, double velocity) noexcept {
    if(is_playable_note(midi_n)) {
        post_command(PianoCommandType::note_off, midi_n, velocity);
    }
}
void note_aftertouch(int midi_n, double pressure) noexcept {
    if(is_playable_note(midi_n)) {
        post_command(PianoCommandType::note_aftertouch, midi_n, pressure);
    }
}

// ======================== ======================== ========================
// Piano pedal
// 钢琴踏板
// ======================== ======================== ========================
void softpedal_control(double depth) noexcept {
    post_command(PianoCommandType::soft_pedal, 0, depth);
}
void harmonicpedal_control(double depth) noexcept {
    post_command(PianoCommandType::harmonic_pedal, 0, depth);
}
void sostenutopedal_control(double depth) noexcept {
    post_command(PianoCommandType::sostenuto_pedal, 0, depth);
}
void sustainpedal_control(double depth) noexcept {
    post_command(PianoCommandType::sustain_pedal, 0, depth);
}

void all_silence() noexcept {
    post_command(PianoCommandType::all_silence, 0, 0.0);
}




// ======================== ======================== ========================
// Deal with Error
// 报错与线程
// ======================== ======================== ========================

void clear_piano_commands() noexcept {
    piano_commands.clear();

    if (bBpiano) {
        bBpiano->system_reset();
    }
}

void report_audio_render_failure() noexcept {
    audio_render_failure.store(true, std::memory_order_release);

    notify_engine_event();
}

bool audio_render_failed() noexcept {
    return audio_render_failure.load(std::memory_order_acquire);
}

void clear_audio_render_failure() noexcept {
    audio_render_failure.store(false, std::memory_order_release);
}

bool wait_for_stop_or_audio_failure(std::stop_token stop_token) noexcept {
                                        std::stop_callback wake_on_stop(stop_token,
                                                                        [] {
                                                                            notify_engine_event();
                                                                        });

                                        while (!stop_token.stop_requested() &&
                                               !audio_render_failed()) {

                                            const std::uint32_t generation =
                                            engine_event_generation.load(std::memory_order_acquire);

                                            if (stop_token.stop_requested() ||
                                                audio_render_failed()) {
                                                break;
                                            }

                                            engine_event_generation.wait(generation,
                                                                         std::memory_order_acquire);
                                        }

                                        return audio_render_failed();
                                    }


// ======================== ======================== ========================
// Test
// 测试
// ======================== ======================== ========================
void print_engine_rate() {
    if (!bBpiano || !engine_eval) {
        throw std::logic_error(
                               "Piano engine is not initialized."
                               );
    }
    
    // 保证测试从完全确定的状态开始。
    piano_commands.clear();
    bBpiano->system_reset();
    
    // 激活 High-range。
    for (int midi = 79;
         midi <= k_highest_midi_note;
         ++midi) {

        apply_piano_command({
            PianoCommandType::note_on,
            midi,
            110.0
        });
    }
    
    engine_eval->print_benchmark(bBpiano, "High-range");
    

    // 保证测试从完全确定的状态开始。
    piano_commands.clear();
    bBpiano->system_reset();

    // 激活 Mid-range。
    for (int midi = 48;
         midi <= 78;
         ++midi) {

        apply_piano_command({
            PianoCommandType::note_on,
            midi,
            110.0
        });
    }

    engine_eval->print_benchmark(bBpiano, "Mid-range");
    
    // 保证测试从完全确定的状态开始。
    piano_commands.clear();
    bBpiano->system_reset();

    // 激活 Bass-range。
    for (int midi = k_lowest_midi_note;
         midi <= 47;
         ++midi) {

        apply_piano_command({
            PianoCommandType::note_on,
            midi,
            110.0
        });
    }

    engine_eval->print_benchmark(bBpiano, "Bass-range");
    
    // 保证测试从完全确定的状态开始。
    piano_commands.clear();
    bBpiano->system_reset();

    // 激活全部 88 个键。
    for (int midi = k_lowest_midi_note;
         midi <= k_highest_midi_note;
         ++midi) {

        apply_piano_command({
            PianoCommandType::note_on,
            midi,
            110.0
        });
    }

    engine_eval->print_benchmark(bBpiano, "88-keys");
    

    bBpiano->system_reset();
}
