//
//  PartialSpectrumAnalyzer.hpp
//  bBpiano
//
//  Created by opus arc on 2026/5/19.
//
//  This document is not AI-assisted.
//

#ifndef PartialSpectrumAnalyzer_hpp
#define PartialSpectrumAnalyzer_hpp

#include <iostream>


struct Partial_Scan {
    std::string pitchName = "A4";
    double f0 = 440.0;
    int midi_n = 69;
    std::string velocity = "110";
    std::vector<double> partials_part;
};

struct KeySpectrum {
    std::string pitchName = "A4";
    double f0 = 440.0;
    int midi_n = 69;
    std::string velocity = "110";
    std::vector<std::vector<float>> spectrogram;
};

struct PianoSpectrogram {
    std::string velocity = "110";
    std::vector<KeySpectrum> ks;
    KeySpectrum& operator [] (double midi_n){
        for(auto& k : ks) {
            if(k.midi_n == midi_n) return k;
        }
        return ks[0];
    }
};

struct AllPianoSpectrogram {
    std::vector<PianoSpectrogram> aps;
    PianoSpectrogram& operator [] (std::string velocity){
        for(auto& ps : aps) {
            if(ps.velocity == velocity) return ps;
        }
        return aps[0];
    }
};

struct BeginFrameAndEndFrame {
    
    std::vector<std::array<const int, 2>> startFrameAndEndFrame;
    
    BeginFrameAndEndFrame(double framesPerSecond, std::size_t spectrogram_size){
        std::vector<std::array<const double, 2>> skipSecAndWindowSec;
        
        skipSecAndWindowSec.push_back({0.18, 0.78});
        skipSecAndWindowSec.push_back({0.05, 0.35});
        skipSecAndWindowSec.push_back({0.017, 0.1});
        
        for(auto& sw : skipSecAndWindowSec) {
            const int startFrame = std::clamp(
                static_cast<int>(std::round(sw[0] * framesPerSecond)),
                0,
                static_cast<int>(spectrogram_size) - 1
            );
            const int endFrame = std::clamp(
                startFrame + static_cast<int>(std::round(sw[1] * framesPerSecond)),
                startFrame,
                static_cast<int>(spectrogram_size) - 1
            );
            startFrameAndEndFrame.push_back({startFrame, endFrame});
        }
    }
    int get(const std::string& beginOrEnd, const std::string& mod) {

        int row = 1; // default: mid

        if (mod == "low") {
            row = 0;
        }
        else if (mod == "mid") {
            row = 1;
        }
        else if (mod == "high") {
            row = 2;
        }

        int col = (beginOrEnd == "begin") ? 0 : 1;

        return startFrameAndEndFrame[row][col];
    }
};

class PartialSpectrumAnalyzer {
public:
    static void analyzer();
    
};

#endif /* PartialSpectrumAnalyzer_hpp */
