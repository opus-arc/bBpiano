//
//  midiService.hpp
//  bBpiano Lite
//
//  Created by opus arc on 2026/6/7.
//
//  平台无关代码，通用
//

#ifndef midiService_hpp
#define midiService_hpp

#include "../core/controller.hpp"

#include <algorithm>
#include <atomic>
#include <condition_variable>
#include <chrono>
#include <cstdint>
#include <fstream>
#include <mutex>
#include <stdexcept>
#include <string>
#include <thread>
#include <unordered_set>
#include <utility>
#include <vector>

class MidiParseError : public std::runtime_error {
public:
    explicit MidiParseError(const std::string& message)
        : std::runtime_error(message) {}
};

class MidiPlaybackState {
public:
    void cancel() {
        std::unordered_set<int> notes;

        {
            std::lock_guard<std::mutex> lock(mutex_);
            cancelled_ = true;
            notes = activeNotes_;
            activeNotes_.clear();
        }

        condition_.notify_all();

        for (int note : notes) {
            note_off(note, 0.0);
        }

        resetPedals();
    }

    bool isCancelled() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return cancelled_;
    }

    bool waitForCancelOrTimeout(std::chrono::duration<double> duration) {
        if (duration <= std::chrono::duration<double>::zero()) {
            return isCancelled();
        }

        std::unique_lock<std::mutex> lock(mutex_);
        return condition_.wait_for(lock, duration, [this]() {
            return cancelled_;
        });
    }

    void finish() {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            finished_ = true;
        }
        condition_.notify_all();
    }

    void waitUntilFinished() {
        std::unique_lock<std::mutex> lock(mutex_);
        condition_.wait(lock, [this]() {
            return finished_ || cancelled_;
        });
    }

    void noteOn(int note) {
        std::lock_guard<std::mutex> lock(mutex_);
        activeNotes_.insert(note);
    }

    void noteOff(int note) {
        std::lock_guard<std::mutex> lock(mutex_);
        activeNotes_.erase(note);
    }

private:
    static void resetPedals() {
        softPedal_control(0.0);
        harmonicPedal_control(0.0);
        sostenutoPedal_control(0.0);
        sustainPedal_control(0.0);
    }
    mutable std::mutex mutex_;
    bool cancelled_ = false;
    bool finished_ = false;
    std::condition_variable condition_;
    std::unordered_set<int> activeNotes_;
};

class MidiService {
public:
    struct MidiEvent {
    static std::vector<MidiEvent> loadEvents(
        const std::string& midiFilePath
    ) {
        std::vector<uint8_t> data = readWholeFile(midiFilePath);

        std::vector<MidiEvent> allEvents = parseMidiFile(std::move(data));
        std::vector<MidiEvent> pianoEvents = selectPianoEvents(allEvents);

        std::sort(
            pianoEvents.begin(),
            pianoEvents.end(),
            [](const MidiEvent& lhs, const MidiEvent& rhs) {
                return lhs.timeInSeconds < rhs.timeInSeconds;
            }
        );

        return pianoEvents;
    }
        uint64_t tick = 0;
        double timeInSeconds = 0.0;
        uint8_t status = 0;
        uint8_t data1 = 0;
        uint8_t data2 = 0;
        std::string trackName;

        int channel() const {
            return static_cast<int>(status & 0x0F);
        }

        uint8_t statusType() const {
            return status & 0xF0;
        }

        bool isNoteOn() const {
            return statusType() == 0x90 && data2 > 0;
        }

        bool isNoteOff() const {
            return statusType() == 0x80 || (statusType() == 0x90 && data2 == 0);
        }

        bool isControlChange() const {
            return statusType() == 0xB0;
        }

        bool isPedalControlChange() const {
            if (!isControlChange()) {
                return false;
            }

            switch (data1) {
                case 64:
                case 66:
                case 67:
                case 68:
                    return true;

                default:
                    return false;
            }
        }

        double normalizedData2() const {
            return std::clamp(static_cast<double>(data2) / 127.0, 0.0, 1.0);
        }
    };

    struct TempoChange {
        uint64_t tick = 0;
        double microsecondsPerQuarter = 500'000.0;
    };

    class MidiReader {
    public:
        explicit MidiReader(std::vector<uint8_t> data)
            : data_(std::move(data)) {}

        size_t index() const {
            return index_;
        }

        void setIndex(size_t index) {
            index_ = index;
        }

        uint8_t readByte() {
            if (index_ >= data_.size()) {
                throw MidiParseError("Invalid MIDI file.");
            }
            return data_[index_++];
        }

        std::vector<uint8_t> readData(size_t length) {
            if (index_ + length > data_.size()) {
                throw MidiParseError("Invalid MIDI file.");
            }

            std::vector<uint8_t> result(
                data_.begin() + static_cast<std::ptrdiff_t>(index_),
                data_.begin() + static_cast<std::ptrdiff_t>(index_ + length)
            );
            index_ += length;
            return result;
        }

        std::string readString(size_t length) {
            if (index_ + length > data_.size()) {
                return {};
            }

            std::string result(
                data_.begin() + static_cast<std::ptrdiff_t>(index_),
                data_.begin() + static_cast<std::ptrdiff_t>(index_ + length)
            );
            index_ += length;
            return result;
        }

        uint16_t readUInt16() {
            const uint16_t high = readByte();
            const uint16_t low = readByte();
            return static_cast<uint16_t>((high << 8) | low);
        }

        uint32_t readUInt32() {
            const uint32_t byte1 = readByte();
            const uint32_t byte2 = readByte();
            const uint32_t byte3 = readByte();
            const uint32_t byte4 = readByte();
            return (byte1 << 24) | (byte2 << 16) | (byte3 << 8) | byte4;
        }

        uint32_t readVariableLengthQuantity() {
            uint32_t value = 0;

            for (int i = 0; i < 4; ++i) {
                const uint8_t byte = readByte();
                value = (value << 7) | static_cast<uint32_t>(byte & 0x7F);

                if ((byte & 0x80) == 0) {
                    return value;
                }
            }

            throw MidiParseError("Invalid MIDI file.");
        }

        void skip(size_t count) {
            if (index_ + count > data_.size()) {
                throw MidiParseError("Invalid MIDI file.");
            }
            index_ += count;
        }

    private:
        std::vector<uint8_t> data_;
        size_t index_ = 0;
    };

public:
    MidiService() = delete;

    // 对应 Swift:
    // MidiService.play(playbackRate:startTime:midiFileURL:)
    static void play(
        double playbackRate,
        double startTime,
        const std::string& midiFilePath
    ) {
        if (playbackRate <= 0.0) {
            throw std::runtime_error("playbackRate must be greater than 0.");
        }

        stop();

        std::vector<MidiEvent> pianoEvents = MidiEvent::loadEvents(midiFilePath);

        const double safeStartTime = std::max(0.0, startTime);

        pianoEvents.erase(
            std::remove_if(
                pianoEvents.begin(),
                pianoEvents.end(),
                [safeStartTime](const MidiEvent& event) {
                    return event.timeInSeconds < safeStartTime;
                }
            ),
            pianoEvents.end()
        );

        std::sort(
            pianoEvents.begin(),
            pianoEvents.end(),
            [](const MidiEvent& lhs, const MidiEvent& rhs) {
                return lhs.timeInSeconds < rhs.timeInSeconds;
            }
        );

        auto* state = new MidiPlaybackState();
        playbackState_.store(state);

        playbackThread_ = std::thread(
            [events = std::move(pianoEvents), playbackRate, safeStartTime, state]() {
                playEvents(events, playbackRate, safeStartTime, *state);
            }
        );
    }

    static void stop() {
        MidiPlaybackState* state = playbackState_.exchange(nullptr);
        if (state != nullptr) {
            state->cancel();
        }

        if (playbackThread_.joinable()) {
            playbackThread_.join();
        }

//        resetPedals();

        delete state;
    }

    static void waitUntilFinished() {
        MidiPlaybackState* state = playbackState_.load();
        if (state != nullptr) {
            state->waitUntilFinished();
        }

        if (playbackThread_.joinable()) {
            playbackThread_.join();
        }

        resetPedals();

        MidiPlaybackState* finishedState = playbackState_.exchange(nullptr);
        delete finishedState;
    }

private:
    static void resetPedals() {
        softPedal_control(0.0);
        harmonicPedal_control(0.0);
        sostenutoPedal_control(0.0);
        sustainPedal_control(0.0);
    }
    static std::vector<uint8_t> readWholeFile(const std::string& path) {
        std::ifstream file(path, std::ios::binary);
        if (!file) {
            throw MidiParseError("Invalid MIDI file.");
        }

        file.seekg(0, std::ios::end);
        const std::streampos size = file.tellg();
        file.seekg(0, std::ios::beg);

        if (size <= 0) {
            throw MidiParseError("Invalid MIDI file.");
        }

        std::vector<uint8_t> data(static_cast<size_t>(size));
        file.read(reinterpret_cast<char*>(data.data()), size);

        if (!file) {
            throw MidiParseError("Invalid MIDI file.");
        }

        return data;
    }

    static void playEvents(
        const std::vector<MidiEvent>& events,
        double playbackRate,
        double startTime,
        MidiPlaybackState& state
    ) {
        double lastScheduledTime = startTime;

        for (const MidiEvent& event : events) {
            if (state.isCancelled()) {
                return;
            }

            const double delta = std::max(0.0, event.timeInSeconds - lastScheduledTime) / playbackRate;
            if (delta > 0.0 && state.waitForCancelOrTimeout(std::chrono::duration<double>(delta))) {
                return;
            }
            lastScheduledTime = event.timeInSeconds;

            if (state.isCancelled()) {
                return;
            }

            if (event.isNoteOn()) {
                const int note = static_cast<int>(event.data1);

                // Note velocity must stay in the MIDI domain here: 0~127.
                // HammerModel maps this raw MIDI velocity to physical hammer
                // impact velocity in m/s. Do not normalize note velocity here.
                const double velocity = static_cast<double>(event.data2);

                state.noteOn(note);
                note_on(note, velocity);
            } else if (event.isNoteOff()) {
                const int note = static_cast<int>(event.data1);

                // Note-off velocity is currently not used physically, but keep
                // the same MIDI-domain convention for semantic consistency.
                const double velocity = static_cast<double>(event.data2);

                state.noteOff(note);
                note_off(note, velocity);
            } else if (event.isPedalControlChange()) {
                dispatchPedalEvent(event);
            }
        }

        state.finish();
    }

    static void dispatchPedalEvent(const MidiEvent& event) {
        const double depth = event.normalizedData2();

        switch (event.data1) {
            case 64:
                sustainPedal_control(depth);
                break;

            case 66:
                sostenutoPedal_control(depth);
                break;

            case 67:
                softPedal_control(depth);
                break;

            case 68:
                harmonicPedal_control(depth);
                break;

            default:
                break;
        }
    }

    static std::vector<MidiEvent> selectPianoEvents(const std::vector<MidiEvent>& events) {
        std::vector<MidiEvent> pianoRelevantEvents;
        pianoRelevantEvents.reserve(events.size());

        for (const MidiEvent& event : events) {
            if (event.isNoteOn() || event.isNoteOff() || event.isPedalControlChange()) {
                pianoRelevantEvents.push_back(event);
            }
        }

        std::vector<MidiEvent> explicitlyNamedPianoEvents;
        for (const MidiEvent& event : pianoRelevantEvents) {
            std::string name = event.trackName;
            std::transform(name.begin(), name.end(), name.begin(), [](unsigned char c) {
                return static_cast<char>(std::tolower(c));
            });

            if (name.find("piano") != std::string::npos ||
                name.find("keyboard") != std::string::npos ||
                name.find("grand") != std::string::npos) {
                explicitlyNamedPianoEvents.push_back(event);
            }
        }

        if (!explicitlyNamedPianoEvents.empty()) {
            return explicitlyNamedPianoEvents;
        }

        std::vector<MidiEvent> channelZeroEvents;
        for (const MidiEvent& event : pianoRelevantEvents) {
            if (event.channel() == 0) {
                channelZeroEvents.push_back(event);
            }
        }

        if (!channelZeroEvents.empty()) {
            return channelZeroEvents;
        }

        return pianoRelevantEvents;
    }

    static std::vector<MidiEvent> parseMidiFile(std::vector<uint8_t> data) {
        MidiReader reader(std::move(data));

        if (reader.readString(4) != "MThd") {
            throw MidiParseError("Missing MIDI header chunk.");
        }

        const uint32_t headerLength = reader.readUInt32();
        const uint16_t format = reader.readUInt16();
        const uint16_t trackCount = reader.readUInt16();
        const uint16_t division = reader.readUInt16();

        if (format != 0 && format != 1) {
            throw MidiParseError("Unsupported MIDI format: " + std::to_string(format) + ".");
        }

        if ((division & 0x8000) != 0) {
            throw MidiParseError("SMPTE time division is not supported.");
        }

        const double ticksPerQuarterNote = static_cast<double>(division);

        if (headerLength > 6) {
            reader.skip(static_cast<size_t>(headerLength - 6));
        }

        std::vector<MidiEvent> events;
        std::vector<TempoChange> tempoChanges;
        tempoChanges.push_back(TempoChange{0, 500'000.0});

        for (uint16_t track = 0; track < trackCount; ++track) {
            if (reader.readString(4) != "MTrk") {
                throw MidiParseError("Corrupt MIDI track chunk.");
            }

            const size_t trackLength = static_cast<size_t>(reader.readUInt32());
            const size_t trackEndIndex = reader.index() + trackLength;

            uint64_t absoluteTick = 0;
            uint8_t runningStatus = 0;
            bool hasRunningStatus = false;
            std::string trackName;

            while (reader.index() < trackEndIndex) {
                const uint32_t deltaTicks = reader.readVariableLengthQuantity();
                absoluteTick += static_cast<uint64_t>(deltaTicks);

                const uint8_t firstByte = reader.readByte();
                uint8_t status = 0;
                uint8_t data1 = 0;
                bool hasData1 = false;

                if (firstByte < 0x80) {
                    if (!hasRunningStatus) {
                        throw MidiParseError("Corrupt MIDI track chunk.");
                    }
                    status = runningStatus;
                    data1 = firstByte;
                    hasData1 = true;
                } else {
                    status = firstByte;
                }

                if (status == 0xFF) {
                    const uint8_t metaType = reader.readByte();
                    const size_t length = static_cast<size_t>(reader.readVariableLengthQuantity());
                    const std::vector<uint8_t> metaData = reader.readData(length);

                    if (metaType == 0x03) {
                        trackName.assign(metaData.begin(), metaData.end());
                    } else if (metaType == 0x51 && metaData.size() == 3) {
                        const double microsecondsPerQuarter =
                            static_cast<double>(metaData[0]) * 65'536.0 +
                            static_cast<double>(metaData[1]) * 256.0 +
                            static_cast<double>(metaData[2]);

                        tempoChanges.push_back(TempoChange{absoluteTick, microsecondsPerQuarter});
                    }

                    hasRunningStatus = false;
                    continue;
                }

                if (status == 0xF0 || status == 0xF7) {
                    const size_t length = static_cast<size_t>(reader.readVariableLengthQuantity());
                    reader.skip(length);
                    hasRunningStatus = false;
                    continue;
                }

                runningStatus = status;
                hasRunningStatus = true;

                const uint8_t statusType = status & 0xF0;
                const uint8_t firstDataByte = hasData1 ? data1 : reader.readByte();

                switch (statusType) {
                    case 0x80:
                    case 0x90: {
                        const uint8_t velocity = reader.readByte();
                        events.push_back(
                            MidiEvent{
                                absoluteTick,
                                0.0,
                                status,
                                firstDataByte,
                                velocity,
                                trackName
                            }
                        );
                        break;
                    }

                    case 0xA0:
                    case 0xE0:
                        static_cast<void>(reader.readByte());
                        break;

                    case 0xB0: {
                        const uint8_t value = reader.readByte();
                        events.push_back(
                            MidiEvent{
                                absoluteTick,
                                0.0,
                                status,
                                firstDataByte,
                                value,
                                trackName
                            }
                        );
                        break;
                    }

                    case 0xC0:
                    case 0xD0:
                        break;

                    default:
                        throw MidiParseError("Corrupt MIDI track chunk.");
                }
            }

            if (reader.index() != trackEndIndex) {
                reader.setIndex(trackEndIndex);
            }
        }

        applyTempoMap(events, tempoChanges, ticksPerQuarterNote);

        std::sort(
            events.begin(),
            events.end(),
            [](const MidiEvent& lhs, const MidiEvent& rhs) {
                if (lhs.timeInSeconds == rhs.timeInSeconds) {
                    return lhs.tick < rhs.tick;
                }
                return lhs.timeInSeconds < rhs.timeInSeconds;
            }
        );

        return events;
    }

    static void applyTempoMap(
        std::vector<MidiEvent>& events,
        std::vector<TempoChange>& tempoChanges,
        double ticksPerQuarterNote
    ) {
        std::sort(
            tempoChanges.begin(),
            tempoChanges.end(),
            [](const TempoChange& lhs, const TempoChange& rhs) {
                return lhs.tick < rhs.tick;
            }
        );

        tempoChanges.erase(
            std::unique(
                tempoChanges.begin(),
                tempoChanges.end(),
                [](const TempoChange& lhs, const TempoChange& rhs) {
                    return lhs.tick == rhs.tick;
                }
            ),
            tempoChanges.end()
        );

        std::sort(
            events.begin(),
            events.end(),
            [](const MidiEvent& lhs, const MidiEvent& rhs) {
                return lhs.tick < rhs.tick;
            }
        );

        size_t tempoIndex = 0;
        uint64_t previousTick = 0;
        double seconds = 0.0;
        double currentTempo = tempoChanges.empty() ? 500'000.0 : tempoChanges.front().microsecondsPerQuarter;

        for (MidiEvent& event : events) {
            while (tempoIndex + 1 < tempoChanges.size() && tempoChanges[tempoIndex + 1].tick <= event.tick) {
                const TempoChange& nextTempo = tempoChanges[tempoIndex + 1];
                const uint64_t deltaTicks = nextTempo.tick - previousTick;

                seconds += (
                    static_cast<double>(deltaTicks) *
                    currentTempo /
                    1'000'000.0
                ) / ticksPerQuarterNote;

                previousTick = nextTempo.tick;
                currentTempo = nextTempo.microsecondsPerQuarter;
                ++tempoIndex;
            }

            const uint64_t deltaTicks = event.tick - previousTick;
            event.timeInSeconds = seconds + (
                static_cast<double>(deltaTicks) *
                currentTempo /
                1'000'000.0
            ) / ticksPerQuarterNote;
        }
    }

private:
    inline static std::atomic<MidiPlaybackState*> playbackState_ { nullptr };
    inline static std::thread playbackThread_;
};

#endif /* midiService_hpp */
