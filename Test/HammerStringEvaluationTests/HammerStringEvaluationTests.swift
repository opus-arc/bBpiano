//
//  HammerStringEvaluationTests.swift
//  HammerStringEvaluationTests
//
//  Created by opus arc on 2026/5/23.
//

import XCTest
import Darwin

final class HammerStringEvaluationTests: XCTestCase {
    
    let isRunningTests =
    ProcessInfo.processInfo.environment["XCTestConfigurationFilePath"] != nil
    

    private let sampleRate = 44_100.0
    private let frameCount: Int32 = 512
    private let amplitudeLimiter = 0.8

    private var expectedBufferSeconds: Double {
        Double(frameCount) / sampleRate
    }

    override func setUp() {
        super.setUp()

        bBpiano_init()
        Thread.sleep(forTimeInterval: 2.0)
    }

    func testPianoEngineEfficiencyScore() throws {
        let benchmarkVelocity = 120.0
        let allMidiNotes = Array(21...108).map(Int32.init)

        var results: [(midi: Int32, realtimeRate: Double)] = []

        resetEngineState()

        for midi in allMidiNotes {
            resetEngineState()
            let rate = measureSingleNoteRealtimeRate(midi: midi, velocity: benchmarkVelocity)
            results.append((midi, rate))
            resetEngineState()
        }

        let lowAverage = averageRealtimeRate(in: results, midiRange: 21...50)
        let middleAverage = averageRealtimeRate(in: results, midiRange: 51...79)
        let highAverage = averageRealtimeRate(in: results, midiRange: 80...108)
        let allAverage = averageRealtimeRate(in: results, midiRange: 21...108)
        let commonRangeAverage = averageRealtimeRate(in: results, midiRange: 36...84)
        let estimatedCommonPolyphonyUnder90Percent = 0.90 / commonRangeAverage

        writeEngineTestResultJSON(
            velocity: benchmarkVelocity,
            lowAverage: lowAverage,
            middleAverage: middleAverage,
            highAverage: highAverage,
            allAverage: allAverage,
            estimatedCommonPolyphonyUnder90Percent: estimatedCommonPolyphonyUnder90Percent
        )

        print("\n========== bBpiano Engine Benchmark ==========")
        print(String(format: "Low average: %.2f%%", lowAverage * 100.0))
        print(String(format: "Middle average: %.2f%%", middleAverage * 100.0))
        print(String(format: "High average: %.2f%%", highAverage * 100.0))
        print(String(format: "All-note average: %.2f%%", allAverage * 100.0))
        print(String(format: "Estimated C2-C6 polyphony under 90%%: %.2f notes", estimatedCommonPolyphonyUnder90Percent))
        print("==============================================\n")

        XCTAssertLessThan(allAverage, 0.90)
    }

    private func measureSingleNoteRealtimeRate(midi: Int32, velocity: Double) -> Double {
        all_silence()
        warmUpSilentBuffers(count: 4)

        note_on(midi, velocity)
        warmUpActiveBuffers(count: 4)

        let rate = measureAverageRealtimeRate(seconds: 1.5)

        note_off(midi, 0.0)
        all_silence()
        warmUpSilentBuffers(count: 2)
        return rate
    }


    private func measureAverageRealtimeRate(seconds: Double) -> Double {
        let bufferCount = max(1, Int(seconds / expectedBufferSeconds))
        var buffer = [Float](repeating: 0.0, count: Int(frameCount))
        var realtimeRates: [Double] = []
        realtimeRates.reserveCapacity(bufferCount)

        for _ in 0..<bufferCount {
            let start = clock_gettime_nsec_np(CLOCK_UPTIME_RAW)
            get_next_buffer(&buffer, frameCount, amplitudeLimiter)
            let end = clock_gettime_nsec_np(CLOCK_UPTIME_RAW)

            let actualSeconds = Double(end - start) / 1_000_000_000.0
            realtimeRates.append(actualSeconds / expectedBufferSeconds)
        }

        return realtimeRates.reduce(0.0, +) / Double(realtimeRates.count)
    }

    private func averageRealtimeRate(
        in results: [(midi: Int32, realtimeRate: Double)],
        midiRange: ClosedRange<Int32>
    ) -> Double {
        let filteredRates = results
            .filter { midiRange.contains($0.midi) }
            .map(\.realtimeRate)

        guard !filteredRates.isEmpty else { return 0.0 }
        return filteredRates.reduce(0.0, +) / Double(filteredRates.count)
    }

    private func writeEngineTestResultJSON(
        velocity: Double,
        lowAverage: Double,
        middleAverage: Double,
        highAverage: Double,
        allAverage: Double,
        estimatedCommonPolyphonyUnder90Percent: Double
    ) {
        let result: [String: Any] = [
            "estimatedPolyphonyUnder90PercentLoad": String(format: "%.2f", estimatedCommonPolyphonyUnder90Percent),
            "allNoteAverage": String(format: "%.2f%%", allAverage * 100.0),
            "lowAverage": String(format: "%.2f%%", lowAverage * 100.0),
            "middleAverage": String(format: "%.2f%%", middleAverage * 100.0),
            "highAverage": String(format: "%.2f%%", highAverage * 100.0)
        ]

        do {
            let data = try JSONSerialization.data(
                withJSONObject: result,
                options: [.prettyPrinted]
            )

            let testFileURL = URL(fileURLWithPath: #filePath)
            let outputURL = testFileURL
                .deletingLastPathComponent()
                .appendingPathComponent("EngineTestResult.json")

            try data.write(to: outputURL, options: .atomic)
            print("Engine test result JSON written to: \(outputURL.path)")
        } catch {
            XCTFail("Failed to write EngineTestResult.json: \(error)")
        }
    }

    private func resetEngineState() {
        all_silence()
        warmUpSilentBuffers(count: 4)
    }

    private func warmUpSilentBuffers(count: Int) {
        all_silence()
        processBuffers(count: count)
    }

    private func warmUpActiveBuffers(count: Int) {
        processBuffers(count: count)
    }

    private func processBuffers(count: Int) {
        guard count > 0 else { return }

        var buffer = [Float](repeating: 0.0, count: Int(frameCount))
        for _ in 0..<count {
            get_next_buffer(&buffer, frameCount, amplitudeLimiter)
        }
    }

}
