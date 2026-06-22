#include "../core/piano/PianoModel.hpp"

#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <memory>
#include <streambuf>
#include <vector>
#include <sys/sysctl.h>
#include <string>

namespace {

constexpr int kSampleRate = 44'100;
constexpr int kKeyCount = 88;
constexpr int kWarmupFrames = 512;
constexpr int kMeasureFrames = 8'192;
constexpr double kMidiVelocity = 90.0;

class NullBuffer final : public std::streambuf {
protected:
    int overflow(int character) override {
        return traits_type::not_eof(character);
    }
};

class ScopedCoutSilencer final {
public:
    ScopedCoutSilencer()
        : previousBuffer_(std::cout.rdbuf(&nullBuffer_)) {}

    ~ScopedCoutSilencer() {
        std::cout.rdbuf(previousBuffer_);
    }

private:
    NullBuffer nullBuffer_;
    std::streambuf* previousBuffer_;
};

std::vector<int> distributedKeyIndices(int voiceCount) {
    std::vector<int> indices;
    indices.reserve(static_cast<std::size_t>(voiceCount));

    for (int voice = 0; voice < voiceCount; ++voice) {
        // 37 and 88 are coprime, so this visits every key exactly once while
        // keeping each growing prefix distributed across the keyboard.
        indices.push_back((kKeyCount / 2 + voice * 37) % kKeyCount);
    }
    return indices;
}

double measureOccupancyPercent(int voiceCount, int repetitions) {
    std::vector<double> results;
    results.reserve(static_cast<std::size_t>(repetitions));

    for (int repetition = 0; repetition < repetitions; ++repetition) {
        std::unique_ptr<PianoModel> piano;

        {
            ScopedCoutSilencer silenceSetupLogs;
            piano = std::make_unique<PianoModel>();

            for (int keyIndex : distributedKeyIndices(voiceCount)) {
                piano->note_on(21 + keyIndex, kMidiVelocity);
            }
        }

        volatile float sampleSink = 0.0f;

        for (int frame = 0; frame < kWarmupFrames; ++frame) {
            piano->pianoMovement();
            sampleSink = sampleSink + piano->getSample();
        }

        const auto start = std::chrono::steady_clock::now();

        for (int frame = 0; frame < kMeasureFrames; ++frame) {
            piano->pianoMovement();
            sampleSink = sampleSink + piano->getSample();
        }

        const auto end = std::chrono::steady_clock::now();
        const double elapsedSeconds =
            std::chrono::duration<double>(end - start).count();
        const double renderedSeconds =
            static_cast<double>(kMeasureFrames) / kSampleRate;

        results.push_back(100.0 * elapsedSeconds / renderedSeconds);
    }

    std::sort(results.begin(), results.end());
    return results[results.size() / 2];
}

int estimateMaximumRealtimeVoices() {
    int low = 1;
    int high = kKeyCount;
    int best = 0;

    while (low <= high) {
        const int middle = low + (high - low) / 2;
        const double occupancy = measureOccupancyPercent(middle, 3);

        if (occupancy <= 100.0) {
            best = middle;
            low = middle + 1;
        } else {
            high = middle - 1;
        }
    }

    return best;
}

} // namespace


std::string getAppleChipName() {
    char buffer[256]{};
    size_t size = sizeof(buffer);

    if (sysctlbyname("machdep.cpu.brand_string", buffer, &size, nullptr, 0) == 0 &&
        size > 1) {
        return std::string(buffer);
    }

#if defined(__aarch64__) || defined(__arm64__)
    size = sizeof(buffer);
    if (sysctlbyname("machdep.cpu.brand_string", buffer, &size, nullptr, 0) == 0 &&
        size > 1) {
        return std::string(buffer);
    }

    size = sizeof(buffer);
    if (sysctlbyname("hw.target", buffer, &size, nullptr, 0) == 0 &&
        size > 1) {
        return std::string(buffer);
    }
#endif

    return {};
}

int runEnginePerformanceTest() {
#if !defined(__OPTIMIZE__)
    std::cout << "Warning: run this benchmark with the Release configuration.\n";
#endif

    const std::string hardwareModel = getAppleChipName();

    const double occupancyPercent = measureOccupancyPercent(kKeyCount, 5);
    const double frameBudgetMicroseconds = 1'000'000.0 / kSampleRate;
    const double averageFrameMicroseconds =
        frameBudgetMicroseconds * occupancyPercent / 100.0;
    const double realtimeMultiple = 100.0 / occupancyPercent;
    const bool realtimeCapable = occupancyPercent <= 100.0;

    std::cout << std::fixed << std::setprecision(2)
              << "bBpiano Lite 88-key DSP benchmark (44.1 kHz, median)\n";

    if (!hardwareModel.empty()) {
        std::cout << "Based on your device performance (" << hardwareModel << ")\n";
    } else {
        std::cout << "Based on your device performance\n";
    }

    std::cout
              << "Frame: " << averageFrameMicroseconds << " us / "
              << frameBudgetMicroseconds << " us, occupancy "
              << occupancyPercent << "%, speed " << realtimeMultiple << "x\n"
              << "88-key realtime: "
              << (realtimeCapable ? "PASS" : "FAIL") << '\n';

    if (!realtimeCapable) {
        std::cout << "Estimated realtime polyphony: "
                  << estimateMaximumRealtimeVoices()
                  << " / " << kKeyCount << " keys\n";
    }

    return realtimeCapable ? 0 : 1;
}
