// ============================================================================
// bBpiano Physical Modeling Engine
// Backend Command Line Interface
//
// Authorship:
//   Ziyang Tan & Zhuoran Chen
//
// Design & Architecture:
//   Ziyang Tan
//
// Conceptual Development & Engineering:
//   Ziyang Tan & Zhuoran Chen
//
// AI Disclosure:
//   AI tools were used for learning programming techniques and domain knowledge
//   in physical modeling pianos and synthesizers.
//
//   CORE CLI IMPLEMENTATION:
//   DESIGNED AND DEVELOPED WITHOUT AI-GENERATED CODE.
//
// Project Initiated:
//   April 1, 2026
// ============================================================================


// --------------------------------------------------------------------
// MARK: version & logo

#define VERSION "L0-Works"
static constexpr const char* logo =
    R"(
        ┌────────────────────────────┐──╭────╮
        │                            │==│╲╱╲╱│
        │  bBpiano 0                 │==│╱╲╱╲│
        │  L0-Works/260727           │==│╲╱╲╱│
        │                            │==│╱╲╱╲│
        │  Physical Modeling Piano   │==│╲╱╲╱│
        │                            │==│╱╲╱╲│
        │  Developed by              │==│╲╱╲╱│
        │    Ziyang Tan              │==│╱╲╱╲│
        │    Zhuoran Chen            │==│╱╲╱╲│
        │                            │==│╲╱╲╱│
        │  bBSonicLab                │==│╱╲╱╲│
        └────────────────────────────┘──╰────╯

    )";

// cli 必须库
#include <iostream>
#include <csignal>


// Service 库
#include "./hardware/soundCard.hpp"
#include "./hardware/midiService.hpp"
#include "./hardware/midiKeyboard.hpp"
#include "./hardware/pcKeyboard.hpp"
#include "./hardware/midiRecorder.hpp"
#include "./hardware/midiExporter.hpp"
#include "./core/controller.hpp"

// cli 必须函数
void printLogo();
void signalHandler(int);
void help_zh();
void help_en();
void help_ja();

// Service 函数
void init_engine();
void shotdown_engine();
int runEnginePerformanceTest();

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ---------------------------------------------------------------------------------------------------
// MARK: MAIN
int main(int argc, const char * argv[]) {

    // --------------------------------------------------------------------
    // MARK: 临时用于测试的 Argument Vector
    
//    const char* testArgv[] = {
//        argv[0],
//        "-e",
//        "/Users/opusarc/Projects/XCodeProjects/bBpiano/Cli/bBpiano Lite/2026-06-22 1018 (Monday) 171 notes, 213 seconds.mid",
//        "/Users/opusarc/Projects/XCodeProjects/bBpiano/Cli/bBpiano Lite/260622-opusarc.wav"
//    };
//    argc = 4;
//    argv = testArgv;
    
    
//    const char* testArgv[] = {
//        argv[0],
//        "-k"
//    };
//    argc = 2;
//    argv = testArgv;
//    
//        const char* testArgv[] = {
//            argv[0],
//            "-b"
//        };
//        argc = 2;
//        argv = testArgv;
    
//    const char* testArgv[] = {
//        argv[0],
//        "-m",
//        "/Users/opusarc/Projects/XCodeProjects/bBpiano/Cli/bBpiano Sonic/data/midi_samples/Fancy's selection/midi/Sonata in B Min._2006.midi"
//    };
//    argc = 3;
//    argv = testArgv;
    
    
    // --------------------------------------------------------------------
    // MARK: 注册信号处理函数
    
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);
    
    
    // --------------------------------------------------------------------
    // MARK: 分类处理指令
    
    if(argc < 2) {
        printLogo();
        help_en();
        return EXIT_SUCCESS;
    }

    const std::string cmd = argv[1];
    
    if(cmd == "--version" ||
       cmd == "-v") {
        printLogo();
        std::cout
         << "   bBpiano Lite " << VERSION << "\n";
        return EXIT_SUCCESS;
    } else if(cmd == "--benchmark" ||
              cmd == "-b") {
        return runEnginePerformanceTest();
    } else if(cmd == "-zh" || cmd == "zh") {
        help_zh();
        return EXIT_SUCCESS;
    } else if (cmd == "-en" || cmd == "en") {
        help_en();
        return EXIT_SUCCESS;
    } else if (cmd == "-ja" || cmd == "ja") {
        help_ja();
        return EXIT_SUCCESS;
    }
    
    try {
        printLogo();
        init_engine();
        
        if(cmd == "-t") { // touchboard
            // TODO: touchboard
        } else if (cmd == "-k") { // keyboard
            SoundCard::shared().start();
            PcKeyboard::shared().start();
            PcKeyboard::shared().waitUntilStopped();
        } else if (cmd == "-p") { // piano
            SoundCard::shared().start();
            MidiKeyboard::start();
            MidiKeyboard::waitUntilStopped();
        } else if (cmd == "-m") { // midifile
            
            if(argc < 3) {
                std::cerr << "No MIDI file path provided.\n";
                shotdown_engine();
                return EXIT_FAILURE;
            }
            
            SoundCard::shared().start();
            
            std::string midiFilePath = argv[2];
            double playbackRate = 1.0;
            double startTime = 0.0;
            
            if(argc >= 4) {
                startTime = std::stod(argv[3]);
            }

            if(argc >= 5) {
                playbackRate = std::stod(argv[4]);
            }
            
            MidiService::play(playbackRate, startTime, midiFilePath);
            MidiService::waitUntilFinished();
        } else if (cmd == "-e") {
            if (argc < 3) {
                std::cerr << "No MIDI file path provided.\n";
                shotdown_engine();
                return EXIT_FAILURE;
            }

            std::string midiFilePath = argv[2];
            std::string outputWavPath;
            if (argc >= 4) {
                outputWavPath = argv[3];
            }

            MidiExporter::exportWav(midiFilePath, outputWavPath);
        } else if(cmd == "-pr") {
            SoundCard::shared().start();
            std::string recordFilePath;
            if (argc >= 3) {
                recordFilePath = argv[2];
            }

            MidiRecorder::start(recordFilePath);
            MidiKeyboard::start();
            MidiKeyboard::waitUntilStopped();
        } else {
            std::cerr << "Unknown command: "
                      << cmd << '\n';
            help_en();
            shotdown_engine();
            return EXIT_FAILURE;
        }
    } catch (const std::exception& error) {
        shotdown_engine();
        std::cerr << "Error: " << error.what() << '\n';
        return EXIT_FAILURE;
    }
    
    shotdown_engine();
    return EXIT_SUCCESS;
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ---------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------
// MARK: Service 函数的实现
void init_engine() {
    bBpiano_init();
}
void shotdown_engine() {
    std::cout << "\nStopping...\n";
    SoundCard::shared().stop();
    bBpiano_shutdown();
    
    MidiService::stop();
    PcKeyboard::shared().stop();
    MidiKeyboard::stop();
    MidiRecorder::stop();
}

// --------------------------------------------------------------------
// MARK: cli 必须函数的实现
void printLogo() {
    std::cout << logo << "\n";
}
void signalHandler(int) {
    // 退出前需要执行的函数
    shotdown_engine();
    std::exit(EXIT_SUCCESS);
}
void help_zh() {
    
    std::cout
        << "        用法:\n"
        << "          bbpl -k\n"
        << "          bbpl -p\n"
        << "          bbpl -pr [output.mid]\n"
        << "          bbpl -m <file.mid> [start] [rate]\n"
        << "          bbpl -e <file.mid> [output.wav]\n"
        << "          bbpl -b\n"
        << "\n"
        << "        命令:\n"
        << "          -k    电脑键盘演奏\n"
        << "          -p    MIDI 键盘演奏\n"
        << "          -pr   MIDI 键盘演奏并录制\n"
        << "          -m    播放 MIDI 文件\n"
        << "          -e    将 MIDI 文件导出为 WAV\n"
        << "          -b    引擎实时性能测试\n"
        << "\n";
}

void help_en() {
    
    std::cout
        << "        Usage:\n"
        << "          bbpl -k\n"
        << "          bbpl -p\n"
        << "          bbpl -pr [output.mid]\n"
        << "          bbpl -m <file.mid> [start] [rate]\n"
        << "          bbpl -e <file.mid> [output.wav]\n"
        << "          bbpl -b\n"
        << "\n"
        << "        Commands:\n"
        << "          -k    Play with computer keyboard\n"
        << "          -p    Play with MIDI keyboard\n"
        << "          -pr   Play with MIDI keyboard and record\n"
        << "          -m    Play a MIDI file\n"
        << "          -e    Export a MIDI file to WAV\n"
        << "          -b    Run realtime engine benchmark\n"
        << "\n";
}

void help_ja() {
    
    std::cout
        << "        使用方法:\n"
        << "          bbpl -k\n"
        << "          bbpl -p\n"
        << "          bbpl -pr [output.mid]\n"
        << "          bbpl -m <file.mid> [start] [rate]\n"
        << "          bbpl -e <file.mid> [output.wav]\n"
        << "          bbpl -b\n"
        << "\n"
        << "        コマンド:\n"
        << "          -k    PCキーボードで演奏\n"
        << "          -p    MIDIキーボードで演奏\n"
        << "          -pr   MIDIキーボードで演奏しながら録音\n"
        << "          -m    MIDIファイルを再生\n"
        << "          -e    MIDIファイルをWAVとして書き出し\n"
        << "          -b    エンジンのリアルタイム性能を測定\n"
        << "\n";
}



//
//  main.cpp
//  bBpiano Lite
//
//  Created by opus arc on 2026/6/7.
//
