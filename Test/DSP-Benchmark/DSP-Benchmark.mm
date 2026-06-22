//
//  DSP-Benchmark.mm
//  HammerStringEvaluationTests
//
//  Created by opus arc on 2026/6/22.
//

#import <XCTest/XCTest.h>

#include "../../Cli/bBpiano Lite/core/piano/PianoModel.hpp"

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
    std::streambuf* previousBuffer_ = nullptr;
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

std::string getAppleChipName() {
    char buffer[256]{};
    size_t size = sizeof(buffer);

    if (sysctlbyname("machdep.cpu.brand_string", buffer, &size, nullptr, 0) == 0 &&
        size > 1) {
        return std::string(buffer);
    }

    size = sizeof(buffer);
    if (sysctlbyname("hw.target", buffer, &size, nullptr, 0) == 0 &&
        size > 1) {
        return std::string(buffer);
    }

    return {};
}

struct BenchmarkResult {
    double occupancyPercent = 0.0;
    double frameBudgetMicroseconds = 0.0;
    double averageFrameMicroseconds = 0.0;
    double realtimeMultiple = 0.0;
    bool realtimeCapable = false;
    int estimatedRealtimeVoices = kKeyCount;
};

BenchmarkResult runBBpianoLiteBenchmark() {
    BenchmarkResult result;

    result.occupancyPercent = measureOccupancyPercent(kKeyCount, 5);
    result.frameBudgetMicroseconds = 1'000'000.0 / kSampleRate;
    result.averageFrameMicroseconds =
        result.frameBudgetMicroseconds * result.occupancyPercent / 100.0;
    result.realtimeMultiple = 100.0 / result.occupancyPercent;
    result.realtimeCapable = result.occupancyPercent <= 100.0;

    if (!result.realtimeCapable) {
        result.estimatedRealtimeVoices = estimateMaximumRealtimeVoices();
    }

    return result;
}

} // namespace

@interface DSP_Benchmark : XCTestCase
@end

@implementation DSP_Benchmark

- (void)setUp {
    [super setUp];
    self.continueAfterFailure = YES;
}

-(void)tearDown {
    [super tearDown];
}

-(void)testBBpianoLite88KeyDSPBenchmark {
#if !defined(__OPTIMIZE__)
    NSLog(@"Run this benchmark with the Release configuration for meaningful timing.");
#endif

    const BenchmarkResult result = runBBpianoLiteBenchmark();

    const std::string chipName = getAppleChipName();

    NSString *deviceLine = chipName.empty()
        ? @"Based on your device performance"
        : [NSString stringWithFormat:@"Based on your device performance (%s)", chipName.c_str()];

    NSString *summary = [NSString stringWithFormat:
                         @"bBpiano Lite 88-key DSP benchmark (44.1 kHz, median)\n"
                         @"%@\n"
                         @"Frame: %.2f us / %.2f us, occupancy %.2f%%, speed %.2fx\n"
                         @"88-key realtime: %@\n"
                         @"Estimated realtime polyphony: %d / %d keys",
                         deviceLine,
                         result.averageFrameMicroseconds,
                         result.frameBudgetMicroseconds,
                         result.occupancyPercent,
                         result.realtimeMultiple,
                         result.realtimeCapable ? @"PASS" : @"FAIL",
                         result.estimatedRealtimeVoices,
                         kKeyCount];

    NSLog(@"%@", summary);

    XCTAssertLessThanOrEqual(result.occupancyPercent,
                             100.0,
                             @"88-key realtime DSP occupancy exceeded the realtime budget. %@",
                             summary);
}

- (void)testBBpianoLiteFramePerformanceWithXCTestMeasure {
    std::unique_ptr<PianoModel> piano;

    {
        ScopedCoutSilencer silenceSetupLogs;
        piano = std::make_unique<PianoModel>();

        for (int keyIndex : distributedKeyIndices(kKeyCount)) {
            piano->note_on(21 + keyIndex, kMidiVelocity);
        }
    }

    __block volatile float sampleSink = 0.0f;

    for (int frame = 0; frame < kWarmupFrames; ++frame) {
        piano->pianoMovement();
        sampleSink = sampleSink + piano->getSample();
    }

    PianoModel *rawPiano = piano.get();

    [self measureBlock:^{
        for (int frame = 0; frame < kMeasureFrames; ++frame) {
            rawPiano->pianoMovement();
            sampleSink = sampleSink + rawPiano->getSample();
        }
    }];
}

@end
