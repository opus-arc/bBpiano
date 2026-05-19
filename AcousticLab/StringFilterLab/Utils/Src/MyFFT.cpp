//
// Created by opus arc on 2026/3/20.
//

#include "../MyFFT.h"
#include "../../../../External/fft/kiss_fftr.h"
#include <vector>
#include <cmath>
#include <cstdlib>
#include <algorithm>

namespace {
    std::vector<float> makeHannWindowInternal(const size_t fftSize) {
        std::vector<float> window(fftSize, 0.0f);
        if (fftSize <= 1) {
            return window;
        }

        constexpr float twoPi = 6.28318530717958647692f;
        for (size_t n = 0; n < fftSize; ++n) {
            window[n] = 0.5f - 0.5f * std::cos(twoPi * static_cast<float>(n) /
                                               static_cast<float>(fftSize - 1));
        }
        return window;
    }

    void computeMagnitudeSpectrumInPlaceInternal(const std::vector<float>& frame,
                                                 kiss_fftr_cfg cfg,
                                                 std::vector<kiss_fft_cpx>& spectrum,
                                                 std::vector<float>& magnitude) {
        if (frame.empty() || cfg == nullptr) {
            magnitude.clear();
            return;
        }

        kiss_fftr(cfg, frame.data(), spectrum.data());

        const size_t binCount = magnitude.size();
        for (size_t k = 0; k < binCount; ++k) {
            const float re = spectrum[k].r;
            const float im = spectrum[k].i;
            magnitude[k] = std::sqrt(re * re + im * im);
        }
    }
}

STFTResult MyFFT::computeSpectrogram(const std::vector<float> &audio,
                                                          const unsigned int sampleRate,
                                                          const size_t fftSize,
                                                          const size_t hopSize) {

    if (fftSize == 0 || hopSize == 0 || sampleRate == 0 || audio.empty()) {
        return {};
    }

    const std::vector<float> hannWindow = makeHannWindowInternal(fftSize);
    const size_t spectrumSize = fftSize / 2 + 1;
    const size_t estimatedFrames = (audio.size() + hopSize - 1) / hopSize;

    std::vector<float> binFrequencies(spectrumSize, 0.0f);
    for (size_t k = 0; k < spectrumSize; ++k) {
        binFrequencies[k] = static_cast<float>(
            static_cast<double>(k) * static_cast<double>(sampleRate) /
            static_cast<double>(fftSize)
        );
    }

    kiss_fftr_cfg cfg = kiss_fftr_alloc(static_cast<int>(fftSize), 0, nullptr, nullptr);
    if (cfg == nullptr) {
        return {};
    }

    STFTResult stft_result;
    stft_result.spectrogram.reserve(estimatedFrames);

    std::vector<float> frame(fftSize, 0.0f);
    std::vector<kiss_fft_cpx> spectrum(spectrumSize);
    std::vector<float> magnitude(spectrumSize, 0.0f);

    for (size_t frameStart = 0; frameStart < audio.size(); frameStart += hopSize) {
        for (size_t i = 0; i < fftSize; ++i) {
            const size_t index = frameStart + i;
            const float sample = index < audio.size() ? audio[index] : 0.0f;
            frame[i] = sample * hannWindow[i];
        }

        computeMagnitudeSpectrumInPlaceInternal(frame, cfg, spectrum, magnitude);
        if (magnitude.empty()) {
            continue;
        }

        stft_result.spectrogram.push_back(magnitude);

        if (frameStart + hopSize >= audio.size()) {
            break;
        }
    }

    stft_result.binFrequencies = std::move(binFrequencies);

    free(cfg);
    
    return stft_result;
}

float MyFFT::getPartialAmplitude(
    const std::vector<std::vector<float>>& spectrogram,
    size_t frameIndex,
    double targetFreq,
    double sampleRate,
    int fftSize,
    int searchRadiusBins
)
{
    if (spectrogram.empty() || frameIndex >= spectrogram.size()) {
        return 0.0f;
    }

    const std::vector<float>& frameMagnitude = spectrogram[frameIndex];
    if (frameMagnitude.empty()) {
        return 0.0f;
    }

    double binHz = sampleRate / fftSize;
    int centerBin = static_cast<int>(std::round(targetFreq / binHz));

    int startBin = std::max(0, centerBin - searchRadiusBins);
    int endBin = std::min(
        static_cast<int>(frameMagnitude.size()) - 1,
        centerBin + searchRadiusBins
    );

    float maxAmp = 0.0f;

    for (int k = startBin; k <= endBin; ++k)
    {
        maxAmp = std::max(maxAmp, frameMagnitude[k]);
    }

    return maxAmp;
}
