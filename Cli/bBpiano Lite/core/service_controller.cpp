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

#include <condition_variable>
#include <cstdint>
#include <filesystem>
#include <iostream>
#include <mutex>
#include <chrono>
#include <thread>

#include "./service_controller.hpp"
#include "./bbpl/piano_controller.hpp"

#include "./bbpl/hardware/cli_helper.hpp"

#include "./bbpl/hardware/service/midi_exporter_service.hpp"
#include "./bbpl/hardware/service/midi_keyboard_service.hpp"
#include "./bbpl/hardware/service/midi_service.hpp"
#include "./bbpl/hardware/service/pc_keyboard_service.hpp"
#include "./bbpl/hardware/service/midi_recorder_service.hpp"


namespace {

void wait_for_stop(std::stop_token stop_token) {
    std::mutex mutex;
    std::condition_variable_any condition;
    std::unique_lock lock(mutex);
    condition.wait(lock, stop_token, [] { return false; });
}

}

// ======================== ======================== ========================
// Basic support
// 基础支持
// ======================== ======================== ========================
int cli_entry(int argc, char* argv[], const char* version, const char* logo) {
    return cli_helper(argc, argv, version, logo);
}

// ======================== ======================== ========================
// Services
// 服务
// ======================== ======================== ========================
void midi_service(const std::string& midi_path, std::stop_token stop_token) {
    std::cout << "Playing MIDI: " << midi_path << '\n';
    const MidiService::Result result =
        MidiService::play(1.0, 0.0, midi_path, stop_token);
    if (result == MidiService::Result::completed) {
      std::cout << "MIDI playback completed.\n";
    }
}
void piano_service(std::stop_token stop_token) {
    MidiInputHub input_hub;
    MidiKeyboard keyboard(input_hub);
    keyboard.start();
    input_hub.start();
    
    std::cout << "MIDI piano started (Ctrl-C exits).\n";

    wait_for_stop(stop_token);

    input_hub.stop();
    keyboard.stop();
}
void keyboard_service(std::stop_token stop_token) {
    PcKeyboard keyboard;
    keyboard.run(stop_token);
}
void export_service(std::string midi_path, std::stop_token stop_token) {
    std::filesystem::path output_path(midi_path);
    output_path.replace_extension(".wav");
    const MidiExporter::Result result = MidiExporter::export_wav(
          midi_path, output_path, static_cast<std::uint32_t>(sample_rate), 5.0,
          stop_token);
    if (result == MidiExporter::Result::completed) {
        std::cout << "Exported WAV: " << output_path << '\n';
    }
}
void record_service(std::stop_token stop_token) {
    MidiInputHub input_hub;
    MidiKeyboard keyboard(input_hub);
    MidiRecorder recorder(input_hub);

    keyboard.start();
    recorder.start();
    input_hub.start();
    std::cout << "MIDI recording started (Ctrl-C saves and exits).\n";

    wait_for_stop(stop_token);

    input_hub.stop();
    const std::filesystem::path output_path = recorder.stop();
    keyboard.stop();
    std::cout << "Recorded MIDI: " << output_path << '\n';
}
void test_service(std::stop_token stop_token) {
    std::cout << "Test service started.\n\n";



    for (int midi = 21; midi <= 108; ++midi) {
        note_on(midi, 110.0);
    }

    std::this_thread::sleep_for(
        std::chrono::milliseconds(500)
    );

    print_engine_rate();

//    wait_for_stop(stop_token);
}

void internal_test_service(std::stop_token stop_token) {
    std::cout << "Internal test started (Ctrl-C exits).\n";
    
    sustainpedal_control(0.6);
    note_on(69, 110);
    wait_for_stop(stop_token);
//    note_off(69, 0.0);
    all_silence();
}



