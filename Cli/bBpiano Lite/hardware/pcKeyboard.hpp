

//
//  pcKeyboard.hpp
//  bBpiano Lite
//
//  Created by opus arc on 2026/6/7.
//

#ifndef pcKeyboard_hpp
#define pcKeyboard_hpp

#include "../core/controller.hpp"



#include "../core/piano/PianoModel.hpp"





#include <algorithm>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cctype>
#include <iostream>
#include <mutex>
#include <stdexcept>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

#include <termios.h>
#include <unistd.h>

class PcKeyboard {
public:
    static PcKeyboard& shared() {
        static PcKeyboard instance;
        return instance;
    }

    PcKeyboard(const PcKeyboard&) = delete;
    PcKeyboard& operator=(const PcKeyboard&) = delete;

    void start() {
        bool expected = false;
        if (!running_.compare_exchange_strong(expected, true)) {
            return;
        }

        installRawMode();

        inputThread_ = std::thread([this]() {
            inputLoop();
        });
        
        std::lock_guard<std::mutex> lock(mutex_);
        stopped_ = false;
        
        printHelp();
    }

    void stop() {
        bool expected = true;
        if (!running_.compare_exchange_strong(expected, false)) {
            return;
        }

        restoreTerminalMode();

        if (inputThread_.joinable()) {
            if (inputThread_.get_id() == std::this_thread::get_id()) {
                inputThread_.detach();
            } else {
                inputThread_.join();
            }
        }

        {
            std::lock_guard<std::mutex> lock(mutex_);
            stopped_ = true;
        }
        condition_.notify_all();
    }

    void waitUntilStopped() {
        std::unique_lock<std::mutex> lock(mutex_);
        condition_.wait(lock, [this]() {
            return stopped_;
        });
    }

    bool isRunning() const {
        return running_.load();
    }

    void setNoteDurationMs(int durationMs) {
        noteDurationMs_ = std::clamp(durationMs, 20, 2000);
    }

    int noteDurationMs() const {
        return noteDurationMs_;
    }

private:
    PcKeyboard() = default;

    ~PcKeyboard() {
        try {
            stop();
        } catch (...) {
            restoreTerminalMode();
        }
    }

    void installRawMode() {
        if (!isatty(STDIN_FILENO)) {
            throw std::runtime_error("PcKeyboard requires an interactive terminal.");
        }

        if (tcgetattr(STDIN_FILENO, &originalTerminalMode_) != 0) {
            throw std::runtime_error("Failed to read terminal mode.");
        }

        terminalModeInstalled_ = true;

        termios raw = originalTerminalMode_;
        raw.c_lflag &= static_cast<unsigned long>(~(ICANON | ECHO));
        raw.c_cc[VMIN] = 1;
        raw.c_cc[VTIME] = 0;

        if (tcsetattr(STDIN_FILENO, TCSANOW, &raw) != 0) {
            terminalModeInstalled_ = false;
            throw std::runtime_error("Failed to enter raw terminal mode.");
        }
    }

    void restoreTerminalMode() {
        if (terminalModeInstalled_) {
            tcsetattr(STDIN_FILENO, TCSANOW, &originalTerminalMode_);
            terminalModeInstalled_ = false;
        }
    }

    void inputLoop() {
        while (running_.load()) {
            char ch = 0;
            const ssize_t readCount = read(STDIN_FILENO, &ch, 1);

            if (readCount <= 0) {
                continue;
            }

            handleKey(ch);
        }
    }

    void handleKey(char ch) {
        if (ch == 27) { // ESC
            stop();
            return;
        }

        if (ch >= '1' && ch <= '8') {
            setOctave(static_cast<int>(ch - '0'));
            return;
        }

        const KeyMapping mapping = mapKey(ch);
        if (!mapping.valid) {
            return;
        }

        const int note = std::clamp(baseNote_ + mapping.offset, 0, 127);
        const double velocity = mapping.velocity;
        const int durationMs = noteDurationMs_.load();

        std::thread([note, velocity, durationMs]() {
            note_on(note, velocity);
            std::this_thread::sleep_for(std::chrono::milliseconds(durationMs));
            note_off(note, velocity);
        }).detach();
    }

    void setOctave(int octave) {
        octave = std::clamp(octave, 1, 8);
        baseNote_ = 12 * (octave + 1);

        std::cout << "\rPcKeyboard octave: " << octave
                  << "  base MIDI note: " << baseNote_.load()
                  << "        " << std::flush;
    }

    struct KeyMapping {
        bool valid = false;
        int offset = 0;
        double velocity = 0.0;
    };

    KeyMapping mapKey(char ch) const {
        const char lower = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));

        static const std::string topRow = "qwertyuiop";
        static const std::string middleRow = "asdfghjkl;";
        static const std::string bottomRow = "zxcvbnm,./";

        const size_t topIndex = topRow.find(lower);
        if (topIndex != std::string::npos) {
            return KeyMapping{true, static_cast<int>(topIndex), 1.0};
        }

        const size_t middleIndex = middleRow.find(lower);
        if (middleIndex != std::string::npos) {
            return KeyMapping{true, static_cast<int>(middleIndex), 0.6};
        }

        const size_t bottomIndex = bottomRow.find(lower);
        if (bottomIndex != std::string::npos) {
            return KeyMapping{true, static_cast<int>(bottomIndex), 0.3};
        }

        return KeyMapping{};
    }

    void printHelp() const {
        std::cout
            << "PcKeyboard started.\n"
            << "  Q W E R T Y U I O P : strong velocity\n"
            << "  A S D F G H J K L ; : medium velocity\n"
            << "  Z X C V B N M , . / : soft velocity\n"
            << "  1-8                 : switch octave\n"
            << "  ESC                 : stop\n";
    }

private:
    std::atomic<bool> running_ { false };
    std::atomic<int> baseNote_ { 60 };
    std::atomic<int> noteDurationMs_ { 100 };

    mutable std::mutex mutex_;
    std::condition_variable condition_;
    bool stopped_ = false;

    std::thread inputThread_;

    termios originalTerminalMode_ {};
    bool terminalModeInstalled_ = false;
};

#endif /* pcKeyboard_hpp */
