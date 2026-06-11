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
#include <stdexcept>
#include <vector>
#include <algorithm>

class MidiInputHub {
public:
    using MessageHandler = std::function<void(const std::vector<uint8_t>&)>;

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
        while (index < length) {
            const uint8_t status = static_cast<uint8_t>(data[index]);

            if ((status & 0x80) == 0) {
                ++index;
                continue;
            }

            const UInt16 messageLength = messageLengthForStatus(status);
            if (messageLength == 0 || index + messageLength > length) {
                return;
            }

            std::vector<uint8_t> message(
                data + index,
                data + index + messageLength
            );

            dispatchMessage(message);
            index = static_cast<UInt16>(index + messageLength);
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

    static void dispatchMessage(const std::vector<uint8_t>& message) {
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

    inline static MIDIClientRef client_ = 0;
    inline static MIDIPortRef inputPort_ = 0;
    inline static std::vector<MIDIEndpointRef> connectedSources_;
};

#endif /* midiInputHub_hpp */
