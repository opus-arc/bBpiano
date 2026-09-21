//
//  main.cpp
//  bBpiano Lite
//
//  Created by opus arc on 2026/6/7.
//

#define VERSION "1.0.0"

#include <iostream>
#include <csignal>

#include "./hardware/soundCard.hpp"
#include "./hardware/midiService.hpp"
#include "./hardware/midiKeyboard.hpp"
#include "./hardware/pcKeyboard.hpp"

#include "./core/controller.hpp"

void help_zh();
void help_en();
void help_ja();
void printLogo();

void init_engine();
void shotdown_engine();

void run(int argc, const char * argv[]);

void signalHandler(int);

int main(int argc, const char * argv[]) {
//    const char* testArgv[] = {
//        argv[0],
//        "run",
//        "-k"
//    };
//    argc = 3;
//    argv = testArgv;

    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);
    
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
         << "bBpiano Lite " << VERSION << "\n";
        return 0;
    } else if(cmd == "-zh" || cmd == "zh") {
        help_zh();
        return 0;
    } else if (cmd == "-en" || cmd == "en") {
        help_en();
        return 0;
    } else if (cmd == "-ja" || cmd == "ja") {
        help_ja();
        return  0;;
    }
    
    try{
        if (cmd == "run") {
            run(argc, argv);
        } else {
            std::cerr << "Unknown command: "
                      << cmd << '\n';
            help_en();
            return EXIT_FAILURE;
        }
    } catch (const std::exception& error) {
        std::cerr << "Error: " << error.what() << '\n';
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}

void run(int argc, const char * argv[]){
    if(argc < 3) {
        std::cerr << "No running commands.\n";
        return ;
    } else {
        try {
            printLogo();
            init_engine();
            const std::string cmd = argv[2];
            
            if(cmd == "-t") { // touchboard
                // TODO: touchboard
            } else if (cmd == "-k") { // keyboard
                PcKeyboard::shared().start();
                PcKeyboard::shared().waitUntilStopped();
            } else if (cmd == "-p") { // piano
                MidiKeyboard::start();
                MidiKeyboard::waitUntilStopped();
            } else if (cmd == "-m") { // midifile
                
                if(argc < 4) {
                    std::cerr << "No MIDI file path provided.\n";
                    return;
                }
                
                std::string midiFilePath = argv[3];
                double playbackRate = 1.0;
                double startTime = 0.0;
                
                if(argc >= 5) {
                    startTime = std::stod(argv[4]);
                }

                if(argc >= 6) {
                    playbackRate = std::stod(argv[5]);
                }
                
                MidiService::play(playbackRate, startTime, midiFilePath);
                MidiService::waitUntilFinished();
            }
        } catch (const std::exception& error) {
            shotdown_engine();
            std::cerr << "Error: " << error.what() << '\n';
            return ;
        }
        shotdown_engine();
    }
}

void init_engine() {
    bBpiano_init();
    SoundCard::shared().start();
}
void shotdown_engine() {
    std::cout << "\nStopping...\n";
    SoundCard::shared().stop();
    bBpiano_shutdown();
    
    MidiService::stop();
    PcKeyboard::shared().stop();
    MidiKeyboard::stop();
}

void signalHandler(int) {
    shotdown_engine();
    std::exit(EXIT_SUCCESS);
}



void help_zh() {
    
    std::cout
        << "bBpiano Lite " << VERSION << "\n\n"
        << "用法:\n"
        << "  bbpiano run -k\n"
        << "  bbpiano run -p\n"
        << "  bbpiano run -m <file.mid> [start] [rate]\n\n"
        << "命令:\n"
        << "  -k    电脑键盘演奏\n"
        << "  -p    MIDI 键盘演奏\n"
        << "  -m    播放 MIDI 文件\n\n"
        << "选项:\n"
        << "  -v, --version   显示版本信息\n"
        << "  zh, -zh         中文帮助\n"
        << "  en, -en         English Help\n"
        << "  ja, -ja         日本語ヘルプ\n\n"
        << "MIDI 参数:\n"
        << "  start   起始时间（秒）\n"
        << "  rate    播放速度（默认 1.0）\n\n";
}

void help_en() {
    
    std::cout
        << "bBpiano Lite " << VERSION << "\n\n"
        << "Usage:\n"
        << "  bbpiano run -k\n"
        << "  bbpiano run -p\n"
        << "  bbpiano run -m <file.mid> [start] [rate]\n\n"
        << "Commands:\n"
        << "  -k    Play with computer keyboard\n"
        << "  -p    Play with MIDI keyboard\n"
        << "  -m    Play a MIDI file\n\n"
        << "Options:\n"
        << "  -v, --version   Show version information\n"
        << "  zh, -zh         中文帮助\n"
        << "  en, -en         English Help\n"
        << "  ja, -ja         日本語ヘルプ\n\n"
        << "MIDI Parameters:\n"
        << "  start   Start time in seconds\n"
        << "  rate    Playback speed (default: 1.0)\n\n";
}

void help_ja() {
    
    std::cout
        << "bBpiano Lite " << VERSION << "\n\n"
        << "使用方法:\n"
        << "  bbpiano run -k\n"
        << "  bbpiano run -p\n"
        << "  bbpiano run -m <file.mid> [start] [rate]\n\n"
        << "コマンド:\n"
        << "  -k    PCキーボードで演奏\n"
        << "  -p    MIDIキーボードで演奏\n"
        << "  -m    MIDIファイルを再生\n\n"
        << "オプション:\n"
        << "  -v, --version   バージョン情報を表示\n"
        << "  zh, -zh         中文帮助\n"
        << "  en, -en         English Help\n"
        << "  ja, -ja         日本語ヘルプ\n\n"
        << "MIDI パラメータ:\n"
        << "  start   開始位置（秒）\n"
        << "  rate    再生速度（デフォルト: 1.0）\n\n";
}

void printLogo() {
    static constexpr const char* logo = 
 R"(
    ┌──────────────────────────────────────┐
    │  bBpiano Lite                        │
    │                                      │
    │  Physical Modeling Piano             │
    │                                      │
    │  Developed by                        │
    │  Ziyang Tan · Zhuoran Chen           │
    │                                      │
    │  bBSonicLab                          │
    │  2026-06-07                          │
    └──────────────────────────────────────┘)";

    std::cout << logo << "\n\n";
}
