//
// Created by opus arc on 2026/3/20.
//

#ifndef MOTIF_MYFFT_H
#define MOTIF_MYFFT_H
#include <vector>
#include <atomic>


class MyFFT {
public:
    static std::vector<std::vector<float>> computeSpectrogram(
        const std::vector<float> &audio,
        unsigned int sampleRate,
        size_t fftSize,
        size_t hopSize);
    
    static double binToFreq(size_t bin, unsigned int sampleRate, size_t fftSize) {
        return static_cast<double>(bin) * static_cast<double>(sampleRate)
               / static_cast<double>(fftSize);
    }

    static size_t freqToBin(double freq, unsigned int sampleRate, size_t fftSize) {
        return static_cast<size_t>(
            std::llround(freq * static_cast<double>(fftSize)
                         / static_cast<double>(sampleRate))
        );
    }
    
};

#endif //MOTIF_MYFFT_H
