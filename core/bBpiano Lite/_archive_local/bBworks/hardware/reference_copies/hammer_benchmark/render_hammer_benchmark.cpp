
#include <algorithm>
#include <array>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <streambuf>
#include <string>
#include <vector>

namespace {

constexpr int kSampleRate = 44'100;

class NullBuffer final : public std::streambuf {
protected:
    int overflow(int character) override {
        return traits_type::not_eof(character);
    }
};

class ScopedCoutSilencer final {
public:
    ScopedCoutSilencer()
        : previous_(std::cout.rdbuf(&buffer_)) {}

    ~ScopedCoutSilencer() {
        std::cout.rdbuf(previous_);
    }

private:
    NullBuffer buffer_;
    std::streambuf* previous_;
};

void writeUInt16LE(std::ofstream& file, std::uint16_t value) {
    file.put(static_cast<char>(value & 0xFF));
    file.put(static_cast<char>((value >> 8) & 0xFF));
}

void writeUInt32LE(std::ofstream& file, std::uint32_t value) {
    file.put(static_cast<char>(value & 0xFF));
    file.put(static_cast<char>((value >> 8) & 0xFF));
    file.put(static_cast<char>((value >> 16) & 0xFF));
    file.put(static_cast<char>((value >> 24) & 0xFF));
}

void writeWav(
    const std::filesystem::path& path,
    const std::vector<float>& samples
) {
    std::filesystem::create_directories(path.parent_path());
    std::ofstream file(path, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Cannot create WAV: " + path.string());
    }

    constexpr std::uint16_t audioFormat = 3;
    constexpr std::uint16_t channelCount = 1;
    constexpr std::uint16_t bitsPerSample = 32;
    constexpr std::uint16_t blockAlign =
        channelCount * bitsPerSample / 8;
    constexpr std::uint32_t byteRate =
        kSampleRate * blockAlign;
    constexpr std::uint32_t fmtChunkSize = 16;

    const auto dataChunkSize = static_cast<std::uint32_t>(
        samples.size() * sizeof(float)
    );
    const std::uint32_t riffChunkSize =
        4 + (8 + fmtChunkSize) + (8 + dataChunkSize);

    file.write("RIFF", 4);
    writeUInt32LE(file, riffChunkSize);
    file.write("WAVE", 4);
    file.write("fmt ", 4);
    writeUInt32LE(file, fmtChunkSize);
    writeUInt16LE(file, audioFormat);
    writeUInt16LE(file, channelCount);
    writeUInt32LE(file, kSampleRate);
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

std::unique_ptr<PianoModel> makePiano() {
    ScopedCoutSilencer silence;
    auto piano = std::make_unique<PianoModel>();
    piano->test_sustainPedal_active = true;
    return piano;
}

std::vector<float> renderSingleNote(
    int midi,
    int velocity,
    double seconds
) {
    auto piano = makePiano();
    {
        ScopedCoutSilencer silence;
        piano->note_on(midi, velocity);
    }

    const std::size_t frameCount = static_cast<std::size_t>(
        seconds * static_cast<double>(kSampleRate) + 0.5
    );
    std::vector<float> samples(frameCount, 0.0f);
    for (std::size_t frame = 0; frame < frameCount; ++frame) {
        piano->pianoMovement();
        samples[frame] = piano->getSample();
    }
    return samples;
}

void dispatchMidiEvent(
    PianoModel& piano,
    const MidiService::MidiEvent& event
) {
    const int note = static_cast<int>(event.data1);
    if (event.isNoteOn()) {
        piano.note_on(note, static_cast<double>(event.data2));
    } else if (event.isNoteOff()) {
        piano.note_off(note, static_cast<double>(event.data2));
    } else if (event.isPedalControlChange()) {
        const double depth = event.normalizedData2();
        switch (event.data1) {
            case 64: piano.sustainPedal_control(depth); break;
            case 66: piano.sostenutoPedal_control(depth); break;
            case 67: piano.softPedal_control(depth); break;
            case 68: piano.harmonicPedal_control(depth); break;
            default: break;
        }
    }
}

std::vector<float> renderMidi(
    const std::filesystem::path& midiPath,
    double startSeconds,
    double seconds
) {
    auto events = MidiService::MidiEvent::loadEvents(midiPath.string());
    std::sort(
        events.begin(),
        events.end(),
        [](const auto& lhs, const auto& rhs) {
            return lhs.timeInSeconds < rhs.timeInSeconds;
        }
    );

    auto piano = makePiano();
    const double safeStart = std::max(0.0, startSeconds);
    const std::size_t startFrame = static_cast<std::size_t>(
        safeStart * static_cast<double>(kSampleRate) + 0.5
    );
    const std::size_t endFrame = static_cast<std::size_t>(
        (safeStart + seconds) * static_cast<double>(kSampleRate) + 0.5
    );
    std::vector<float> samples;
    samples.reserve(endFrame - startFrame);
    std::size_t eventIndex = 0;

    ScopedCoutSilencer silence;
    for (std::size_t frame = 0; frame < endFrame; ++frame) {
        const double time =
            static_cast<double>(frame) / static_cast<double>(kSampleRate);
        while (
            eventIndex < events.size()
            && events[eventIndex].timeInSeconds <= time
        ) {
            dispatchMidiEvent(*piano, events[eventIndex]);
            ++eventIndex;
        }
        piano->pianoMovement();
        const float sample = piano->getSample();
        if (frame >= startFrame) {
            samples.push_back(sample);
        }
    }
    return samples;
}

double parseDouble(const char* text, const char* name) {
    try {
        return std::stod(text);
    } catch (...) {
        throw std::runtime_error(std::string("Invalid ") + name);
    }
}

int parseInt(const char* text, const char* name) {
    try {
        return std::stoi(text);
    } catch (...) {
        throw std::runtime_error(std::string("Invalid ") + name);
    }
}

} // namespace

int main(int argc, char** argv) {
    try {
        if (argc >= 3 && std::string(argv[1]) == "--single-notes") {
            const std::filesystem::path outputDirectory = argv[2];
            const int velocity =
                argc >= 4 ? parseInt(argv[3], "velocity") : 80;
            const double seconds =
                argc >= 5 ? parseDouble(argv[4], "duration") : 4.0;
            constexpr std::array<int, 7> midiNotes{
                33, 45, 57, 69, 81, 93, 105
            };

            for (std::size_t i = 0; i < midiNotes.size(); ++i) {
                const std::string name =
                    "A" + std::to_string(i + 1) + "_2s.wav";
                writeWav(
                    outputDirectory / name,
                    renderSingleNote(midiNotes[i], velocity, seconds)
                );
            }
            std::cout
                << "Rendered seven single-note attacks to "
                << outputDirectory << '\n';
            return 0;
        }

        if (argc >= 5 && std::string(argv[1]) == "--midi") {
            const std::filesystem::path midiPath = argv[2];
            const std::filesystem::path outputPath = argv[3];
            const double startSeconds =
                argc >= 6 ? parseDouble(argv[4], "start time") : 0.0;
            const double seconds = parseDouble(
                argc >= 6 ? argv[5] : argv[4],
                "duration"
            );
            writeWav(
                outputPath,
                renderMidi(midiPath, startSeconds, seconds)
            );
            std::cout << "Rendered MIDI excerpt to " << outputPath << '\n';
            return 0;
        }

        std::cerr
            << "Usage:\n"
            << "  render_hammer_benchmark --single-notes OUTPUT_DIR [velocity] [seconds]\n"
            << "  render_hammer_benchmark --midi INPUT.mid OUTPUT.wav [start] seconds\n";
        return 2;
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
