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

bool isTest = true;
int test_midi_n = 96;
std::string test_velocity = "110";

namespace fs = std::filesystem;
const fs::path generatedDataRoot = "/Users/opusarc/XCodeProjects/bBpiano/AcousticLab/StringFilterLab/Generated";
const fs::path spectrogramFolderPath = generatedDataRoot / "Spectrogram";
const fs::path splitFolder =
    "/Users/opusarc/XCodeProjects/bBpiano/AcousticLab/StringFilterLab/Samples/Pianoteq 9/SingleNoteSamples/Split";

const bool useSymmetricDoubleSidedLoss = true; // 双边反射各承担一半 loss, 如果以后改成单边滤波，这里改成 false。

double estimateBFromPartials(double f0, const std::vector<double>& partials) {
    double numerator = 0.0;
    double denominator = 0.0;

    for (double fp : partials) {
        if (fp <= 0.0 || f0 <= 0.0)
            continue;

        // 先粗略估计 partial index
        int p = static_cast<int>(std::round(fp / f0));
        if (p <= 0)
            continue;

        // 单个 partial 反算 B
        const double ratio = fp / (static_cast<double>(p) * f0);
        const double Bp = (ratio * ratio - 1.0) / (static_cast<double>(p) * p);

        if (!std::isfinite(Bp) || Bp <= 0.0)
            continue;

        // 高阶 partial 对 B 更敏感，给一点权重
        const double w = std::sqrt(static_cast<double>(p));

        numerator += w * Bp;
        denominator += w;
    }

    if (denominator <= 0.0)
        return 0.0;

    return numerator / denominator;
}


void PartialSpectrumAnalyzer::analyzer() {
    using std::cout;
    if(!fs::exists(generatedDataRoot)) return ;
    if(!fs::exists(splitFolder)) return ;
    fs::create_directories(spectrogramFolderPath);
    
    double sampleRate = 44100.0;
    int channel = 2;
    const int fftSize = 32768;
    const int hopSize = 512;
    
    std::vector<std::vector<PartialBeforeMerge>> all_partialBeforeMerge;
    
    for (const auto& split_entry : fs::directory_iterator(splitFolder)) {
        
        const std::string velocity = MyFile::findVelocity(split_entry.path().filename());
        std::vector<PartialBeforeMerge> velocity_partialBeforeMerge;
        
        if(isTest && velocity != test_velocity) continue ;
        
        for (const auto& entry : fs::directory_iterator(split_entry)) {
            
            const fs::path path = entry.path();
            if (!entry.is_regular_file() || path.extension() != ".wav")
                continue;
            
            const std::string filename = path.filename();
            const std::string pitchName = MyFile::findPitchName(filename);
            const double f0 = MyPitch::getFrequency(pitchName);
            const double midi_n = MyPitch::nameToMidi(pitchName);
            
            if(static_cast<int>(std::round(midi_n)) != test_midi_n) continue;
            
            // TODO: 这里临时关掉了沙盒 为了不用复制这些采样音频进入 resource，以后还是要打开的吧
            // 转成 pcm 信号
            const std::vector<float> pcm_stereo = MyDrWav::loadWav(path, sampleRate, channel);
                
            // pcm 双声道变 单声道 好做 SFTF
            const std::vector<float> pcm_mono = MyDrWav::downmixStereoToMono(pcm_stereo);
            
            // 快速傅立叶变换
            STFTResult stft_result = MyFFT::computeSpectrogram(pcm_mono, sampleRate, fftSize, hopSize);
        
            // spectrogram[frame][f_bin] -> amp
            const auto& spectrogram = stft_result.spectrogram;
            const auto& binFrequencies = stft_result.binFrequencies;

            if (spectrogram.empty() || binFrequencies.empty())
                continue;
            
            // 全局扫峰
            cout << "pitchName:" << pitchName << "\n";
            cout << "f0:" << f0 << "\n";
            cout << "spectrogram.size():" << spectrogram.size();
            PartialBeforeMerge partialBeforeMerge;
            partialBeforeMerge.f0 = f0;
            partialBeforeMerge.pitchName = pitchName;
            partialBeforeMerge.midi_n = midi_n;
            partialBeforeMerge.velocity = velocity;
            
//                        hold_duration + sustain_duration
//
//                        21~48:   12s +   8s = 20s
//                        48~59:    9s +   6s = 15s
//                        60~71:    6s +   4s = 10s
//                        72~83:    4s + 2.5s = 6.5s
//                        84~108:   3s + 1.5s = 4.5s
            
//            const double skipSec = 0.03; // 跳过最开始的 hammer transient
//            const double windowSec = 0.25;
            
            const double low_skipSec = 0.18; // 跳过最开始的 hammer transient
            const double low_windowSec = 0.78;

            const int low_startFrame = std::clamp(
                static_cast<int>(std::round(low_skipSec * sampleRate / hopSize)),
                0,
                static_cast<int>(spectrogram.size()) - 1
            );

            const int low_endFrame = std::clamp(
                low_startFrame + static_cast<int>(std::round(low_windowSec * sampleRate / hopSize)),
                low_startFrame,
                static_cast<int>(spectrogram.size()) - 1
            );
            
            const double mid_skipSec = 0.05; // 跳过最开始的 hammer transient
            const double mid_windowSec = 0.35;

            const int mid_startFrame = std::clamp(
                static_cast<int>(std::round(mid_skipSec * sampleRate / hopSize)),
                0,
                static_cast<int>(spectrogram.size()) - 1
            );

            const int mid_endFrame = std::clamp(
                mid_startFrame + static_cast<int>(std::round(mid_windowSec * sampleRate / hopSize)),
                mid_startFrame,
                static_cast<int>(spectrogram.size()) - 1
            );
            
            const double high_skipSec = 0.017; // 跳过最开始的 hammer transient
            const double high_windowSec = 0.1;

            const int high_startFrame = std::clamp(
                static_cast<int>(std::round(high_skipSec * sampleRate / hopSize)),
                0,
                static_cast<int>(spectrogram.size()) - 1
            );

            const int high_endFrame = std::clamp(
                high_startFrame + static_cast<int>(std::round(high_windowSec * sampleRate / hopSize)),
                high_startFrame,
                static_cast<int>(spectrogram.size()) - 1
            );
            

            float globalPeak = 0.0f; // 局部主峰强度

            for (int frame = mid_startFrame; frame <= mid_endFrame; ++frame) {
                for (float amp : spectrogram[frame]) {
                    globalPeak = std::max(globalPeak, amp);
                }
            }
            
            const float threshold = globalPeak * 0.001f;

            double last_bin = 0.0;

            std::vector<std::array<double, 2>> partial_candidate;
            
            for (int b = 0; b < static_cast<int>(spectrogram[0].size()); ++b) {
                const double f_bin = binFrequencies[b];

                if (f_bin < f0 * 0.5)
                    continue;

                float peak = 0.0f;
                
                if(f0 < 261.0) {
                    // 低音
                    for (int frame = low_startFrame; frame <= low_endFrame; ++frame) {
                        peak = std::max(peak, spectrogram[frame][b]);
                    }
                } else if(f0 < 1047) {
                    // 中音
                    for (int frame = mid_startFrame; frame <= mid_endFrame; ++frame) {
                        peak = std::max(peak, spectrogram[frame][b]);
                    }
                } else {
                    // 高音
                    for (int frame = high_startFrame; frame <= high_endFrame; ++frame) {
                        peak = std::max(peak, spectrogram[frame][b]);
                    }
                }

                if (peak > threshold) {
                    if (std::abs(f_bin - last_bin) > 20.0 && partial_candidate.size()) { // C0 最小的 partial 间距就是 20 左右
                        
                        double maxPeak = 0.0f;
                        double partial_f = 0.0f;
                        for(auto& p : partial_candidate) {
                            if(p[1] > maxPeak) {
                                maxPeak = p[1];
                                partial_f = p[0];
                            }
                        }
//                        cout << "[" << partial_f << "/" << maxPeak << "]";
                        
                        if(partial_f >= f0 - 20)
                            partialBeforeMerge.partials_part.push_back(partial_f);
                        
                        partial_candidate.clear();
//                        cout << "\n";
                    }

                    partial_candidate.push_back({f_bin, peak});
//                    cout << "[" << f_bin << "/" << peak << "]";
                    last_bin = f_bin;
                }
            }
            velocity_partialBeforeMerge.push_back(partialBeforeMerge);
        }
        all_partialBeforeMerge.push_back(velocity_partialBeforeMerge);
    }
    
    cout << "\n";
    
    // 融合正态分布
    for(int midi_n_i = 21; midi_n_i <= 108; midi_n_i++) {
        
        if(midi_n_i != test_midi_n) continue;
        
        std::vector<std::vector<double>> same_midiN_candidate;
        std::vector<double> biggestVector;
        int biggestVector_index = -1;
        
        for(int v_i = 0; v_i < all_partialBeforeMerge.size(); v_i++) {
            for(int j = 0; j < all_partialBeforeMerge[v_i].size(); j++) {
                if(static_cast<int>(std::round(all_partialBeforeMerge[v_i][j].midi_n)) == midi_n_i) {
                    same_midiN_candidate
                        .push_back(all_partialBeforeMerge[v_i][j].partials_part);
                    if(all_partialBeforeMerge[v_i][j].partials_part.size() > biggestVector.size()) {
                        biggestVector = all_partialBeforeMerge[v_i][j].partials_part;
                        biggestVector_index = static_cast<int>(same_midiN_candidate.size()) - 1;
                    }
                    break;
                }
            }
        }
        
        if(biggestVector_index >= 0 && biggestVector_index < same_midiN_candidate.size()) {
            same_midiN_candidate.erase(std::next(same_midiN_candidate.begin(), biggestVector_index));
        }
        
        for(int i = 0; i < biggestVector.size(); i++) {
            for(int j = 0; j < same_midiN_candidate.size(); j++) {
                for(int k = 0; k < same_midiN_candidate[j].size(); k++) {
                    if(abs(biggestVector[i] - same_midiN_candidate[j][k]) < 20.0) { // c0 的前两个 partials 之间的间隔就是 20
                        biggestVector[i] = (biggestVector[i] + same_midiN_candidate[j][k]) / 2.0f;
                    }
                }
            }
        }
        cout << "midi_n_i: " << midi_n_i << "\n";
        for(auto& p : biggestVector) {
            cout << "["<< p << "]" << "\n";
        }
        cout << "\n";
        
        double B = estimateBFromPartials(MyPitch::midiToFrequency(midi_n_i),
                                         biggestVector);

        std::cout << "B: " << B << "\n";
    }
    
    
}

