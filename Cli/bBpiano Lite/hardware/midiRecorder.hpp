//
//  midiRecorder.hpp
//  bBpiano
//
//  Created by opus arc on 2026/6/8.
//

#ifndef midiRecorder_hpp
#define midiRecorder_hpp

#include "midiInputHub.hpp"

#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <fstream>
#include <mutex>
#include <string>
#include <vector>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <algorithm>

class MidiRecorder {
public:
    struct Event {
        uint64_t timeUs = 0;
        std::vector<uint8_t> message;
    };

    MidiRecorder() = delete;

    static void start(const std::string& filePath) {
        if (isRecording()) {
            return;
        }

        filePath_ = filePath;
        events_.clear();
        startTime_ = Clock::now();

        MidiInputHub::start();

        handlerToken_ = MidiInputHub::addHandler(
            [](const std::vector<uint8_t>& message) {
                recordMessage(message);
            }
        );

        {
            std::lock_guard<std::mutex> lock(mutex_);
            recording_ = true;
        }
    }

    static void stop() {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            if (!recording_) {
                return;
            }
        }

        if (handlerToken_ != 0) {
            MidiInputHub::removeHandler(handlerToken_);
            handlerToken_ = 0;
        }

        writeMidiFile();

        if (!MidiInputHub::hasHandlers()) {
            MidiInputHub::stop();
        }

        {
            std::lock_guard<std::mutex> lock(mutex_);
            recording_ = false;
        }

        condition_.notify_all();
    }

    static bool isRecording() {
        std::lock_guard<std::mutex> lock(mutex_);
        return recording_;
    }

    static void waitUntilStopped() {
        std::unique_lock<std::mutex> lock(mutex_);
        condition_.wait(lock, [](){ return !recording_; });
    }

private:
    using Clock = std::chrono::steady_clock;

    static void recordMessage(const std::vector<uint8_t>& message) {
        if (message.empty()) {
            return;
        }

        Event event;
        event.timeUs = static_cast<uint64_t>(
            std::chrono::duration_cast<std::chrono::microseconds>(
                Clock::now() - startTime_
            ).count()
        );
        event.message = message;

        std::lock_guard<std::mutex> lock(eventMutex_);
        events_.push_back(std::move(event));
    }

    static std::string buildDefaultFileName() {
        const std::time_t now = std::time(nullptr);
        std::tm localTime = *std::localtime(&now);

        uint64_t durationSeconds = 0;
        size_t noteCount = 0;

        {
            std::lock_guard<std::mutex> lock(eventMutex_);

            if (!events_.empty()) {
                durationSeconds = events_.back().timeUs / 1000000ULL;
            }

            for (const Event& event : events_) {
                if (event.message.size() >= 3) {
                    const uint8_t status = event.message[0] & 0xF0;
                    if (status == 0x90 && event.message[2] > 0) {
                        ++noteCount;
                    }
                }
            }
        }

        static const char* weekdays[] = {
            "Sunday", "Monday", "Tuesday", "Wednesday",
            "Thursday", "Friday", "Saturday"
        };

        std::ostringstream oss;
        oss
            << std::put_time(&localTime, "%Y-%m-%d %H-%M")
            << " (" << weekdays[localTime.tm_wday] << ") "
            << noteCount
            << " notes, "
            << durationSeconds
            << " seconds.mid";

        return oss.str();
    }

    static void writeUInt16BE(std::ofstream& file, uint16_t value) {
        file.put(static_cast<char>((value >> 8) & 0xFF));
        file.put(static_cast<char>(value & 0xFF));
    }

    static void writeUInt32BE(std::ofstream& file, uint32_t value) {
        file.put(static_cast<char>((value >> 24) & 0xFF));
        file.put(static_cast<char>((value >> 16) & 0xFF));
        file.put(static_cast<char>((value >> 8) & 0xFF));
        file.put(static_cast<char>(value & 0xFF));
    }

    static void appendUInt32BE(std::vector<uint8_t>& out, uint32_t value) {
        out.push_back(static_cast<uint8_t>((value >> 24) & 0xFF));
        out.push_back(static_cast<uint8_t>((value >> 16) & 0xFF));
        out.push_back(static_cast<uint8_t>((value >> 8) & 0xFF));
        out.push_back(static_cast<uint8_t>(value & 0xFF));
    }

    static void appendVariableLengthQuantity(std::vector<uint8_t>& out, uint32_t value) {
        uint8_t buffer[5] {};
        int index = 4;

        buffer[index] = static_cast<uint8_t>(value & 0x7F);
        value >>= 7;

        while (value > 0 && index > 0) {
            --index;
            buffer[index] = static_cast<uint8_t>((value & 0x7F) | 0x80);
            value >>= 7;
        }

        for (; index < 5; ++index) {
            out.push_back(buffer[index]);
        }
    }

    static uint32_t microsecondsToTicks(uint64_t timeUs) {
        return static_cast<uint32_t>((timeUs * ticksPerQuarterNote_) / microsecondsPerQuarterNote_);
    }

    static bool isSupportedMidiMessage(const std::vector<uint8_t>& message) {
        if (message.empty()) {
            return false;
        }

        const uint8_t status = message[0];
        const uint8_t statusType = status & 0xF0;

        switch (statusType) {
            case 0x80:
            case 0x90:
            case 0xA0:
            case 0xB0:
            case 0xE0:
                return message.size() >= 3;

            case 0xC0:
            case 0xD0:
                return message.size() >= 2;

            default:
                return false;
        }
    }

    static void writeMidiFile() {
        if (filePath_.empty()) {
            filePath_ = buildDefaultFileName();
        }

        std::vector<Event> events;
        {
            std::lock_guard<std::mutex> lock(eventMutex_);
            events = events_;
        }

        std::sort(
            events.begin(),
            events.end(),
            [](const Event& lhs, const Event& rhs) {
                return lhs.timeUs < rhs.timeUs;
            }
        );

        std::vector<uint8_t> track;

        // Tempo meta event: 120 BPM = 500000 microseconds per quarter note.
        appendVariableLengthQuantity(track, 0);
        track.push_back(0xFF);
        track.push_back(0x51);
        track.push_back(0x03);
        track.push_back(static_cast<uint8_t>((microsecondsPerQuarterNote_ >> 16) & 0xFF));
        track.push_back(static_cast<uint8_t>((microsecondsPerQuarterNote_ >> 8) & 0xFF));
        track.push_back(static_cast<uint8_t>(microsecondsPerQuarterNote_ & 0xFF));

        uint32_t previousTick = 0;

        for (const Event& event : events) {
            if (!isSupportedMidiMessage(event.message)) {
                continue;
            }

            const uint32_t currentTick = microsecondsToTicks(event.timeUs);
            const uint32_t deltaTick = currentTick >= previousTick ? currentTick - previousTick : 0;
            previousTick = currentTick;

            appendVariableLengthQuantity(track, deltaTick);
            track.insert(track.end(), event.message.begin(), event.message.end());
        }

        // End of Track meta event.
        appendVariableLengthQuantity(track, 0);
        track.push_back(0xFF);
        track.push_back(0x2F);
        track.push_back(0x00);

        std::ofstream file(filePath_, std::ios::binary);
        if (!file) {
            return;
        }

        file.write("MThd", 4);
        writeUInt32BE(file, 6);
        writeUInt16BE(file, 0); // format 0
        writeUInt16BE(file, 1); // one track
        writeUInt16BE(file, ticksPerQuarterNote_);

        file.write("MTrk", 4);
        writeUInt32BE(file, static_cast<uint32_t>(track.size()));
        file.write(reinterpret_cast<const char*>(track.data()), static_cast<std::streamsize>(track.size()));
    }

    static constexpr uint16_t ticksPerQuarterNote_ = 480;
    static constexpr uint32_t microsecondsPerQuarterNote_ = 500000;

private:
    inline static std::mutex mutex_;
    inline static std::condition_variable condition_;
    inline static bool recording_ = false;

    inline static std::mutex eventMutex_;
    inline static std::vector<Event> events_;

    inline static int handlerToken_ = 0;
    inline static std::string filePath_;
    inline static Clock::time_point startTime_;
};

#endif /* midiRecorder_hpp */
