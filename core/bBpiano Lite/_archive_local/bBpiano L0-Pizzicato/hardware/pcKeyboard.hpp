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
        resetPedals();

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
    static constexpr int kMinimumKeyboardOctave = 1;
    static constexpr int kMaximumKeyboardOctave = 9;
    static constexpr int kKeyboardPitchCountPerOctaveSlot = 10;
    static constexpr int kKeyboardLowestBaseNote = 24;
    static constexpr int kMaximumPlayableMidiNote = 108;

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

        if (ch >= '1' && ch <= '9') {
            setOctave(static_cast<int>(ch - '0'));
            return;
        }

        if (handlePedalKey(ch)) {
            return;
        }

        const KeyMapping mapping = mapKey(ch);
        if (!mapping.valid) {
            return;
        }

        const int note = baseNote_ + mapping.offset;
        if (note > kMaximumPlayableMidiNote) {
            return;
        }

        const double velocity = mapping.velocity;
        const int durationMs = noteDurationMs_.load();

        std::thread([note, velocity, durationMs]() {
            note_on(note, velocity);
            std::this_thread::sleep_for(std::chrono::milliseconds(durationMs));
            note_off(note, velocity);
        }).detach();
    }

    bool handlePedalKey(char ch) {
        switch (ch) {
            case '-':
                toggleSoftPedal();
                return true;

            case '=':
                toggleHarmonicPedal();
                return true;

            case '[':
                toggleSostenutoPedal();
                return true;

            case ']':
                toggleSustainPedal();
                return true;

            default:
                return false;
        }
    }

    void toggleSoftPedal() {
        const bool enabled = !softPedalDown_.load();
        softPedalDown_ = enabled;
        softPedal_control(enabled ? 1.0 : 0.0);
        printPedalState("soft", enabled);
    }

    void toggleHarmonicPedal() {
        const bool enabled = !harmonicPedalDown_.load();
        harmonicPedalDown_ = enabled;
        harmonicPedal_control(enabled ? 1.0 : 0.0);
        printPedalState("harmonic", enabled);
    }

    void toggleSostenutoPedal() {
        const bool enabled = !sostenutoPedalDown_.load();
        sostenutoPedalDown_ = enabled;
        sostenutoPedal_control(enabled ? 1.0 : 0.0);
        printPedalState("sostenuto", enabled);
    }

    void toggleSustainPedal() {
        const bool enabled = !sustainPedalDown_.load();
        sustainPedalDown_ = enabled;
        sustainPedal_control(enabled ? 1.0 : 0.0);
        printPedalState("sustain", enabled);
    }

    void resetPedals() {
        softPedalDown_ = false;
        harmonicPedalDown_ = false;
        sostenutoPedalDown_ = false;
        sustainPedalDown_ = false;

        softPedal_control(0.0);
        harmonicPedal_control(0.0);
        sostenutoPedal_control(0.0);
        sustainPedal_control(0.0);
    }

    void printPedalState(const char* name, bool enabled) const {
//        std::cout << "\rPcKeyboard " << name << " pedal: "
//                  << (enabled ? "down" : "up")
//                  << "        " << std::flush;
    }

    void setOctave(int octave) {
        octave = std::clamp(octave, kMinimumKeyboardOctave, kMaximumKeyboardOctave);
        baseNote_ = kKeyboardLowestBaseNote
                  + (octave - kMinimumKeyboardOctave) * kKeyboardPitchCountPerOctaveSlot;

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
            return KeyMapping{true, static_cast<int>(topIndex), 112.0};
        }

        const size_t middleIndex = middleRow.find(lower);
        if (middleIndex != std::string::npos) {
            return KeyMapping{true, static_cast<int>(middleIndex), 72.0};
        }

        const size_t bottomIndex = bottomRow.find(lower);
        if (bottomIndex != std::string::npos) {
            return KeyMapping{true, static_cast<int>(bottomIndex), 40.0};
        }

        return KeyMapping{};
    }

    void printHelp() const {
        std::cout
            << "PcKeyboard started.\n"
            << "  Q W E R T Y U I O P : strong velocity 112/127\n"
            << "  A S D F G H J K L ; : medium velocity 72/127\n"
            << "  Z X C V B N M , . / : soft velocity 40/127\n"
            << "  1-9                 : switch keyboard range, continuous by Q-P pitch span\n"
            << "  - = [ ]             : soft / harmonic / sostenuto / sustain pedal toggle\n"
            << "  ESC                 : stop\n";
    }

private:
    std::atomic<bool> running_ { false };
    std::atomic<int> baseNote_ { 60 };
    std::atomic<int> noteDurationMs_ { 650 };

    std::atomic<bool> softPedalDown_ { false };
    std::atomic<bool> harmonicPedalDown_ { false };
    std::atomic<bool> sostenutoPedalDown_ { false };
    std::atomic<bool> sustainPedalDown_ { false };

    mutable std::mutex mutex_;
    std::condition_variable condition_;
    bool stopped_ = false;

    std::thread inputThread_;

    termios originalTerminalMode_ {};
    bool terminalModeInstalled_ = false;
};

#endif /* pcKeyboard_hpp */
