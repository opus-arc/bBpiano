//
//  PartialSpectrumAnalyzer.cpp
//  bBpiano
//
//  Created by opus arc on 2026/5/19.
//
//  This document is not AI-assisted.
//

#include "PartialSpectrumAnalyzer.hpp"

#include "../MyFile.hpp"
#include "../MyPitch.hpp"
#include "../MyDrWav.h"
#include "../MyFFT.h"
#include "../LinearRegression.hpp"

#include <filesystem>
#include <fstream>
#include <memory>
#include <algorithm>
#include <cmath>
#include <limits>
#include <vector>

bool isTestSpecificMidi = true;
bool isTestSpecificVelocity = false;
//int test_midi_n_begin = 53;
//int test_midi_n_end = 97;
int test_midi_n_begin = 21;
int test_midi_n_end = 108;
std::string test_velocity = "95";

namespace fs = std::filesystem;
const fs::path generatedDataRoot = "/Users/opusarc/XCodeProjects/bBpiano/AcousticLab/StringFilterLab/Generated";
const fs::path spectrogramFolderPath = generatedDataRoot / "Spectrogram";
const fs::path splitFolder =
    "/Users/opusarc/XCodeProjects/bBpiano/AcousticLab/StringFilterLab/Samples/Pianoteq 9/SingleNoteSamples/Split";

//const bool useSymmetricDoubleSidedLoss = true; // 双边反射各承担一半 loss, 如果以后改成单边滤波，这里改成 false。

double estimateBFromPartials(double f0, const std::vector<double>& partials);
double estimateSigmaFromPartials(
    const std::vector<std::vector<float>>& spectrogram,
    const std::vector<float>& binFrequencies,
    double f_predict,
    double f0,
    double sampleRate,
    int fftSize,
    int hopSize,
    double framesPerSecond
);
std::array<QuadraticRegressionResult, 109> smoothLossABCByMidi(
        const std::array<QuadraticRegressionResult, 109>& raw,
        int midiBegin = 21,
        int midiEnd = 108
);
std::array<QuadraticRegressionResult, 109> makeRawLossFitByMidi();
void writeLossFilterConstantsCSV();
std::array<double, 109> makeAccurateBByMidi();
void writeDispersionFilterConstantsCSV();

void testABC();



void PartialSpectrumAnalyzer::analyzer() {
    testABC();
    return ;
    
    
    using std::cout;
    if(!fs::exists(generatedDataRoot)) return ;
    if(!fs::exists(splitFolder)) return ;
    fs::create_directories(spectrogramFolderPath);
    
    double sampleRate = 44100.0;
    int channel = 2;
    const int fftSize = 32768;
    const int hopSize = 512;
    const double framesPerSecond = sampleRate / hopSize;
    const double binHz = sampleRate / fftSize;
    
    std::vector<std::vector<Partial_Scan>> all_partialBeforeMerge;
    
    AllPianoSpectrogram all_piano_spectrogram;
    
    std::vector<float> binFrequencies;
    
    for (const auto& split_entry : fs::directory_iterator(splitFolder)) {
        
        const std::string velocity = MyFile::findVelocity(split_entry.path().filename());
        std::vector<Partial_Scan> velocity_partialBeforeMerge;
        
        PianoSpectrogram ps;
        ps.velocity = velocity;
        
        if((velocity != test_velocity) && isTestSpecificVelocity) continue;
        
        for (const auto& entry : fs::directory_iterator(split_entry)) {
            
            const fs::path path = entry.path();
            if (!entry.is_regular_file() || path.extension() != ".wav")
                continue;
            
            const std::string filename = path.filename();
            const std::string pitchName = MyFile::findPitchName(filename);
            const double f0 = MyPitch::getFrequency(pitchName);
            const double midi_n = MyPitch::nameToMidi(pitchName);
            
            if(isTestSpecificMidi && (midi_n <= test_midi_n_begin || midi_n >= test_midi_n_end)) continue;
            
            
            // TODO: 这里临时关掉了沙盒 为了不用复制这些采样音频进入 resource，以后还是要打开的吧
            // 转成 pcm 信号
            const std::vector<float> pcm_stereo = MyDrWav::loadWav(path, sampleRate, channel);
                
            // pcm 双声道变 单声道 好做 SFTF
            const std::vector<float> pcm_mono = MyDrWav::downmixStereoToMono(pcm_stereo);
            
            // 快速傅立叶变换
            std::unique_ptr<STFTResult> stft_result = // 这东西会占用好几个G的内存，还是用唯一指针来管理，不允许复制
                std::make_unique<STFTResult>(
                    MyFFT::computeSpectrogram(pcm_mono, sampleRate, fftSize, hopSize)
                );
        
            // spectrogram[frame][f_bin] -> amp
            const auto& spectrogram = stft_result->spectrogram;
            if(binFrequencies.size() == 0)
                binFrequencies = stft_result->binFrequencies;

            if (spectrogram.empty() || binFrequencies.empty())
                continue;
            
            // 全局扫峰
            cout << "pitchName:" << pitchName << "\n";
            cout << "f0:" << f0 << "\n";
            cout << "spectrogram.size():" << spectrogram.size() << "\n";
            Partial_Scan partialBeforeMerge;
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
            
            BeginFrameAndEndFrame B_E(double(framesPerSecond), spectrogram.size());
            
            float globalPeak = 0.0f; // 全局主峰强度 与全局 gain 相适配的

            for (int frame = B_E.get("begin", "mid"); frame <= B_E.get("end", "mid"); ++frame) {
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
                    for (int frame = B_E.get("begin", "low"); frame <= B_E.get("end", "low"); ++frame) {
                        peak = std::max(peak, spectrogram[frame][b]);
                    }
                } else if(f0 < 1047) {
                    // 中音
                    for (int frame = B_E.get("begin", "mid"); frame <= B_E.get("end", "mid"); ++frame) {
                        peak = std::max(peak, spectrogram[frame][b]);
                    }
                } else {
                    // 高音
                    for (int frame = B_E.get("begin", "high"); frame <= B_E.get("end", "high"); ++frame) {
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
            
            KeySpectrum ks;
            ks.velocity = velocity;
            ks.f0 = f0;
            ks.midi_n = midi_n;
            ks.pitchName = pitchName;
            ks.spectrogram = std::move(spectrogram);
            ps.ks.push_back(std::move(ks));
        }
        
        all_piano_spectrogram.aps.push_back(std::move(ps));
        
        all_partialBeforeMerge.push_back(velocity_partialBeforeMerge);
    }
    
    cout << "\n";
    
    // 融合正态分布
    std::vector<std::array<double, 2>> Bpoints;
    
    for(int midi_n_i = 21; midi_n_i <= 108; midi_n_i++) {
        
        if(isTestSpecificMidi && (midi_n_i <= test_midi_n_begin || midi_n_i >= test_midi_n_end)) continue;
        
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
//        for(auto& p : biggestVector) {
////            cout << "["<< p << "]" << "\n";
//        }
//        cout << "\n";
        
        double B = estimateBFromPartials(MyPitch::midiToFrequency(midi_n_i),
                                         biggestVector);

        std::cout << "B: " << B << "\n";
        
        // 取对数 线性回归 但只用 MIDI 53–96 因为因为这一段最容易拟合准确
        if (std::isfinite(B) && B > 0.0 && midi_n_i >= 53 && midi_n_i <= 96) {
            Bpoints.push_back({
                static_cast<double>(midi_n_i),
                std::log(B)
            });
        }
    }
    
    LinearRegressionResult lrr = LinearRegression::fit(Bpoints);
    
    cout << "b: " << lrr.b << ", k: " << lrr.k << ", n: " << lrr.n << ", r2: " << lrr.r2;
    
    std::vector<double> accurateB(109);

    for (int midi = 21; midi <= 108; ++midi) {
        accurateB[midi] = std::exp(lrr.k * midi + lrr.b);
        cout << "midi: " << midi << ", accurateB[midi]: " << accurateB[midi] << "\n";
    }
    
    // 重新用拟合出的常数来算 partials
    for(int midi = 21; midi <= 108; ++midi) {
        if(isTestSpecificMidi && (midi <= test_midi_n_begin || midi >= test_midi_n_end)) continue;
        double f0 = MyPitch::midiToFrequency(midi);
        
        // Nyquist 限制
        int nyquistLimit = static_cast<int>(std::floor((sampleRate * 0.5) / f0));
        
        // partial 至少占 3 个 FFT bin
        double binResolution = sampleRate / fftSize;
        int resolutionLimit = static_cast<int>(std::floor(f0 / (3.0 * binResolution)));
        int maxPartial = std::min(nyquistLimit, resolutionLimit);
        
        // 先把每一个力度上的 sigma 都算一遍打印出来看下 然后再想咋聚合
        std::vector<double> sigmas;
        std::vector<std::array<double, 2>> SigmaPoints;
        for(int p = 1; p <= maxPartial; p++) {
            double f_predict = p * f0 * std::sqrt(1.0 + accurateB[midi] * p * p);
            cout << "midi: " << midi << ", partial: " << p << ", f_predict: " << f_predict << "\n";
            int radiusBin = 20;
            int centerBin = static_cast<int>(std::round(f_predict / binResolution));
            for(int v = 0; v < all_piano_spectrogram.aps.size(); v++) {
                KeySpectrum* targetKeySpectrum = nullptr;
                for(auto& ks : all_piano_spectrogram.aps[v].ks) {
                    if(static_cast<int>(std::round(ks.midi_n)) == midi) {
                        targetKeySpectrum = &ks;
                        break;
                    }
                }
                if(targetKeySpectrum == nullptr) continue;
                
                auto& spectrogram = targetKeySpectrum->spectrogram;
                if(spectrogram.empty() || spectrogram[0].empty()) continue;
                
                int bin_begin = std::max(0, centerBin - radiusBin);
                int bin_end = std::min(static_cast<int>(spectrogram[0].size()) - 1,
                                       centerBin + radiusBin
                                       );
                if(bin_begin > bin_end) continue;
                
                BeginFrameAndEndFrame B_E(double(framesPerSecond), spectrogram.size());
                // 先计算全局主峰强度
                // 但这一段遍历也很要命
                float globalPeak = 0.0f;
                for(int frame = B_E.get("begin", "mid"); frame <= B_E.get("end", "mid"); frame++) {
                    for (auto& amp : spectrogram[frame]) {
                        globalPeak = std::max(globalPeak, amp);
                    }
                }
                const float threshold = globalPeak * 0.001f;
                // 找最可能的位置
                double maxPeak = 0.0f;
                int partial_bin_index = 0;
                double partial_bin_f = 0.0;
                for(int bin = bin_begin; bin <= bin_end; bin++) {
                    const double f_bin = binFrequencies[bin];

                    if (f_bin < f0 * 0.5)
                        continue;

                    float peak = 0.0f;
                    
                    if(f0 < 261.0) {
                        // 低音
                        for (int frame = B_E.get("begin", "low"); frame <= B_E.get("end", "low"); ++frame) {
                            peak = std::max(peak, spectrogram[frame][bin]);
                        }
                    } else if(f0 < 1047) {
                        // 中音
                        for (int frame = B_E.get("begin", "mid"); frame <= B_E.get("end", "mid"); ++frame) {
                            peak = std::max(peak, spectrogram[frame][bin]);
                        }
                    } else {
                        // 高音
                        for (int frame = B_E.get("begin", "high"); frame <= B_E.get("end", "high"); ++frame) {
                            peak = std::max(peak, spectrogram[frame][bin]);
                        }
                    }
                    if(peak > maxPeak) {
                        maxPeak = peak;
                        partial_bin_index = bin;
                        partial_bin_f = f_bin;
                    }
                }
                
                double sigma = estimateSigmaFromPartials(
                    spectrogram,
                    binFrequencies,
                    f_predict,
                    f0,
                    sampleRate,
                    fftSize,
                    hopSize,
                    framesPerSecond
                );
                if(sigma <= 0.0 || !std::isfinite(sigma)) continue;
                cout << sigma << "\n";
                sigmas.push_back(sigma);
            }
            std::sort(sigmas.begin(), sigmas.end());
            double median = (sigmas[std::ceil(sigmas.size() / 2)] + sigmas[std::floor(sigmas.size() / 2)]) / 2.0;
            double mean_sigma = median;
            for(auto& s : sigmas) {
                if(std::abs(s - median) <= 2.0) {
                    mean_sigma += s;
                    mean_sigma /= 2.0;
                }
            }
            cout << "mean_sigma: " << mean_sigma << "\n";
            SigmaPoints.push_back({static_cast<double>(p), mean_sigma});
        }
        
        if(SigmaPoints.size() < 2) continue;
        
        QuadraticRegressionResult qlrr = LinearRegression::fit2(SigmaPoints);
        cout << "r2: " << qlrr.r2 << ", a: " << qlrr.a << ", b: " << qlrr.b << ", c: " << qlrr.c << ", n: " << qlrr.n<< "\n";
    }
    all_piano_spectrogram.aps.clear();
    all_piano_spectrogram.aps.shrink_to_fit();
    

}

void testABC() {
//    writeLossFilterConstantsCSV();
    writeDispersionFilterConstantsCSV();
//    auto rawLossFit = makeRawLossFitByMidi();
//    smoothLossABCByMidi(rawLossFit);
}




// MARK: AI Mixed


void writeDispersionFilterConstantsCSV() {
    
    const std::filesystem::path outputPath =
        "/Users/opusarc/XCodeProjects/bBpiano/AcousticLab/StringFilterLab/Generated/dispersion_filter_constants.csv";
    
    std::filesystem::create_directories(outputPath.parent_path());
    
    std::ofstream out(outputPath, std::ios::trunc);
    
    if (!out.is_open()) {
        std::cerr << "Failed to open: " << outputPath << "\n";
        return;
    }
    
    out << "key,f0,B,order,a,fitError,pointCount\n";
    
    auto accurateB = makeAccurateBByMidi();
    
    constexpr double sampleRate = 44100.0;
    
    auto firstOrderAllpassGroupDelay = [](double omega, double a) {
        return (1.0 - a * a) /
               (1.0 + a * a + 2.0 * a * std::cos(omega));
    };
    
    for (int midi = 21; midi <= 108; ++midi) {
        
        const double f0 = MyPitch::midiToFrequency(midi);
        const double B = accurateB[midi];
        
        const int maxPartial =
            std::max(2, std::min(12, static_cast<int>((sampleRate * 0.45) / f0)));
        
        double bestA = 0.0;
        double bestError = std::numeric_limits<double>::max();
        
        for (double a = -0.95; a <= 0.95; a += 0.0005) {
            
            const double omega1 = 2.0 * M_PI * f0 / sampleRate;
            const double tau1 = firstOrderAllpassGroupDelay(omega1, a);
            
            double error = 0.0;
            int pointCount = 0;
            
            for (int p = 2; p <= maxPartial; ++p) {
                
                const double fp =
                    p * f0 * std::sqrt(1.0 + B * p * p);
                
                if (fp >= sampleRate * 0.45)
                    continue;
                
                const double omega = 2.0 * M_PI * fp / sampleRate;
                
                const double targetDelay =
                    (static_cast<double>(p) * sampleRate / (2.0 * fp)) -
                    (sampleRate / (2.0 * f0));
                
                const double fittedDelay =
                    firstOrderAllpassGroupDelay(omega, a) - tau1;
                
                const double w = 1.0 / static_cast<double>(p);
                const double e = fittedDelay - targetDelay;
                
                error += w * e * e;
                ++pointCount;
            }
            
            if (pointCount <= 0)
                continue;
            
            error /= static_cast<double>(pointCount);
            
            if (error < bestError) {
                bestError = error;
                bestA = a;
            }
        }
        
        out
            << midi << ","
            << f0 << ","
            << B << ","
            << 1 << ","
            << bestA << ","
            << bestError << ","
            << maxPartial - 1
            << "\n";
    }
    
    out.close();
    
    std::cout << "dispersion filter constants written to: "
              << outputPath << "\n";
}

void writeLossFilterConstantsCSV() {
    
    const fs::path outputPath =
        "/Users/opusarc/XCodeProjects/bBpiano/AcousticLab/StringFilterLab/Generated/loss_filter_constants.csv";

    fs::create_directories(outputPath.parent_path());

    // std::ios::trunc 会清空上一次数据
    std::ofstream out(outputPath, std::ios::trunc);

    if (!out.is_open()) {
        std::cerr << "Failed to open: " << outputPath << "\n";
        return;
    }

    out << "key,pitch,f0,g,a1,pointCount,fitError\n";

    auto rawLossFit = makeRawLossFitByMidi();
    auto smoothLossFit = smoothLossABCByMidi(rawLossFit);

    constexpr double sampleRate = 44100.0;
    constexpr bool useDoubleSidedLoss = true;

    for (int midi = 21; midi <= 108; ++midi) {
        
        const auto& p = smoothLossFit[midi];

        const double f0 = MyPitch::midiToFrequency(midi);
//        const std::string pitch = MyPitch::midiToName(midi);

        const int nyquistLimit =
            static_cast<int>(std::floor((sampleRate * 0.5) / f0));

        const int maxPartial = std::min(nyquistLimit, 32);

        std::vector<std::array<double, 2>> targets;

        for (int partial = 1; partial <= maxPartial; ++partial) {
            
            const double f = f0 * partial;
            const double x = f / 1000.0;

            double sigma =
                p.a +
                p.b * x +
                0.35 * p.c * x * x;

            const double softLimit = 80.0;
            if (sigma > softLimit) {
                sigma = softLimit + std::sqrt(sigma - softLimit);
            }

            sigma = std::clamp(sigma, 0.05, 180.0);

            // 一次完整 loop 的目标衰减
            double targetGain = std::exp(-sigma / f0);

            // 如果左右边界各滤一次，每个 filter 只承担一半 loss
            if (useDoubleSidedLoss) {
                targetGain = std::sqrt(targetGain);
            }

            targets.push_back({f, targetGain});
        }

        double bestG = 1.0;
        double bestA1 = 0.0;
        double bestError = std::numeric_limits<double>::max();

        for (double a1 = -0.98; a1 <= 0.98; a1 += 0.0005) {
            
            double numerator = 0.0;
            double denominator = 0.0;

            for (const auto& point : targets) {
                
                const double f = point[0];
                const double target = point[1];

                const double omega = 2.0 * M_PI * f / sampleRate;

                const double base =
                    (1.0 + a1) /
                    std::sqrt(1.0 + a1 * a1 + 2.0 * a1 * std::cos(omega));

                numerator += base * target;
                denominator += base * base;
            }

            if (denominator <= 0.0)
                continue;

            const double g = numerator / denominator;

            double error = 0.0;

            for (const auto& point : targets) {
                
                const double f = point[0];
                const double target = point[1];

                const double omega = 2.0 * M_PI * f / sampleRate;

                const double h =
                    g *
                    (1.0 + a1) /
                    std::sqrt(1.0 + a1 * a1 + 2.0 * a1 * std::cos(omega));

                const double e = h - target;
                error += e * e;
            }

            error /= static_cast<double>(targets.size());

            if (error < bestError) {
                bestError = error;
                bestG = g;
                bestA1 = a1;
            }
        }

        out
            << midi << ","
//            << pitch << ","
            << f0 << ","
            << bestG << ","
            << bestA1 << ","
            << targets.size() << ","
            << bestError
            << "\n";
    }

    out.close();

    std::cout << "loss filter constants written to: "
              << outputPath << "\n";
}

std::array<QuadraticRegressionResult, 109> smoothLossABCByMidi(
    const std::array<QuadraticRegressionResult, 109>& raw,
    int midiBegin,
    int midiEnd
) {
    std::array<QuadraticRegressionResult, 109> result{};

    const double sigmaMidi = 7.0;

    auto softQuality = [](double r2) {
        if (!std::isfinite(r2)) return 0.0;
        const double q = std::clamp((r2 - 0.40) / 0.60, 0.0, 1.0);
        return q * q * q;
    };

    auto nWeightOf = [](double n) {
        if (!std::isfinite(n)) return 1.0;
        return std::sqrt(std::max(1.0, n));
    };

    for (int midi = midiBegin; midi <= midiEnd; ++midi) {
        double wa = 0.0, wb = 0.0, wc = 0.0;
        double wr2 = 0.0, wn = 0.0;
        double wsumA = 0.0, wsumB = 0.0, wsumC = 0.0;
        double wsumMeta = 0.0;

        for (int sourceMidi = midiBegin; sourceMidi <= midiEnd; ++sourceMidi) {
            const auto& p = raw[sourceMidi];

            if (!std::isfinite(p.a) || !std::isfinite(p.b) || !std::isfinite(p.c))
                continue;

            const double d = static_cast<double>(midi - sourceMidi);

            const double distanceWeight =
                std::exp(-(d * d) / (2.0 * sigmaMidi * sigmaMidi));

            const double qWeight = softQuality(p.r2);
            const double nWeight = nWeightOf(double(p.n));
            const double baseWeight = distanceWeight * qWeight * nWeight;

            if (baseWeight <= 0.0)
                continue;

            double aWeight = baseWeight;
            double bWeight = baseWeight;
            double cWeight = baseWeight;

            // 高音区 b 容易因为 partial 数量少而病态偏大，温和降权。
            if (sourceMidi >= 80)
                bWeight *= 0.35;

            // 高音区主要由二次项控制高频 loss，略微提高 c 的可信度。
            if (sourceMidi >= 80)
                cWeight *= 1.25;

            wa += aWeight * p.a;
            wb += bWeight * p.b;
            wc += cWeight * p.c;

            wsumA += aWeight;
            wsumB += bWeight;
            wsumC += cWeight;

            wr2 += baseWeight * p.r2;
            wn += baseWeight * p.n;
            wsumMeta += baseWeight;
        }

        if (wsumA <= 0.0 || wsumB <= 0.0 || wsumC <= 0.0)
            continue;

        result[midi].a = wa / wsumA;
        result[midi].b = wb / wsumB;
        result[midi].c = wc / wsumC;

        if (wsumMeta > 0.0) {
            result[midi].r2 = wr2 / wsumMeta;
            result[midi].n = wn / wsumMeta;
        }
    }
    
    for (int midi = midiBegin; midi <= midiEnd; ++midi) {
        const auto& p = result[midi];

        std::cout
            << "midi: " << midi
            << ", a: " << p.a
            << ", b: " << p.b
            << ", c: " << p.c
            << ", r2: " << p.r2
            << ", n: " << p.n
            << "\n";

        const double f0 = MyPitch::midiToFrequency(midi);

        std::cout << "    sigma test:\n";

        for (int partial = 1; partial <= 8; ++partial) {
            const double f = f0 * partial;
            const double x = f / 1000.0;

            double sigma =
                p.a +
                p.b * x +
                0.35 * p.c * x * x;

            const double softLimit = 80.0;
            if (sigma > softLimit) {
                sigma = softLimit + std::sqrt(sigma - softLimit);
            }

            sigma = std::clamp(sigma, 0.05, 180.0);

            std::cout
                << "        partial: " << partial
                << ", f: " << f
                << ", sigma: " << sigma
                << "\n";
        }

        std::cout << "\n";
    }

    return result;

    return result;
}


double estimateBFromPartials(double f0, const std::vector<double>& partials) {
    double sum = 0.0;
    int count = 0;

    for (double fp : partials) {
        if (fp <= 0.0 || f0 <= 0.0)
            continue;

        int p = static_cast<int>(std::round(fp / f0));
        if (p <= 0)
            continue;

        const double ratio = fp / (static_cast<double>(p) * f0);
        const double Bp = (ratio * ratio - 1.0) / (static_cast<double>(p) * p);

        if (!std::isfinite(Bp) || Bp <= 0.0)
            continue;

        sum += Bp;
        count++;
    }

    if (count == 0)
        return 0.0;

    return sum / static_cast<double>(count);
}


double estimateSigmaFromPartials(
    const std::vector<std::vector<float>>& spectrogram,
    const std::vector<float>& binFrequencies,
    double f_predict,
    double f0,
    double sampleRate,
    int fftSize,
    int hopSize,
    double framesPerSecond
) {
    if (spectrogram.empty() || spectrogram[0].empty() || binFrequencies.empty()) {
        return 0.0;
    }
    if (f_predict <= 0.0 || f0 <= 0.0 || sampleRate <= 0.0 || fftSize <= 0 || hopSize <= 0) {
        return 0.0;
    }

    const int frameCount = static_cast<int>(spectrogram.size());
    const int binCount = static_cast<int>(std::min(
        spectrogram[0].size(),
        static_cast<std::size_t>(binFrequencies.size())
    ));

    if (frameCount <= 2 || binCount <= 2) {
        return 0.0;
    }

    const double binResolution = sampleRate / static_cast<double>(fftSize);
    const int centerBin = static_cast<int>(std::round(f_predict / binResolution));
    const int maxBin = binCount - 1;

    if (centerBin < 0 || centerBin > maxBin) {
        return 0.0;
    }

    const bool isHighRegister = f0 >= 1047.0;
    const bool isLowRegister = f0 < 261.0;
    const int searchRadiusBins = isHighRegister ? 20 : (isLowRegister ? 8 : 12);
    const int localTrackRadiusBins = isHighRegister ? 8 : (isLowRegister ? 3 : 5);
    const int noiseGuardBins = std::max(3, localTrackRadiusBins + 2);

    const int bin_begin = std::max(0, centerBin - searchRadiusBins);
    const int bin_end = std::min(maxBin, centerBin + searchRadiusBins);
    if (bin_begin > bin_end) {
        return 0.0;
    }

    BeginFrameAndEndFrame B_E(framesPerSecond, spectrogram.size());

    std::string band = "mid";
    if (isLowRegister) {
        band = "low";
    } else if (isHighRegister) {
        band = "high";
    }

    int beginFrame = B_E.get("begin", band);
    int endFrame = B_E.get("end", band);
    beginFrame = std::clamp(beginFrame, 0, frameCount - 1);
    endFrame = std::clamp(endFrame, beginFrame, frameCount - 1);

    // 1. 在当前音区窗口内，先找到预测频率附近真正最强的 reference peak。
    double referencePeak = 0.0;
    int referencePeakBin = centerBin;
    int referencePeakFrame = beginFrame;

    for (int frame = beginFrame; frame <= endFrame; ++frame) {
        for (int bin = bin_begin; bin <= bin_end; ++bin) {
            const double amp = spectrogram[frame][bin];
            if (amp > referencePeak) {
                referencePeak = amp;
                referencePeakBin = bin;
                referencePeakFrame = frame;
            }
        }
    }

    if (referencePeak <= 0.0) {
        return 0.0;
    }

    // 2. Prominence 检查：reference peak 必须明显高于附近背景。
    std::vector<double> noiseCandidates;
    const int noiseBegin = std::max(0, referencePeakBin - searchRadiusBins);
    const int noiseEnd = std::min(maxBin, referencePeakBin + searchRadiusBins);

    for (int bin = noiseBegin; bin <= noiseEnd; ++bin) {
        if (std::abs(bin - referencePeakBin) <= noiseGuardBins) {
            continue;
        }

        double localMax = 0.0;
        for (int frame = beginFrame; frame <= endFrame; ++frame) {
            localMax = std::max(localMax, static_cast<double>(spectrogram[frame][bin]));
        }

        if (localMax > 0.0) {
            noiseCandidates.push_back(localMax);
        }
    }

    if (!noiseCandidates.empty()) {
        std::sort(noiseCandidates.begin(), noiseCandidates.end());
        const double noiseFloor = noiseCandidates[noiseCandidates.size() / 2];
        const double requiredProminence = isHighRegister ? 2.0 : 3.0;
        if (noiseFloor > 0.0 && referencePeak < noiseFloor * requiredProminence) {
            return 0.0;
        }
    }

    // 3. 从 reference peak 后面开始拟合，跳过 attack/transient。
    const int skipFrames = std::max(1, static_cast<int>(std::round(0.03 * framesPerSecond)));
    int fitBeginFrame = std::min(referencePeakFrame + skipFrames, endFrame);

    // 如果 reference peak 太晚，说明这个 peak 可能不是自然衰减段里的 partial。
    if (fitBeginFrame >= endFrame) {
        return 0.0;
    }

    const int trackBeginBin = std::max(0, referencePeakBin - localTrackRadiusBins);
    const int trackEndBin = std::min(maxBin, referencePeakBin + localTrackRadiusBins);

    std::vector<std::array<double, 2>> envelopePoints;
    const double stopAmp = referencePeak * 0.001;
    const int maxFitFrames = std::max(4, static_cast<int>(std::round(1.2 * framesPerSecond)));
    const int fitEndFrame = std::min(endFrame, fitBeginFrame + maxFitFrames);

    for (int frame = fitBeginFrame; frame <= fitEndFrame; ++frame) {
        double framePeak = 0.0;

        // 每一帧都重新在 reference peak 附近找局部峰，而不是固定一个 bin。
        for (int bin = trackBeginBin; bin <= trackEndBin; ++bin) {
            framePeak = std::max(framePeak, static_cast<double>(spectrogram[frame][bin]));
        }

        if (framePeak <= stopAmp) {
            break;
        }

        const double t = static_cast<double>(frame - fitBeginFrame) *
                         static_cast<double>(hopSize) / sampleRate;
        envelopePoints.push_back({t, std::log(framePeak)});
    }

    const std::size_t minFittedFrames = isHighRegister ? 4 : 6;
    if (envelopePoints.size() < minFittedFrames) {
        return 0.0;
    }

    const LinearRegressionResult result = LinearRegression::fit(envelopePoints);
    const double sigma = -result.k;

    if (!std::isfinite(sigma) || sigma <= 0.0) {
        return 0.0;
    }

    // 4. 质量门槛：明显不是指数衰减的 envelope 直接丢掉。
    const double minR2 = isHighRegister ? 0.55 : 0.70;
    if (result.r2 < minR2) {
        return 0.0;
    }

    // 5. 极端 sigma 通常来自吃到 transient / resonance bump / 噪声，不进入 loss 点。
    const double maxSigma = isHighRegister ? 80.0 : 50.0;
    if (sigma > maxSigma) {
        return 0.0;
    }

    return sigma;
}

std::array<double, 109> makeAccurateBByMidi() {
    
    std::array<double, 109> accurateB{};
    
    accurateB[21] = 6.87669e-06;
    accurateB[22] = 7.57327e-06;
    accurateB[23] = 8.34042e-06;
    accurateB[24] = 9.18528e-06;
    accurateB[25] = 1.01157e-05;
    accurateB[26] = 1.11404e-05;
    accurateB[27] = 1.22689e-05;
    accurateB[28] = 1.35117e-05;
    accurateB[29] = 1.48804e-05;
    accurateB[30] = 1.63877e-05;
    accurateB[31] = 1.80477e-05;
    accurateB[32] = 1.98759e-05;
    accurateB[33] = 2.18892e-05;
    accurateB[34] = 2.41065e-05;
    accurateB[35] = 2.65484e-05;
    accurateB[36] = 2.92377e-05;
    accurateB[37] = 3.21994e-05;
    accurateB[38] = 3.5461e-05;
    accurateB[39] = 3.90531e-05;
    accurateB[40] = 4.30091e-05;
    accurateB[41] = 4.73657e-05;
    accurateB[42] = 5.21637e-05;
    accurateB[43] = 5.74477e-05;
    accurateB[44] = 6.3267e-05;
    accurateB[45] = 6.96757e-05;
    accurateB[46] = 7.67336e-05;
    accurateB[47] = 8.45064e-05;
    accurateB[48] = 9.30666e-05;
    accurateB[49] = 0.000102494;
    accurateB[50] = 0.000112876;
    accurateB[51] = 0.00012431;
    accurateB[52] = 0.000136902;
    accurateB[53] = 0.00015077;
    accurateB[54] = 0.000166042;
    accurateB[55] = 0.000182862;
    accurateB[56] = 0.000201385;
    accurateB[57] = 0.000221785;
    accurateB[58] = 0.000244251;
    accurateB[59] = 0.000268993;
    accurateB[60] = 0.000296241;
    accurateB[61] = 0.000326249;
    accurateB[62] = 0.000359297;
    accurateB[63] = 0.000395692;
    accurateB[64] = 0.000435774;
    accurateB[65] = 0.000479917;
    accurateB[66] = 0.000528531;
    accurateB[67] = 0.000582069;
    accurateB[68] = 0.00064103;
    accurateB[69] = 0.000705964;
    accurateB[70] = 0.000777476;
    accurateB[71] = 0.000856232;
    accurateB[72] = 0.000942965;
    accurateB[73] = 0.00103848;
    accurateB[74] = 0.00114368;
    accurateB[75] = 0.00125953;
    accurateB[76] = 0.00138711;
    accurateB[77] = 0.00152762;
    accurateB[78] = 0.00168237;
    accurateB[79] = 0.00185279;
    accurateB[80] = 0.00204047;
    accurateB[81] = 0.00224716;
    accurateB[82] = 0.00247479;
    accurateB[83] = 0.00272547;
    accurateB[84] = 0.00300156;
    accurateB[85] = 0.0033056;
    accurateB[86] = 0.00364045;
    accurateB[87] = 0.00400921;
    accurateB[88] = 0.00441533;
    accurateB[89] = 0.00486259;
    accurateB[90] = 0.00535515;
    accurateB[91] = 0.00589761;
    accurateB[92] = 0.00649502;
    accurateB[93] = 0.00715294;
    accurateB[94] = 0.0078775;
    accurateB[95] = 0.00867547;
    accurateB[96] = 0.00955426;
    accurateB[97] = 0.0105221;
    accurateB[98] = 0.0115879;
    accurateB[99] = 0.0127617;
    accurateB[100] = 0.0140545;
    accurateB[101] = 0.0154781;
    accurateB[102] = 0.017046;
    accurateB[103] = 0.0187727;
    accurateB[104] = 0.0206743;
    accurateB[105] = 0.0227685;
    accurateB[106] = 0.0250749;
    accurateB[107] = 0.0276149;
    accurateB[108] = 0.0304122;
    
    return accurateB;
}


std::array<QuadraticRegressionResult, 109> makeRawLossFitByMidi() {
    std::array<QuadraticRegressionResult, 109> data{};

    auto set = [&](int midi, double r2, double a, double b, double c, int n) {
        data[midi].r2 = r2;
        data[midi].a = a;
        data[midi].b = b;
        data[midi].c = c;
        data[midi].n = n;
    };

    set(21, 0.841597, 0.000839864, -0.00704851, 0.74194, 7);
    set(22, 0.730765, 0.0167589, -0.103913, 0.50521, 7);
    set(23, 0.792249, 0.000397848, -0.00360043, 0.43319, 8);
    set(24, 0.674653, 0.000225881, -0.00218775, 0.369733, 8);
    set(25, 0.828794, 0.00130816, -0.00991031, 0.407968, 9);
    set(26, 0.719011, 2.77309e-05, -0.000338481, 0.363884, 9);
    set(27, 0.696378, 0.000432582, -0.00375548, 0.289494, 10);
    set(28, 0.932496, 0.000505199, 0.0341535, 0.294346, 10);
    set(29, 0.781587, 0.000170963, 0.00268077, 0.525017, 11);
    set(30, 0.598602, 0.00877207, -0.0880265, 0.888701, 12);
    set(31, 0.905092, 0.00530365, -0.0398318, 0.72032, 12);
    set(32, 0.703191, -6.26454e-05, 0.0386607, 0.627317, 13);
    set(33, 0.710778, -0.0017455, 0.0584114, 0.856519, 14);
    set(34, 0.79946, 0.00299571, -0.00555117, 1.45733, 15);
    set(35, 0.838689, 0.00873248, -0.0945727, 1.64619, 16);
    set(36, 0.966038, 0.0021834, -0.000628879, 1.17528, 17);
    set(37, 0.666068, 0.00657648, -0.091297, 2.03604, 18);
    set(38, 0.889722, 0.00169997, 0.0682128, 1.18104, 19);
    set(39, 0.930781, 0.00182389, 0.0596403, 1.12997, 20);
    set(40, 0.730528, 0.00137592, 0.0183103, 1.73734, 21);
    set(41, 0.837861, 0.00232507, 0.0259511, 1.39311, 22);
    set(42, 0.937013, 0.00220308, 0.0380306, 1.25177, 24);
    set(43, 0.913698, 0.00177396, 0.0366241, 1.55162, 25);
    set(44, 0.88138, 0.000963186, 0.0362638, 1.82587, 27);
    set(45, 0.883249, -0.00248238, 0.166919, 0.745332, 28);
    set(46, 0.96627, -0.00351996, 0.200735, 0.646252, 30);
    set(47, 0.964809, -0.00225755, 0.152759, 0.775125, 32);
    set(48, 0.953203, -0.00280265, 0.184635, 0.769271, 34);
    set(49, 0.978866, -0.0031949, 0.191071, 1.08758, 36);
    set(50, 0.939674, -0.00321806, 0.190138, 1.02358, 38);
    set(51, 0.97081, -0.00301574, 0.194388, 1.04112, 40);
    set(52, 0.923058, -0.00258776, 0.161411, 1.39192, 43);
    set(53, 0.902595, -0.00162848, 0.132046, 1.65393, 45);
    set(54, 0.926154, -0.00194949, 0.132377, 1.49532, 48);
    set(55, 0.961731, -0.00169484, 0.148088, 1.55497, 51);
    set(56, 0.90664, -0.00130397, 0.112381, 2.08825, 54);
    set(57, 0.793296, -0.00134281, 0.102007, 2.17711, 57);
    set(58, 0.813575, -0.00108772, 0.0919178, 2.20889, 61);
    set(59, 0.900706, -0.00140497, 0.137249, 2.2547, 64);
    set(60, 0.85573, -0.00161353, 0.147182, 2.37759, 68);
    set(61, 0.698827, -0.00095187, 0.0879723, 3.15992, 72);
    set(62, 0.659523, -0.00092977, 0.0839801, 3.33337, 70);
    set(63, 0.867099, -0.00153745, 0.137166, 3.16097, 66);
    set(64, 0.641303, -0.00112462, 0.0905314, 3.79524, 63);
    set(65, 0.859014, -0.00184815, 0.145728, 3.42782, 59);
    set(66, 0.900938, -0.00224979, 0.175149, 3.01474, 56);
    set(67, 0.659173, -0.00182022, 0.127062, 3.50153, 53);
    set(68, 0.699468, -0.00238831, 0.153068, 3.83169, 50);
    set(69, 0.880658, -0.00153782, 0.102579, 3.10522, 47);
    set(70, 0.736965, -0.00187258, 0.110232, 3.50977, 44);
    set(71, 0.629882, -0.00239866, 0.12889, 3.29865, 42);
    set(72, 0.594003, -0.0036758, 0.182466, 3.98778, 39);
    set(73, 0.492299, -0.00157186, 0.0738131, 4.24693, 37);
    set(74, 0.69678, -0.00298618, 0.137993, 4.05956, 35);
    set(75, 0.489816, -0.00167135, 0.0706261, 3.84835, 33);
    set(76, 0.0485605, -0.000801011, 0.0275237, 4.86936, 31);
    set(77, 0.155722, -0.00184295, 0.041575, 5.20337, 29);
    set(78, 0.820445, -0.00752278, 0.285607, 3.64068, 28);
    set(79, 0.0578643, -0.000701899, 0.0224608, 4.23547, 26);
    set(80, 0.25839, -0.0016537, 0.0709622, 4.99559, 25);
    set(81, 0.765839, -0.0172386, 0.50162, 2.77722, 23);
    set(82, 0.484545, -0.00411653, 0.145137, 5.47927, 22);
    set(83, 0.933324, -0.0193255, 0.579119, 2.36642, 21);
    set(84, 0.613753, -0.0170093, 0.579309, 5.46619, 19);
    set(85, 0.403642, -0.00258473, 0.106829, 7.74933, 18);
    set(86, 0.036125, -0.00163748, 0.0481713, 8.32339, 17);
    set(87, 0.443488, -0.0111822, 0.420061, 6.71471, 16);
    set(88, 0.555745, -0.0438976, 0.887778, 5.58897, 15);
    set(89, 0.48834, -0.00504414, 0.112786, 8.10654, 14);
    set(90, 0.442373, -0.0194588, 0.346667, 8.59809, 14);
    set(91, 0.707859, -0.0315022, 0.557637, 9.44504, 13);
    set(92, 0.319009, -0.006296, 0.132196, 10.6437, 12);
    set(93, 0.784422, -0.109296, 1.6718, 3.87772, 11);
    set(94, 0.183392, -0.0164087, 0.286607, 9.36501, 11);
    set(95, 0.744149, -0.225496, 3.09048, 2.49796, 10);
    set(96, 0.926585, -0.300102, 4.46368, -0.297974, 9);
    set(97, 0.354715, -0.193954, 2.00892, 5.59655, 9);
    set(98, 0.871293, -0.10377, 1.19316, 4.26826, 8);
    set(99, 0.560351, -0.231044, 2.17584, 3.71048, 8);
    set(100, 0.663161, -0.213757, 2.03066, 5.79332, 7);
    set(101, 0.435292, -0.302343, 2.32382, 3.23109, 7);
    set(102, 0.166553, -0.161761, 1.13345, 3.9676, 7);
    set(103, 0.525047, -0.198177, 1.65256, 6.58921, 6);
    set(104, 0.353991, -0.0103532, 0.245277, 9.63382, 6);
    set(105, 0.905034, -0.621997, 4.68464, 7.95296, 5);
    set(106, 0.911502, 0.00620229, -0.0518092, 9.02553, 5);

    return data;
}
