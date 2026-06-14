//
//  midiExporter.hpp
//  bBpiano
//
//  Created by opus arc on 2026/6/8.
//

#ifndef midiExporter_hpp
#define midiExporter_hpp

#include "midiService.hpp"
#include "../core/controller.hpp"

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include <chrono>

class MidiExporter {
public:
    static void exportWav(
        const std::string& midiFilePath,
        const std::string& outputWavPath = ""
    ) {
        if (midiFilePath.empty()) {
            throw std::runtime_error("No MIDI file path provided.");
        }
        const auto exportStart = std::chrono::steady_clock::now();

        constexpr int sampleRate = 44'100;
        constexpr int bufferSize = 512;
        constexpr double amplitudeLimiter = 1.0;
        constexpr double tailSeconds = 5.0;

        std::vector<MidiService::MidiEvent> midiEvents = MidiService::MidiEvent::loadEvents(midiFilePath);
        std::sort(
            midiEvents.begin(),
            midiEvents.end(),
            [](const MidiService::MidiEvent& lhs, const MidiService::MidiEvent& rhs) {
                return lhs.timeInSeconds < rhs.timeInSeconds;
            }
        );

        const std::string outPath = outputWavPath.empty()
            ? buildDefaultOutputPath(midiFilePath)
            : outputWavPath;

        all_silence();

        const double songDuration = midiEvents.empty()
            ? 0.0
            : midiEvents.back().timeInSeconds;

        const double totalDuration = songDuration + tailSeconds;
        const size_t totalFrames = static_cast<size_t>(totalDuration * static_cast<double>(sampleRate) + 0.5);

        std::vector<float> samples;
        samples.reserve(totalFrames);

        size_t frame = 0;
        size_t eventIndex = 0;

        while (frame < totalFrames) {
            const size_t framesToRender = std::min(
                static_cast<size_t>(bufferSize),
                totalFrames - frame
            );

            const double bufferEndTime = static_cast<double>(frame + framesToRender) / static_cast<double>(sampleRate);

            while (eventIndex < midiEvents.size() && midiEvents[eventIndex].timeInSeconds <= bufferEndTime) {
                dispatchMidiEvent(midiEvents[eventIndex]);
                ++eventIndex;
            }

            float buffer[bufferSize] {};
            get_next_buffer(
                buffer,
                static_cast<int>(framesToRender),
                amplitudeLimiter
            );

            samples.insert(samples.end(), buffer, buffer + framesToRender);
            frame += framesToRender;
        }

        all_silence();
        writeWavFloat32Mono(outPath, samples);

        const auto exportEnd = std::chrono::steady_clock::now();
        const double elapsedSeconds =
            std::chrono::duration<double>(exportEnd - exportStart).count();

        std::cout << "MIDI export finished in "
                  << elapsedSeconds
                  << " seconds" << std::endl;
    }

private:
    static void dispatchMidiEvent(const MidiService::MidiEvent& event) {
        const int note = static_cast<int>(event.data1);
        const double value = std::clamp(static_cast<double>(event.data2) / 127.0, 0.0, 1.0);

        if (event.isNoteOn()) {
            note_on(note, value);
        } else if (event.isNoteOff()) {
            note_off(note, value);
        } else if (event.statusType() == 0xA0) {
            note_afterTouch(note, value);
        }
    }

    static std::string buildDefaultOutputPath(const std::string& midiFilePath) {
        namespace fs = std::filesystem;

        const char* home = std::getenv("HOME");
        if (home == nullptr || std::string(home).empty()) {
            throw std::runtime_error("Cannot locate HOME directory.");
        }

        const fs::path midiPath(midiFilePath);
        const std::string fileName = midiPath.stem().string() + ".wav";
        const fs::path desktop = fs::path(home) / "Desktop";

        return (desktop / fileName).string();
    }

    static void writeUInt16LE(std::ofstream& file, uint16_t value) {
        file.put(static_cast<char>(value & 0xFF));
        file.put(static_cast<char>((value >> 8) & 0xFF));
    }

    static void writeUInt32LE(std::ofstream& file, uint32_t value) {
        file.put(static_cast<char>(value & 0xFF));
        file.put(static_cast<char>((value >> 8) & 0xFF));
        file.put(static_cast<char>((value >> 16) & 0xFF));
        file.put(static_cast<char>((value >> 24) & 0xFF));
    }

    static void writeWavFloat32Mono(
        const std::string& outputPath,
        const std::vector<float>& samples
    ) {
        constexpr uint16_t audioFormat = 3; // IEEE float
        constexpr uint16_t channelCount = 1;
        constexpr uint32_t sampleRate = 44'100;
        constexpr uint16_t bitsPerSample = 32;
        constexpr uint16_t blockAlign = channelCount * bitsPerSample / 8;
        constexpr uint32_t byteRate = sampleRate * blockAlign;
        constexpr uint32_t fmtChunkSize = 16;

        const uint32_t dataChunkSize = static_cast<uint32_t>(samples.size() * sizeof(float));
        const uint32_t riffChunkSize = 4 + (8 + fmtChunkSize) + (8 + dataChunkSize);

        std::ofstream file(outputPath, std::ios::binary);
        if (!file) {
            throw std::runtime_error("Failed to create WAV file: " + outputPath);
        }

        file.write("RIFF", 4);
        writeUInt32LE(file, riffChunkSize);
        file.write("WAVE", 4);

        file.write("fmt ", 4);
        writeUInt32LE(file, fmtChunkSize);
        writeUInt16LE(file, audioFormat);
        writeUInt16LE(file, channelCount);
        writeUInt32LE(file, sampleRate);
        writeUInt32LE(file, byteRate);
        writeUInt16LE(file, blockAlign);
        writeUInt16LE(file, bitsPerSample);

        file.write("data", 4);
        writeUInt32LE(file, dataChunkSize);
        file.write(
            reinterpret_cast<const char*>(samples.data()),
            static_cast<std::streamsize>(dataChunkSize)
        );
    }
};

#endif /* midiExporter_hpp */
