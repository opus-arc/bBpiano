//
//  midiKeyboard.hpp
//  bBpiano Lite
//
//  Created by opus arc on 2026/6/7.
//

#ifndef midiKeyboard_hpp
#define midiKeyboard_hpp

#include "../core/controller.hpp"
#include "./midiInputHub.hpp"
#include "midiInputHub.hpp"

#include <cstdint>
#include <condition_variable>
#include <mutex>
#include <stdexcept>

class MidiKeyboard {
public:
    MidiKeyboard() = delete;
    MidiKeyboard(const MidiKeyboard&) = delete;
    MidiKeyboard& operator=(const MidiKeyboard&) = delete;

    static void start() {
        if (isRunning()) {
            return;
        }

        MidiInputHub::start();

        handlerToken_ = MidiInputHub::addHandler([](const std::vector<uint8_t>& message) {
            handleMessage(message);
        });

        {
            std::lock_guard<std::mutex> lock(mutex_);
            running_ = true;
        }
    }

    static void stop() {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            if (!running_) {
                return;
            }
        }

        all_silence();

        if (handlerToken_ != 0) {
            MidiInputHub::removeHandler(handlerToken_);
            handlerToken_ = 0;
        }

        if (!MidiInputHub::hasHandlers()) {
            MidiInputHub::stop();
        }

        {
            std::lock_guard<std::mutex> lock(mutex_);
            running_ = false;
        }
        condition_.notify_all();
    }

    static void waitUntilStopped() {
        std::unique_lock<std::mutex> lock(mutex_);
        condition_.wait(lock, []() {
            return !running_;
        });
    }

    static bool isRunning() {
        std::lock_guard<std::mutex> lock(mutex_);
        return running_;
    }

private:
    static void handleMessage(const std::vector<uint8_t>& message) {
        if (message.empty()) {
            return;
        }

        const uint8_t status = message[0];
        const uint8_t statusType = status & 0xF0;

        switch (statusType) {
            case 0x80: {
                if (message.size() < 3) {
                    return;
                }

                const int note = static_cast<int>(message[1]);
                const double velocity = static_cast<double>(message[2]);
                note_off(note, velocity);
                break;
            }

            case 0x90: {
                if (message.size() < 3) {
                    return;
                }

                const int note = static_cast<int>(message[1]);
                const double velocity = static_cast<double>(message[2]);

                if (velocity > 0.0) {
                    note_on(note, velocity);
                } else {
                    note_off(note, velocity);
                }

                break;
            }

            case 0xA0: {
                if (message.size() < 3) {
                    return;
                }

                const int note = static_cast<int>(message[1]);
                const double pressure = static_cast<double>(message[2]);
                note_afterTouch(note, pressure);
                break;
            }

            default:
                break;
        }
    }

private:
    inline static std::mutex mutex_;
    inline static std::condition_variable condition_;
    inline static bool running_ = false;

    inline static int handlerToken_ = 0;
};

#endif /* midiKeyboard_hpp */
