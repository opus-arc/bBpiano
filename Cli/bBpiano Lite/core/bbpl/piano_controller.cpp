// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
// [ZERO AI-GENERATED CODE]
// Every line in this file is written and understood by its author. Every result is
// reproducible, every assumption is open to inspection, and every implementation
// stands open to criticism and challenge.
// AI may be used for non-core, replaceable, engineering work;
// this file, however, contains core logic that the author considers
// necessary to understand firsthand, explain line by line, and take full responsibility for,
// and is therefore implemented entirely by hand.
// ---------------------------------------------------------------------------
// [本文件承诺不含任何 AI 生成代码]
// 每一行代码均由作者亲自编写，并确知其意义。一切结果可以复现，一切假设可经受检验，一切实现经得起批评与质疑。
// AI 可用于非核心、可替代的工程工作；
// 本文件承载作者认为必须亲自理解、能够逐行解释并为之负责的核心逻辑，因此刻意保持完全人工实现。
//
// Ziyang Tan
// 2026-09-04
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

#include <stop_token>
#include <atomic>
#include <cstdint>
#include <memory>
#include <stdexcept>


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
            KeyModel* key = bBpiano->piano_keys[command.note - k_lowest_midi_note];
            key->key_down_ = true;
            for (int i = 0; i < key->string_count_; ++i) {
                key->strings_[i].damper_active = false;
            }
            key->key_active_ = true;
            // Yamaha Disklavier 使用的 mapping
            const double hammer_v0 = std::pow(2, (command.value - 52.0) / 25.0);
            key->trigger(hammer_v0);
            break;
        }

        case PianoCommandType::note_off: {
            if (is_playable_note(command.note)) {
                bBpiano->piano_keys[command.note - k_lowest_midi_note]->key_down_ = false;
            }
            KeyModel* key = bBpiano->piano_keys[command.note - k_lowest_midi_note];
            key->key_down_ = false;
            if (!key->sustainpedal_active_) {
                for (int i = 0; i < key->string_count_; ++i) {
                    key->strings_[i].damper_active = true;
                }
            }
            key->key_active_ = true;
            break;
        }

        case PianoCommandType::sustain_pedal:
            bBpiano->sustainpedal_control(command.value > 0.01);
            
            break;

        case PianoCommandType::all_silence:
            bBpiano->system_reset();
            break;

        case PianoCommandType::note_aftertouch:
            
        case PianoCommandType::soft_pedal:
        case PianoCommandType::harmonic_pedal:
        case PianoCommandType::sostenuto_pedal:
            // 当前 PianoModel 尚未实现这些动作。
            break;
    }
}

}  // namespace


// ======================== ======================== ========================
// Initialize
// 初始化
// ======================== ======================== ========================
void bbpiano_init(double sample_rate) {
    bBpiano = std::make_unique<PianoModel>(sample_rate);
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
// Hardware callback and test
// 硬件回调与测试
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
            std::cerr
                << "OUTPUT ABNORMAL\n"
                << "sample = " << x << '\n'
                << "frame = " << i << '\n';
            
            for(int i = 0; i < bBpiano->piano_keys.size(); i++) {
                if(bBpiano->piano_keys[i]->key_active_) {
                    std::cout << "hammer_.w_a_1_" <<
                    bBpiano->piano_keys[i]->hammer_.w_a_1_ << "\n";
                    std::cout << "hammer_.w_b_1_" <<
                    bBpiano->piano_keys[i]->hammer_.w_b_1_ << "\n";
                    std::cout << "hammer_.middle_v_" <<
                    bBpiano->piano_keys[i]->hammer_.middle_v_ << "\n";
                }
            }
            
            std::cout<<"\n";
                x = 0.0f;
            }
        
        out[i] = x * amplitudeLimiter;
    }
    
    engine_eval->end_timing(frameCount);
    
}
double get_engine_rate() noexcept {
    return engine_eval
        ? engine_eval->engine_rate()
        : 0.0;
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
    audio_render_failure.store(
        true, std::memory_order_release);

    notify_engine_event();
}

bool audio_render_failed() noexcept {
    return audio_render_failure.load(
        std::memory_order_acquire);
}

void clear_audio_render_failure() noexcept {
    audio_render_failure.store(
        false, std::memory_order_release);
}

bool wait_for_stop_or_audio_failure(
    std::stop_token stop_token) noexcept {

    std::stop_callback wake_on_stop(
        stop_token,
        [] {
            notify_engine_event();
        });

    while (!stop_token.stop_requested() &&
           !audio_render_failed()) {

        const std::uint32_t generation =
            engine_event_generation.load(
                std::memory_order_acquire);

        if (stop_token.stop_requested() ||
            audio_render_failed()) {
            break;
        }

        engine_event_generation.wait(
            generation,
            std::memory_order_acquire);
    }

    return audio_render_failed();
}
