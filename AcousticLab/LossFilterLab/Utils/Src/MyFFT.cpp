//
// Created by opus arc on 2026/3/20.
//

#include "../MyFFT.h"
#include "../../../../External/fft/kiss_fftr.h"
#include <vector>
#include <cmath>
#include <cstdlib>

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

std::vector<std::vector<float>> MyFFT::computeSpectrogram(const std::vector<float> &audio,
                                                          const unsigned int sampleRate,
                                                          const size_t fftSize,
                                                          const size_t hopSize) {

    if (fftSize == 0 || hopSize == 0 || sampleRate == 0 || audio.empty()) {
        return {};
    }

    const std::vector<float> hannWindow = makeHannWindowInternal(fftSize);
    const size_t spectrumSize = fftSize / 2 + 1;
    const size_t estimatedFrames = (audio.size() + hopSize - 1) / hopSize;

    kiss_fftr_cfg cfg = kiss_fftr_alloc(static_cast<int>(fftSize), 0, nullptr, nullptr);
    if (cfg == nullptr) {
        return {};
    }

    std::vector<std::vector<float>> spectrogram;
    spectrogram.reserve(estimatedFrames);

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

        spectrogram.push_back(magnitude);

        if (frameStart + hopSize >= audio.size()) {
            break;
        }
    }

    free(cfg);
    return spectrogram;
}
