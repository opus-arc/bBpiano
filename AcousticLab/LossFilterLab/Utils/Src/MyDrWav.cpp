//
// Created by opus arc on 2026/3/20.
//

#define DR_WAV_IMPLEMENTATION
#include "../../../../External/dr_wav/dr_wav.h"


#include "../MyDrWav.h"
#include <cstdint>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>


std::vector<float> MyDrWav::loadWav(
    const std::string& wavPath,
    uint32_t targetSampleRate,
    uint32_t targetChannels
) {
    try {
        unsigned int channels = 0;
        unsigned int sampleRate = 0;
        drwav_uint64 totalFrameCount = 0;

        float* samples = drwav_open_file_and_read_pcm_frames_f32(
            wavPath.c_str(),
            &channels,
            &sampleRate,
            &totalFrameCount,
            nullptr
        );

        if (!samples) {
            throw std::runtime_error("Failed to open wav");
        }

        if (targetSampleRate != 0 && sampleRate != targetSampleRate) {
            std::cerr << "Warning: wav sampleRate is " << sampleRate
                      << ", expected " << targetSampleRate << "\n";
        }

        if (targetChannels != 0 && channels != targetChannels) {
            std::cerr << "Warning: wav channels is " << channels
                      << ", expected " << targetChannels << "\n";
        }

        std::vector<float> pcm(
            samples,
            samples + totalFrameCount * channels
        );

        drwav_free(samples, nullptr);

        std::cout << "wavPath: " << wavPath << "\n";
        std::cout << "channels: " << channels << "\n";
        std::cout << "sampleRate: " << sampleRate << "\n";
        std::cout << "frames: " << totalFrameCount << "\n";

        return pcm;
    } catch (const std::exception& e) {
        std::cerr << e.what() << "\n";
        return {};
    }
}
