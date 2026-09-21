//
//  midiInputHub.hpp
//  bBpiano
//
//  Created by opus arc on 2026/6/8.
//

#ifndef midiInputHub_hpp
#define midiInputHub_hpp

#include <CoreMIDI/CoreMIDI.h>
#include <CoreServices/CoreServices.h>

#include <cstdint>
#include <functional>
#include <mutex>
#include <span>
#include <stdexcept>
#include <vector>
#include <algorithm>
#include <array>

class MidiInputHub {
public:
    using MidiMessage = std::span<const uint8_t>;
    using MessageHandler = std::function<void(MidiMessage)>;

    struct PedalState {
        double soft;
        double harmonic;
        double sostenuto;
        double sustain;

        constexpr PedalState()
            : soft(0.0),
              harmonic(0.0),
              sostenuto(0.0),
              sustain(0.0) {}

        constexpr PedalState(
            double softDepth,
            double harmonicDepth,
            double sostenutoDepth,
            double sustainDepth
        )
            : soft(softDepth),
              harmonic(harmonicDepth),
              sostenuto(sostenutoDepth),
              sustain(sustainDepth) {}
    };

    using PedalHandler = std::function<void(const PedalState&)>;

    static constexpr uint8_t kSustainPedalController = 64;
    static constexpr uint8_t kSostenutoPedalController = 66;
    static constexpr uint8_t kSoftPedalController = 67;

    // No universal General MIDI controller exists for a piano harmonic pedal.
    // CC68 is reserved here as bBpiano's internal default mapping.
    static constexpr uint8_t kHarmonicPedalController = 68;

    MidiInputHub() = delete;
    MidiInputHub(const MidiInputHub&) = delete;
    MidiInputHub& operator=(const MidiInputHub&) = delete;

    static int addHandler(MessageHandler handler) {
        if (!handler) {
            throw std::runtime_error("Invalid MIDI message handler.");
        }

        std::lock_guard<std::mutex> lock(mutex_);

        const int token = nextToken_++;
        handlers_.push_back(HandlerEntry{token, std::move(handler)});
        return token;
    }

    static int addPedalHandler(PedalHandler handler) {
        if (!handler) {
            throw std::runtime_error("Invalid MIDI pedal handler.");
        }

        std::lock_guard<std::mutex> lock(mutex_);

        const int token = nextToken_++;
        pedalHandlers_.push_back(PedalHandlerEntry{token, std::move(handler)});
        return token;
    }

    static void removeHandler(int token) {
        std::lock_guard<std::mutex> lock(mutex_);

        handlers_.erase(
            std::remove_if(
                handlers_.begin(),
                handlers_.end(),
                [token](const HandlerEntry& entry) {
                    return entry.token == token;
                }
            ),
            handlers_.end()
        );
    }

    static void removePedalHandler(int token) {
        std::lock_guard<std::mutex> lock(mutex_);

        pedalHandlers_.erase(
            std::remove_if(
                pedalHandlers_.begin(),
                pedalHandlers_.end(),
                [token](const PedalHandlerEntry& entry) {
                    return entry.token == token;
                }
            ),
            pedalHandlers_.end()
        );
    }

    static void start() {
        std::lock_guard<std::mutex> lock(mutex_);

        if (running_) {
            return;
        }

        const ItemCount sourceCount = MIDIGetNumberOfSources();
        if (sourceCount == 0) {
            throw std::runtime_error("No MIDI input device found.");
        }

        OSStatus status = MIDIClientCreate(
            CFSTR("bbpl MIDI Input Hub"),
            nullptr,
            nullptr,
            &client_
        );

        if (status != noErr || client_ == 0) {
            client_ = 0;
            throw std::runtime_error("Failed to create CoreMIDI client.");
        }

        status = MIDIInputPortCreate(
            client_,
            CFSTR("bbpl MIDI Input Port"),
            &MidiInputHub::midiReadProc,
            nullptr,
            &inputPort_
        );

        if (status != noErr || inputPort_ == 0) {
            disposeCoreMIDIObjects();
            throw std::runtime_error("Failed to create CoreMIDI input port.");
        }

        connectedSources_.clear();
        connectedSources_.reserve(static_cast<size_t>(sourceCount));

        for (ItemCount index = 0; index < sourceCount; ++index) {
            MIDIEndpointRef source = MIDIGetSource(index);
            if (source == 0) {
                continue;
            }

            status = MIDIPortConnectSource(inputPort_, source, nullptr);
            if (status == noErr) {
                connectedSources_.push_back(source);
            }
        }

        if (connectedSources_.empty()) {
            disposeCoreMIDIObjects();
            throw std::runtime_error("No MIDI input device could be opened.");
        }

        running_ = true;
    }

    static void stop() {
        std::lock_guard<std::mutex> lock(mutex_);

        if (!running_) {
            return;
        }

        for (MIDIEndpointRef source : connectedSources_) {
            if (inputPort_ != 0 && source != 0) {
                MIDIPortDisconnectSource(inputPort_, source);
            }
        }

        connectedSources_.clear();
        disposeCoreMIDIObjects();
        running_ = false;
    }

    static bool isRunning() {
        std::lock_guard<std::mutex> lock(mutex_);
        return running_;
    }

    static bool hasHandlers() {
        std::lock_guard<std::mutex> lock(mutex_);
        return !handlers_.empty();
    }

private:
    struct HandlerEntry {
        int token = 0;
        MessageHandler handler;
    };

    struct PedalHandlerEntry {
        int token = 0;
        PedalHandler handler;
    };

    static void midiReadProc(
        const MIDIPacketList* packetList,
        void* readProcRefCon,
        void* srcConnRefCon
    ) {
        static_cast<void>(readProcRefCon);
        static_cast<void>(srcConnRefCon);

        if (packetList == nullptr) {
            return;
        }

        const MIDIPacket* packet = &packetList->packet[0];
        for (UInt32 packetIndex = 0; packetIndex < packetList->numPackets; ++packetIndex) {
            parsePacket(packet->data, packet->length);
            packet = MIDIPacketNext(packet);
        }
    }

    static void parsePacket(const Byte* data, UInt16 length) {
        if (data == nullptr || length == 0) {
            return;
        }

        UInt16 index = 0;
        uint8_t runningStatus = 0;

        while (index < length) {
            uint8_t status = static_cast<uint8_t>(data[index]);
            UInt16 messageStart = index;

            if ((status & 0x80) != 0) {
                ++index;

                if (status < 0xF0) {
                    runningStatus = status;
                } else {
                    runningStatus = 0;
                }
            } else {
                if (runningStatus == 0) {
                    ++index;
                    continue;
                }

                status = runningStatus;
                messageStart = index;
            }

            const UInt16 messageLength = messageLengthForStatus(status);
            if (messageLength == 0) {
                continue;
            }

            const UInt16 dataByteCount = static_cast<UInt16>(messageLength - 1);
            if (index + dataByteCount > length) {
                return;
            }

            std::array<uint8_t, 3> message {};
            message[0] = status;
            UInt16 messageSize = 1;

            for (UInt16 offset = 0; offset < dataByteCount; ++offset) {
                const uint8_t value = static_cast<uint8_t>(data[index + offset]);
                if ((value & 0x80) != 0) {
                    runningStatus = 0;
                    index = static_cast<UInt16>(index + offset);
                    break;
                }
                message[messageSize++] = value;
            }

            if (messageSize != messageLength) {
                continue;
            }

            const MidiMessage messageView(message.data(), messageSize);
            dispatchMessage(messageView);
            dispatchPedalMessage(messageView);

            if ((static_cast<uint8_t>(data[messageStart]) & 0x80) != 0) {
                index = static_cast<UInt16>(messageStart + messageLength);
            } else {
                index = static_cast<UInt16>(messageStart + dataByteCount);
            }
        }
    }

    static UInt16 messageLengthForStatus(uint8_t status) {
        const uint8_t statusType = status & 0xF0;

        switch (statusType) {
            case 0x80:
            case 0x90:
            case 0xA0:
            case 0xB0:
            case 0xE0:
                return 3;

            case 0xC0:
            case 0xD0:
                return 2;

            default:
                break;
        }

        switch (status) {
            case 0xF1:
            case 0xF3:
                return 2;

            case 0xF2:
                return 3;

            case 0xF6:
            case 0xF8:
            case 0xFA:
            case 0xFB:
            case 0xFC:
            case 0xFE:
            case 0xFF:
                return 1;

            default:
                return 0;
        }
    }

    static void dispatchMessage(MidiMessage message) {
        std::vector<MessageHandler> handlers;

        {
            std::lock_guard<std::mutex> lock(mutex_);
            handlers.reserve(handlers_.size());

            for (const HandlerEntry& entry : handlers_) {
                if (entry.handler) {
                    handlers.push_back(entry.handler);
                }
            }
        }

        for (const MessageHandler& handler : handlers) {
            handler(message);
        }
    }

    static void dispatchPedalMessage(MidiMessage message) {
        if (message.size() != 3) {
            return;
        }

        const uint8_t status = message[0];
        if ((status & 0xF0) != 0xB0) {
            return;
        }

        const uint8_t controller = message[1];
        const uint8_t rawValue = static_cast<uint8_t>(std::min<uint8_t>(message[2], 127));
        const double depth = static_cast<double>(rawValue) / 127.0;

        bool changed = false;

        {
            std::lock_guard<std::mutex> lock(mutex_);

            switch (controller) {
                case kSoftPedalController:
                    pedalState_.soft = depth;
                    changed = true;
                    break;

                case kHarmonicPedalController:
                    pedalState_.harmonic = depth;
                    changed = true;
                    break;

                case kSostenutoPedalController:
                    pedalState_.sostenuto = depth;
                    changed = true;
                    break;

                case kSustainPedalController:
                    pedalState_.sustain = depth;
                    changed = true;
                    break;

                default:
                    break;
            }
        }

        if (!changed) {
            return;
        }

        dispatchPedalState();
    }

    static void dispatchPedalState() {
        PedalState stateSnapshot;
        std::vector<PedalHandler> handlers;

        {
            std::lock_guard<std::mutex> lock(mutex_);
            stateSnapshot = pedalState_;
            handlers.reserve(pedalHandlers_.size());

            for (const PedalHandlerEntry& entry : pedalHandlers_) {
                if (entry.handler) {
                    handlers.push_back(entry.handler);
                }
            }
        }

        for (const PedalHandler& handler : handlers) {
            handler(stateSnapshot);
        }
    }

    static void disposeCoreMIDIObjects() {
        if (inputPort_ != 0) {
            MIDIPortDispose(inputPort_);
            inputPort_ = 0;
        }

        if (client_ != 0) {
            MIDIClientDispose(client_);
            client_ = 0;
        }
    }

private:
    inline static std::mutex mutex_;
    inline static bool running_ = false;
    inline static int nextToken_ = 1;
    inline static std::vector<HandlerEntry> handlers_;
    inline static std::vector<PedalHandlerEntry> pedalHandlers_;
    inline static PedalState pedalState_;

    inline static MIDIClientRef client_ = 0;
    inline static MIDIPortRef inputPort_ = 0;
    inline static std::vector<MIDIEndpointRef> connectedSources_;
};

#endif /* midiInputHub_hpp */
