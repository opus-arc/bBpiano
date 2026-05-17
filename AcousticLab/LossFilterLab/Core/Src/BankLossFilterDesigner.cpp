//
//  BankLossFilterDesigner.cpp
//  bBpiano
//
//  Created by opus arc on 2026/5/16.
//


#include "BankLossFilterDesigner.hpp"
#include "../../Utils/MyFFT.h"
#include "../../Utils/MyDrWav.h"
#include "../../Utils/MyPitch.hpp"

#include <fstream>
#include <atomic>
#include <iostream>
#include <filesystem>
#include <string>
#include <vector>
#include <stdexcept>
#include <algorithm>
#include <cmath>
#include <map>

namespace fs = std::filesystem;

struct AmplitudeEnvelope {
    int partialIndex = 0;
    double f = 440.0;
    std::vector<float> A;
};

struct DecayResult {
    int partialIndex = 0;
    double f = 0.0;
    double sigma = 0.0;
    double intercept = 0.0;
    double r2 = 0.0;
};

struct LossPoint {
    double frequency;
    double sigma;
};

static double estimateInharmonicityCoefficient(double f0) {
    if (f0 < 110.0)
        return 0.00010;
    if (f0 < 220.0)
        return 0.00025;
    if (f0 < 440.0)
        return 0.00060;
    if (f0 < 880.0)
        return 0.00120;
    if (f0 < 1760.0)
        return 0.00250;
    return 0.00450;
}

DecayResult fitDecaySigma(
    const AmplitudeEnvelope& ae,
    double sampleRate,
    int hopSize,
    int skipFrames = 10,
    float minAmp = 1e-7f,
    float stopRatio = 0.001f
);

void lossFilterDesinger(){
    
    std::ofstream decay_csv(
        "/Users/opusarc/XCodeProjects/bBpiano/AcousticLab/LossFilterLab/decay_results.csv"
    );
    
    decay_csv << "velocity,pitch,f0,partial,partialFreq,sigma,intercept,r2\n";
    
    const std::string lossOutputFolder =
        "/Users/opusarc/XCodeProjects/bBpiano/AcousticLab/LossFilterLab/LossResults";
    fs::create_directories(lossOutputFolder);
    fs::create_directories(fs::path(lossOutputFolder) / "Merged");
    
    
    const double sampleRate = 44100.0;
    const int channel = 2;
    const int fftSize = 32768;
    const int hopSize = 512;
    const double binHz = sampleRate / static_cast<double>(fftSize); // ≈ 1.3458 Hz
    const int maxUsefulPartial = 96;
    const double maxAnalysisFreq = 16000.0;
    const float relativePeakThreshold = 0.001f;
    const float highFrequencyRelativePeakThreshold = 0.00008f;
    const double highRegisterStartFreq = 880.0;
    
    std::string splitFolder =
        "/Users/opusarc/XCodeProjects/bBpiano/AcousticLab/LossFilterLab/Samples/Pianoteq 9/SingleNoteSamples/Split";
    
    std::map<std::string, std::map<int, std::pair<double, int>>> mergedSigmaByPitchAndPartial;
    std::map<std::string, std::map<int, std::pair<double, int>>> mergedFreqByPitchAndPartial;

    for(int i = 50; i <= 110; i += 15) {
        const fs::path velocityOutputFolder = fs::path(lossOutputFolder) / ("v" + std::to_string(i));
        fs::create_directories(velocityOutputFolder);

        for (const auto& entry : fs::directory_iterator(splitFolder + "/v" + std::to_string(i))){

            const fs::path path = entry.path();
            const std::string filename = path.filename();
            const std::string pitchName = MyPitch::findName(filename);
            const double f0 = MyPitch::getFrequency(pitchName);
            
            if (!entry.is_regular_file() || path.extension() != ".wav")
                continue;
                
            // TODO: 这里临时关掉了沙盒 为了不用复制这些采样音频进入 resource，以后还是要打开的吧
            const std::vector<float> pcm_stereo = MyDrWav::loadWav(path,
                                                            sampleRate,
                                                            channel);
                
            // pcm 双声道变 单声道 好做 SFTF
            const std::vector<float> pcm_mono = MyDrWav::downmixStereoToMono(pcm_stereo);
            
            std::vector<std::vector<float>> spectrogram_hasMeta = MyFFT::computeSpectrogram(pcm_mono, sampleRate, fftSize,hopSize);
            
            const std::vector<float> f_meta = spectrogram_hasMeta.back();
            
            spectrogram_hasMeta.pop_back();
            // spectrogram[frame][f_bin]
            const std::vector<std::vector<float>> spectrogram = spectrogram_hasMeta;
            
            std::vector<AmplitudeEnvelope> partials;

            const int baseSearchRadiusBins = 8;
            const int referenceFrame = 5;

            if (spectrogram.empty() || referenceFrame >= static_cast<int>(spectrogram.size()))
                continue;

            float referenceMaxAmp = 0.0f;
            for (float amp : spectrogram[referenceFrame]) {
                referenceMaxAmp = std::max(referenceMaxAmp, amp);
            }

            if (referenceMaxAmp <= 0.0f)
                continue;

            const int maxPartial = std::min(
                maxUsefulPartial,
                static_cast<int>(maxAnalysisFreq / f0)
            );
            const bool isHighRegister = f0 >= highRegisterStartFreq;
            const double inharmonicityB = estimateInharmonicityCoefficient(f0);

            for (int p = 1; p <= maxPartial; ++p) {
                const double harmonicTargetFreq = f0 * p;
                const double targetFreq = harmonicTargetFreq * std::sqrt(1.0 + inharmonicityB * p * p);
                const int centerBin = static_cast<int>(std::round(targetFreq / binHz));

                const int searchRadiusBins = isHighRegister
                    ? baseSearchRadiusBins + static_cast<int>(std::round(targetFreq / 600.0))
                    : baseSearchRadiusBins + static_cast<int>(std::round(targetFreq / 1500.0));
                int startBin = std::max(1, centerBin - searchRadiusBins);
                int endBin = std::min(
                    static_cast<int>(f_meta.size()) - 2,
                    centerBin + searchRadiusBins
                );

                int peakBin = centerBin;
                float peakAmp = 0.0f;
                size_t peakSearchFrame = referenceFrame;

                const size_t lastSearchFrame = std::min(
                    spectrogram.size() - 1,
                    static_cast<size_t>(referenceFrame + (isHighRegister ? 45 : 20))
                );

                for (size_t frame = referenceFrame; frame <= lastSearchFrame; ++frame) {
                    for (int bin = startBin; bin <= endBin; ++bin) {
                        const float amp = spectrogram[frame][bin];

                        if (amp > peakAmp) {
                            peakAmp = amp;
                            peakBin = bin;
                            peakSearchFrame = frame;
                        }
                    }
                }

                const float partialThreshold = (targetFreq >= 3000.0 || isHighRegister)
                    ? referenceMaxAmp * highFrequencyRelativePeakThreshold
                    : referenceMaxAmp * relativePeakThreshold;

                if (peakAmp < partialThreshold)
                    continue;

                AmplitudeEnvelope ae;
                ae.partialIndex = p;
                ae.f = f_meta[peakBin];
                ae.A.reserve(spectrogram.size());

                for (size_t frame = 0; frame < spectrogram.size(); ++frame) {
                    int localPeakBin = peakBin;
                    float localPeakAmp = 0.0f;

                    const int localTrackRadiusBins = isHighRegister ? 5 : 2;
                    const int localStartBin = std::max(1, peakBin - localTrackRadiusBins);
                    const int localEndBin = std::min(
                        static_cast<int>(f_meta.size()) - 2,
                        peakBin + localTrackRadiusBins
                    );

                    for (int bin = localStartBin; bin <= localEndBin; ++bin) {
                        const float amp = spectrogram[frame][bin];
                        if (amp > localPeakAmp) {
                            localPeakAmp = amp;
                            localPeakBin = bin;
                        }
                    }

                    ae.A.push_back(spectrogram[frame][localPeakBin]);
                }

                partials.push_back(std::move(ae));
            }
            
            std::vector<DecayResult> decayResults;
            std::ofstream key_loss_csv(velocityOutputFolder / (pitchName + ".csv"));
            key_loss_csv << "partial,f,sigma,r2\n";
            
            for (const auto& ae : partials) {
                const bool isHighFrequencyPartial = ae.f >= 3000.0 || f0 >= highRegisterStartFreq;
                const int adaptiveSkipFrames = isHighFrequencyPartial ? 2 : 10;
                const float adaptiveStopRatio = isHighFrequencyPartial ? 0.00003f : 0.001f;
                const double minR2 = isHighFrequencyPartial ? 0.30 : 0.50;

                DecayResult dr = fitDecaySigma(
                    ae,
                    sampleRate,
                    hopSize,
                    adaptiveSkipFrames,
                    1e-7f,
                    adaptiveStopRatio
                );

                if (dr.sigma > 0.0 && dr.r2 >= minR2) {
                    decayResults.push_back(dr);

                    decay_csv
                        << i << ","
                        << pitchName << ","
                        << f0 << ","
                        << dr.partialIndex << ","
                        << dr.f << ","
                        << dr.sigma << ","
                        << dr.intercept << ","
                        << dr.r2
                        << "\n";

                    key_loss_csv
                        << dr.partialIndex << ","
                        << dr.f << ","
                        << dr.sigma << ","
                        << dr.r2
                        << "\n";

                    mergedSigmaByPitchAndPartial[pitchName][dr.partialIndex].first += dr.sigma;
                    mergedSigmaByPitchAndPartial[pitchName][dr.partialIndex].second += 1;
                    mergedFreqByPitchAndPartial[pitchName][dr.partialIndex].first += dr.f;
                    mergedFreqByPitchAndPartial[pitchName][dr.partialIndex].second += 1;
                }
            }
        
            key_loss_csv.close();
            // end directory loop for velocity
        }
    }
        
    const fs::path mergedOutputFolder = fs::path(lossOutputFolder) / "Merged";

    for (const auto& pitchItem : mergedSigmaByPitchAndPartial) {
        const std::string& pitchName = pitchItem.first;
        const auto& sigmaByPartial = pitchItem.second;

        std::ofstream merged_key_csv(mergedOutputFolder / (pitchName + ".csv"));
        merged_key_csv << "partial,f,sigma,count\n";

        for (const auto& partialItem : sigmaByPartial) {
            const int partialIndex = partialItem.first;
            const double sigmaSum = partialItem.second.first;
            const int count = partialItem.second.second;

            if (count <= 0)
                continue;

            const auto freqPitchIt = mergedFreqByPitchAndPartial.find(pitchName);
            if (freqPitchIt == mergedFreqByPitchAndPartial.end())
                continue;

            const auto freqPartialIt = freqPitchIt->second.find(partialIndex);
            if (freqPartialIt == freqPitchIt->second.end() || freqPartialIt->second.second <= 0)
                continue;

            const double meanFrequency = freqPartialIt->second.first / static_cast<double>(freqPartialIt->second.second);
            const double meanSigma = sigmaSum / static_cast<double>(count);

            merged_key_csv << partialIndex << "," << meanFrequency << "," << meanSigma << "," << count << "\n";
        }

        merged_key_csv.close();
    }
    
    decay_csv.close();
}


DecayResult fitDecaySigma(
    const AmplitudeEnvelope& ae,
    double sampleRate,
    int hopSize,
    int skipFrames,
    float minAmp,
    float stopRatio
) {
    double sumT = 0.0;
    double sumY = 0.0;
    double sumTT = 0.0;
    double sumTY = 0.0;
    double sumYY = 0.0;
    int n = 0;

    if (ae.A.empty()) {
        DecayResult result;
        result.partialIndex = ae.partialIndex;
        result.f = ae.f;
        return result;
    }

    float peakAmp = 0.0f;
    size_t peakFrame = 0;
    for (size_t frame = 0; frame < ae.A.size(); ++frame) {
        if (ae.A[frame] > peakAmp) {
            peakAmp = ae.A[frame];
            peakFrame = frame;
        }
    }

    if (peakAmp <= minAmp) {
        DecayResult result;
        result.partialIndex = ae.partialIndex;
        result.f = ae.f;
        return result;
    }

    const float stopAmp = std::max(minAmp, peakAmp * stopRatio);
    const size_t startFrame = std::min(
        peakFrame + static_cast<size_t>(skipFrames),
        ae.A.size() - 1
    );

    for (size_t frame = startFrame; frame < ae.A.size(); ++frame) {
        const float amp = ae.A[frame];

        if (amp <= minAmp)
            continue;

        if (amp < stopAmp)
            break;

        const double t = static_cast<double>(frame * hopSize) / sampleRate;
        const double y = std::log(static_cast<double>(amp));

        sumT += t;
        sumY += y;
        sumTT += t * t;
        sumTY += t * y;
        sumYY += y * y;
        ++n;
    }

    DecayResult result;
    result.partialIndex = ae.partialIndex;
    result.f = ae.f;

    if (n < 2) {
        result.sigma = 0.0;
        result.intercept = 0.0;
        return result;
    }

    const double denom = n * sumTT - sumT * sumT;

    if (std::abs(denom) < 1e-12) {
        result.sigma = 0.0;
        result.intercept = 0.0;
        return result;
    }

    const double numerator = n * sumTY - sumT * sumY;
    const double slope = numerator / denom;
    const double intercept = (sumY - slope * sumT) / n;

    result.sigma = -slope;
    result.intercept = intercept;

    const double denomY = n * sumYY - sumY * sumY;
    if (denomY > 1e-12) {
        result.r2 = (numerator * numerator) / (denom * denomY);
        result.r2 = std::clamp(result.r2, 0.0, 1.0);
    }

    return result;
}
