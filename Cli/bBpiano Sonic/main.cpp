//
//  main.cpp
//  bBpiano Sonic
//
//  Created by opus arc on 2026/6/9.
//


// test
#include "./core/eval/loris/FilterConstantsCompute.hpp"
#include "./utils/MyPitch.hpp"

#include <filesystem>
#include <iostream>
#define VERSION "S0-alpha"
void printLogo() {
    static constexpr const char* logo =
    
     R"(    
        ┌────────────────────────────────┐
        │                            ││││││
        │  bBpiano 0              │││││││││││                      
        │  S-alpha/260609                │││                         
        │                                ││││││││││
        │  Physical Piano Standards    ││││││││││
        │                                │││││
        │  Developed by                 │││││
        │  Ziyang Tan · Zhuoran Chen     ││││││││││
        │                             ││││
        │  bBSonicLab                ││││││
        └────────────────────────────────┘
    )";

    std::cout << logo << "\n";
}


#include <csignal>

void help_zh();
void help_en();
void help_ja();

void init_engine();
void shotdown_engine();

void signalHandler(int);

int main(int argc, const char * argv[]) {
    using std::cout;
    
//    FilterConstantsCompute fcc = FilterConstantsCompute( "/Users/opusarc/Projects/XCodeProjects/bBpiano/AcousticLab/StringFilterLab/Samples/Pianoteq 9/SingleNoteSamples/Split/v80/A2_take03_v80.wav");
//
//    for(int j = 1; j <= 7; j++) {
        
//        std::string pitchName = "A" + std::to_string(j);
    
    std::string basePath_head = "/Users/opusarc/Projects/XCodeProjects/bBpiano/AcousticLab/StringFilterLab/Samples/Pianoteq 9/SingleNoteSamples/Split/v80/";
    std::string basePath_tail = "_take03_v80.wav";
    
    
    for(int i = 0; i <=7; i++) {
        std::string pitchName = "A" + std::to_string(i);
        double f0 = MyPitch::getFrequency(pitchName);
        std::string fullPath = basePath_head + pitchName + basePath_tail;
        if(std::filesystem::exists(fullPath)) {
            FilterConstantsCompute fcc =
                FilterConstantsCompute(f0, fullPath);
            fcc.printTopPartial();
            std::cout<<"\n";
        }
    }
    for(int i = 0; i <=7; i++) {
        std::string pitchName = "As" + std::to_string(i);
        double f0 = MyPitch::getFrequency(pitchName);
        std::string fullPath = basePath_head + pitchName + basePath_tail;
        if(std::filesystem::exists(fullPath)) {
            FilterConstantsCompute fcc =
                FilterConstantsCompute(f0, fullPath);
            fcc.printTopPartial();
            std::cout<<"\n";
        }
    }
    for(int i = 0; i <=7; i++) {
        std::string pitchName = "B" + std::to_string(i);
        double f0 = MyPitch::getFrequency(pitchName);
        std::string fullPath = basePath_head + pitchName + basePath_tail;
        if(std::filesystem::exists(fullPath)) {
            FilterConstantsCompute fcc =
                FilterConstantsCompute(f0, fullPath);
            fcc.printTopPartial();
            std::cout<<"\n";
        }
    }
    for(int i = 0; i <=7; i++) {
        std::string pitchName = "C" + std::to_string(i);
        double f0 = MyPitch::getFrequency(pitchName);
        std::string fullPath = basePath_head + pitchName + basePath_tail;
        if(std::filesystem::exists(fullPath)) {
            FilterConstantsCompute fcc =
                FilterConstantsCompute(f0, fullPath);
            fcc.printTopPartial();
            std::cout<<"\n";
        }
    }
    for(int i = 0; i <=7; i++) {
        std::string pitchName = "Cs" + std::to_string(i);
        double f0 = MyPitch::getFrequency(pitchName);
        std::string fullPath = basePath_head + pitchName + basePath_tail;
        if(std::filesystem::exists(fullPath)) {
            FilterConstantsCompute fcc =
                FilterConstantsCompute(f0, fullPath);
            fcc.printTopPartial();
            std::cout<<"\n";
        }
    }
    for(int i = 0; i <=7; i++) {
        std::string pitchName = "D" + std::to_string(i);
        double f0 = MyPitch::getFrequency(pitchName);
        std::string fullPath = basePath_head + pitchName + basePath_tail;
        if(std::filesystem::exists(fullPath)) {
            FilterConstantsCompute fcc =
                FilterConstantsCompute(f0, fullPath);
            fcc.printTopPartial();
            std::cout<<"\n";
        }
    }
    for(int i = 0; i <=7; i++) {
        std::string pitchName = "Ds" + std::to_string(i);
        double f0 = MyPitch::getFrequency(pitchName);
        std::string fullPath = basePath_head + pitchName + basePath_tail;
        if(std::filesystem::exists(fullPath)) {
            FilterConstantsCompute fcc =
                FilterConstantsCompute(f0, fullPath);
            fcc.printTopPartial();
            std::cout<<"\n";
        }
    }
    for(int i = 0; i <=7; i++) {
        std::string pitchName = "E" + std::to_string(i);
        double f0 = MyPitch::getFrequency(pitchName);
        std::string fullPath = basePath_head + pitchName + basePath_tail;
        if(std::filesystem::exists(fullPath)) {
            FilterConstantsCompute fcc =
                FilterConstantsCompute(f0, fullPath);
            fcc.printTopPartial();
            std::cout<<"\n";
        }
    }
    for(int i = 0; i <=7; i++) {
        std::string pitchName = "F" + std::to_string(i);
        double f0 = MyPitch::getFrequency(pitchName);
        std::string fullPath = basePath_head + pitchName + basePath_tail;
        if(std::filesystem::exists(fullPath)) {
            FilterConstantsCompute fcc =
                FilterConstantsCompute(f0, fullPath);
            fcc.printTopPartial();
            std::cout<<"\n";
        }
    }
    for(int i = 0; i <=7; i++) {
        std::string pitchName = "Fs" + std::to_string(i);
        double f0 = MyPitch::getFrequency(pitchName);
        std::string fullPath = basePath_head + pitchName + basePath_tail;
        if(std::filesystem::exists(fullPath)) {
            FilterConstantsCompute fcc =
                FilterConstantsCompute(f0, fullPath);
            fcc.printTopPartial();
            std::cout<<"\n";
        }
    }
    for(int i = 0; i <=7; i++) {
        std::string pitchName = "G" + std::to_string(i);
        double f0 = MyPitch::getFrequency(pitchName);
        std::string fullPath = basePath_head + pitchName + basePath_tail;
        if(std::filesystem::exists(fullPath)) {
            FilterConstantsCompute fcc =
                FilterConstantsCompute(f0, fullPath);
            fcc.printTopPartial();
            std::cout<<"\n";
        }
    }
    for(int i = 0; i <=7; i++) {
        std::string pitchName = "Gs" + std::to_string(i);
        double f0 = MyPitch::getFrequency(pitchName);
        std::string fullPath = basePath_head + pitchName + basePath_tail;
        if(std::filesystem::exists(fullPath)) {
            FilterConstantsCompute fcc =
                FilterConstantsCompute(f0, fullPath);
            fcc.printTopPartial();
            std::cout<<"\n";
        }
    }
    
//    std::string wavPath = "/Users/opusarc/Projects/XCodeProjects/bBpiano/Cli/bBpiano Sonic/core/eval/loris/pianoteq_A7.wav";
//        
//    FilterConstantsCompute fcc = FilterConstantsCompute(440, wavPath);
//    
//    fcc.printTopPartial();
        
//        for(int i = 0; i < fcc.lorisData.partials.size(); i++)
//            cout << "A" << j << ": f" << i << ": " << /*fcc.lorisData.partials[i].mean_frequency_hz << "\n";*/
//    }

    
    
//    fcc.findRealPartials();
    
//    fcc.computeB();
//    fcc.printTopPartial();
    
    
    
    
    
    
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
         << "   bBpiano Sonic " << VERSION << "\n";
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
        
        if(cmd == "-t") {
            
        } else if (cmd == "-k") {
            
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
    
}
void shotdown_engine() {
    std::cout << "\nStopping...\n";

}

void signalHandler(int) {
    shotdown_engine();
    std::exit(EXIT_SUCCESS);
}




void help_zh() {
    
    std::cout
        << "        用法:\n"
        << "          bbps -k\n"
        << "          bbps -p\n"
        << "          bbps -pr [output.mid]\n"
        << "          bbps -m <file.mid> [start] [rate]\n"
        << "          bbps -e <file.mid> [output.wav]\n\n"
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
        << "          bbps -k\n"
        << "          bbps -p\n"
        << "          bbps -pr [output.mid]\n"
        << "          bbps -m <file.mid> [start] [rate]\n"
        << "          bbps -e <file.mid> [output.wav]\n\n"
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
        << "          bbps -k\n"
        << "          bbps -p\n"
        << "          bbps -pr [output.mid]\n"
        << "          bbps -m <file.mid> [start] [rate]\n"
        << "          bbps -e <file.mid> [output.wav]\n\n"
        << "        コマンド:\n"
        << "          -k    PCキーボードで演奏\n"
        << "         -p    MIDIキーボードで演奏\n"
        << "          -pr   MIDIキーボードで演奏しながら録音\n"
        << "          -m    MIDIファイルを再生\n"
        << "          -e    MIDIファイルをWAVとして書き出し\n\n";
}
