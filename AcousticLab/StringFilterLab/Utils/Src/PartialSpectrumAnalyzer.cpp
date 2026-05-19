//
//  PartialSpectrumAnalyzer.cpp
//  bBpiano
//
//  Created by opus arc on 2026/5/19.
//

#include "PartialSpectrumAnalyzer.hpp"

#include "../MyFile.hpp"
#include "../MyPitch.hpp"
#include "../MyDrWav.h"
#include "../MyFFT.h"

#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;
const fs::path generatedDataRoot = "/Users/opusarc/XCodeProjects/bBpiano/AcousticLab/StringFilterLab/Generated";
const fs::path partialsFolderPath = generatedDataRoot / "Partials";
const fs::path splitFolder =
    "/Users/opusarc/XCodeProjects/bBpiano/AcousticLab/StringFilterLab/Samples/Pianoteq 9/SingleNoteSamples/Split";

const bool useSymmetricDoubleSidedLoss = true; // 双边反射各承担一半 loss, 如果以后改成单边滤波，这里改成 false。

void PartialSpectrumAnalyzer::analyzer() {
    using std::cout;
    if(!fs::exists(generatedDataRoot)) return ;
    if(!fs::exists(splitFolder)) return ;
    fs::create_directories(partialsFolderPath);
    
    double sampleRate = 44100.0;
    int channel = 2;
    const int fftSize = 32768;
    const int hopSize = 512;
    
    for (const auto& split_entry : fs::directory_iterator(splitFolder)) {
        const std::string velocity = MyFile::findVelocity(split_entry.path().filename());
        for (const auto& entry : fs::directory_iterator(split_entry)) {
            const fs::path path = entry.path();
            const std::string filename = path.filename();
            const std::string pitchName = MyFile::findPitchName(filename);
            const double f0 = MyPitch::getFrequency(pitchName);
            
            if (!entry.is_regular_file() || path.extension() != ".wav")
                continue;
            
            // TODO: 这里临时关掉了沙盒 为了不用复制这些采样音频进入 resource，以后还是要打开的吧
            // 转成 pcm 信号
            const std::vector<float> pcm_stereo = MyDrWav::loadWav(path, sampleRate, channel);
                
            // pcm 双声道变 单声道 好做 SFTF
            const std::vector<float> pcm_mono = MyDrWav::downmixStereoToMono(pcm_stereo);
            
            // 快速傅立叶变换
            STFTResult stft_result = MyFFT::computeSpectrogram(pcm_mono, sampleRate, fftSize, hopSize);
        
            // spectrogram[frame][f_bin] -> amp
            const std::vector<std::vector<float>> spectrogram = stft_result.spectrogram;
            const std::vector<float> binFrequencies = stft_result.binFrequencies;
            
            // 全局扫峰
            cout << "f0:" << f0 << "\n";
            double last_bin = 0.0;
            for(int b = 0; b < spectrogram[0].size(); b++) {
                double f_bin = binFrequencies[b];
                    std::size_t points[5] = {
                        spectrogram.size() / 2,
                        spectrogram.size() / 3,
                        spectrogram.size() / 4,
                        spectrogram.size() / 5,
                        spectrogram.size() / 6
                    };
                    float max = 0.0;
                    for(int po = 0; po < 5; po++) {
                        if(spectrogram[points[po]][b] > max)
                            max = spectrogram[points[po]][b];
                    }
                if(max > 0.1) {
                    if(abs(f_bin - last_bin) > 5) {
                        cout << "\n";
                    }
                    cout << "[" << f_bin << "]";
                    last_bin = f_bin;
                }

            }
            cout << "\n";
        
            
            std::vector<Partial> partials;
//            std::ofstream partialsCSV(partialsFolderPath / (pitchName + "_" + velocity + ".csv"));
//            partialsCSV << "partial,f\n";
            
            // 略低于 Nyquist 上限一点
            const double maxCandidateFreq = sampleRate * 0.48;
            const int maxCandidatePartial = static_cast<int>(maxCandidateFreq / f0);
            
            
            
//            partialsCSV.close();
            
        }
    }
    
}
