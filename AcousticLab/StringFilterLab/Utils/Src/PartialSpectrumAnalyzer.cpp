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
#include <iostream>
#include <system_error>

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

struct LossFilterFitResult {
    double g = 0.0;
    double a1 = 0.0;
    int pointCount = 0;
    double fitError = 0.0;
};

LossFilterFitResult fitOnePoleLossFilterFromSigmaPoints(
    int midi,
    const std::vector<std::array<double, 2>>& sigmaPoints,
    double sampleRate
);

void testABC();



void PartialSpectrumAnalyzer::analyzer() {
//    testABC();
//    return ;
    
    
    using std::cout;
    using std::cerr;
    std::error_code ec;
    if(!fs::exists(generatedDataRoot, ec)) {
        cerr << "[PartialSpectrumAnalyzer][missing] " << generatedDataRoot << ", ec = " << ec.message() << "\n";
        return ;
    }
    ec.clear();
    if(!fs::exists(splitFolder, ec)) {
        cerr << "[PartialSpectrumAnalyzer][missing] " << splitFolder << ", ec = " << ec.message() << "\n";
        return ;
    }
    ec.clear();
    fs::create_directories(spectrogramFolderPath, ec);
    if(ec) {
        cerr << "[PartialSpectrumAnalyzer][missing] " << spectrogramFolderPath << ", ec = " << ec.message() << "\n";
        return ;
    }
    
    double sampleRate = 44100.0;
    int channel = 2;
    const int fftSize = 32768;
    const int hopSize = 512;
    const double framesPerSecond = sampleRate / hopSize;
    const double binHz = sampleRate / fftSize;
    
    std::vector<std::vector<Partial_Scan>> all_partialBeforeMerge;
    
    AllPianoSpectrogram all_piano_spectrogram;
    
    std::vector<float> binFrequencies;
    
    std::error_code splitIterEc;
    fs::directory_iterator splitIterator(splitFolder, splitIterEc);
    if(splitIterEc) {
        cerr << "[PartialSpectrumAnalyzer][missing] " << splitFolder << ", ec = " << splitIterEc.message() << "\n";
        return ;
    }
    
    for (const auto& split_entry : splitIterator) {
        std::error_code splitEntryEc;
        if(!split_entry.is_directory(splitEntryEc)) {
            if(splitEntryEc) {
                cerr << "[PartialSpectrumAnalyzer][missing] " << split_entry.path() << ", ec = " << splitEntryEc.message() << "\n";
            }
            continue;
        }
        
        const std::string velocity = MyFile::findVelocity(split_entry.path().filename());
        std::vector<Partial_Scan> velocity_partialBeforeMerge;
        
        PianoSpectrogram ps;
        ps.velocity = velocity;
        
        if((velocity != test_velocity) && isTestSpecificVelocity) continue;
        
        std::error_code wavIterEc;
        fs::directory_iterator wavIterator(split_entry.path(), wavIterEc);
        if(wavIterEc) {
            cerr << "[PartialSpectrumAnalyzer][missing] " << split_entry.path() << ", ec = " << wavIterEc.message() << "\n";
            continue;
        }
        
        for (const auto& entry : wavIterator) {
            
            const fs::path path = entry.path();
            std::error_code entryEc;
            if (!entry.is_regular_file(entryEc) || path.extension() != ".wav") {
                if(entryEc) {
                    cerr << "[PartialSpectrumAnalyzer][missing] " << path << ", ec = " << entryEc.message() << "\n";
                }
                continue;
            }
            
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
        
//        QuadraticRegressionResult qlrr = LinearRegression::fit2(SigmaPoints);
//        cout << "r2: " << qlrr.r2 << ", a: " << qlrr.a << ", b: " << qlrr.b << ", c: " << qlrr.c << ", n: " << qlrr.n<< "\n";
        
        LossFilterFitResult fit =
            fitOnePoleLossFilterFromSigmaPoints(midi, SigmaPoints, sampleRate);
        cout
            << "fit: "
            << midi << ","
            << MyPitch::midiToFrequency(midi) << ","
            << fit.g << ","
            << fit.a1 << ","
            << fit.pointCount << ","
            << fit.fitError
            << "\n";
    }
    all_piano_spectrogram.aps.clear();
    all_piano_spectrogram.aps.shrink_to_fit();
    

}

//void testABC() {
//    writeLossFilterConstantsCSV();
//    writeDispersionFilterConstantsCSV();
//    auto rawLossFit = makeRawLossFitByMidi();
//    smoothLossABCByMidi(rawLossFit);
//}




// MARK: AI Mixed
LossFilterFitResult fitOnePoleLossFilterFromSigmaPoints(
    int midi,
    const std::vector<std::array<double, 2>>& sigmaPoints,
    double sampleRate
) {
    LossFilterFitResult best;
    best.fitError = std::numeric_limits<double>::infinity();

    const double f0 = MyPitch::midiToFrequency(midi);
    if (f0 <= 0.0 || sampleRate <= 0.0 || sigmaPoints.size() < 2) {
        return best;
    }

    auto predictedSigma = [&](double g, double a1, double partial) {
        const double f = f0 * partial;
        const double omega = 2.0 * M_PI * f / sampleRate;

        const double numerator = g * (1.0 + a1);
        const double realDen = 1.0 + a1 * std::cos(omega);
        const double imagDen = -a1 * std::sin(omega);

        const double denMag = std::sqrt(realDen * realDen + imagDen * imagDen);
        if (denMag <= 0.0) return 0.0;

        double mag = std::abs(numerator) / denMag;
        mag = std::clamp(mag, 1e-8, 0.999999);

        return -f0 * std::log(mag);
    };

    for (double g = 0.970; g <= 0.99995; g += 0.00005) {
        for (double a1 = -0.95; a1 <= -0.001; a1 += 0.0005) {
            double error = 0.0;
            int count = 0;

            for (const auto& point : sigmaPoints) {
                const double partial = point[0];
                const double measuredSigma = point[1];

                if (partial <= 0.0 || measuredSigma <= 0.0 || !std::isfinite(measuredSigma)) {
                    continue;
                }

                const double pred = predictedSigma(g, a1, partial);
                const double e = pred - measuredSigma;

                error += e * e;
                count++;
            }

            if (count < 2) continue;

            error /= static_cast<double>(count);

            if (error < best.fitError) {
                best.g = g;
                best.a1 = a1;
                best.pointCount = count;
                best.fitError = error;
            }
        }
    }

    return best;
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




