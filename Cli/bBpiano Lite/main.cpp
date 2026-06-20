//
//  main.cpp
//  bBpiano Lite
//
//  Created by opus arc on 2026/6/7.
//

#include <iostream>
#define VERSION "L0-100c"
void printLogo() {
    static constexpr const char* logo =
    R"(
        ┌────────────────────────────┐──╭────╮
        │                            │==│╲╱╲╱│
        │  bBpiano 0                 │==│╱╲╱╲│
        │  L0-100c🎉/260608          │==│╲╱╲╱│
        │                            │==│╱╲╱╲│
        │  Physical Modeling Piano   │==│╲╱╲╱│
        │                            │==│╱╲╱╲│
        │  Developed by              │==│╲╱╲╱│
        │    Ziyang Tan              │==│╱╲╱╲│
        │    Zhuoran Chen            │==│╱╲╱╲│
        │                            │==│╲╱╲╱│
        │  bBSonicLab                │==│╱╲╱╲│
        └────────────────────────────┘──╰────╯
    
       🎊 Celebrating 100 Commits of bBpiano 🎊
    )";
    std::cout << logo << "\n";
}


#include <csignal>

#include "./hardware/soundCard.hpp"
#include "./hardware/midiService.hpp"
#include "./hardware/midiKeyboard.hpp"
#include "./hardware/pcKeyboard.hpp"

#include "./hardware/midiRecorder.hpp"
#include "./hardware/midiExporter.hpp"

#include "./core/controller.hpp"

void help_zh();
void help_en();
void help_ja();

void init_engine();
void shotdown_engine();

void signalHandler(int);

int main(int argc, const char * argv[]) {
    const char* testArgv[] = {
        argv[0],
        "-e",
        "/Users/opusarc/Projects/XCodeProjects/bBpiano/Cli/bBpiano Sonic/data/midi_samples/Fancy's selection/midi/Sonata No. 28 in A Major, Op. 101 (Complete)_2014.midi",
        "/Users/opusarc/Projects/XCodeProjects/bBpiano/Cli/bBpiano Lite/Sonata No. 28 in A Major, Op. 101 new2.wav"
    };
    argc = 4;
    argv = testArgv;
    
    
//    const char* testArgv[] = {
//        argv[0],
//        "-k"
//    };
//    argc = 2;
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
         << "   bBpiano Lite " << VERSION << "\n";
        return EXIT_SUCCESS;
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

void signalHandler(int) {
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
        << "          bbpl -e <file.mid> [output.wav]\n\n"
        << "        命令:\n"
        << "          -k    电脑键盘演奏\n"
        << "          -p    MIDI 键盘演奏\n"
        << "          -pr   MIDI 键盘演奏并录制\n"
        << "          -m    播放 MIDI 文件\n"
        << "          -e    将 MIDI 文件导出为 WAV\n\n";
}

void help_en() {
    
    std::cout
        << "        Usage:\n"
        << "          bbpl -k\n"
        << "          bbpl -p\n"
        << "          bbpl -pr [output.mid]\n"
        << "          bbpl -m <file.mid> [start] [rate]\n"
        << "          bbpl -e <file.mid> [output.wav]\n\n"
        << "        Commands:\n"
        << "          -k    Play with computer keyboard\n"
        << "          -p    Play with MIDI keyboard\n"
        << "          -pr   Play with MIDI keyboard and record\n"
        << "          -m    Play a MIDI file\n"
        << "          -e    Export a MIDI file to WAV\n\n";
}

void help_ja() {
    
    std::cout
        << "        使用方法:\n"
        << "          bbpl -k\n"
        << "          bbpl -p\n"
        << "          bbpl -pr [output.mid]\n"
        << "          bbpl -m <file.mid> [start] [rate]\n"
        << "          bbpl -e <file.mid> [output.wav]\n\n"
        << "        コマンド:\n"
        << "          -k    PCキーボードで演奏\n"
        << "          -p    MIDIキーボードで演奏\n"
        << "          -pr   MIDIキーボードで演奏しながら録音\n"
        << "          -m    MIDIファイルを再生\n"
        << "          -e    MIDIファイルをWAVとして書き出し\n\n";
}


