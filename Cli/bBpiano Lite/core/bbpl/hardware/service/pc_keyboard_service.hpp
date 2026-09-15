// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
// [AI-ASSISTED, HUMAN-UNDERSTOOD CODE]
// This file may have been written with the assistance of AI for explanation,
// discussion, review, implementation guidance, or non-critical engineering suggestions.
//
// However, every line included in this file has been reviewed, understood,
// and accepted by its author. Every implementation is expected to be explainable,
// reproducible, open to inspection, and subject to criticism and revision.
//
// AI is treated as an engineering assistant rather than an authority:
// the author remains responsible for the design decisions, assumptions,
// correctness, and final implementation contained in this file.
// —————————————————————————
// [本文件包含 AI 辅助下完成的代码]
// 本文件在编写过程中可能使用 AI 进行原理讲解、讨论、代码审阅、实现指导，
// 或提供非关键性的工程建议。
//
// 但最终保留在本文件中的每一行代码，均由作者亲自审阅、理解并确认。
// 所有实现都应能够由作者解释、复现、检查，并接受批评、修改与质疑。
//
// AI 在此仅作为工程辅助工具，而非技术权威；
// 本文件中的设计选择、假设、正确性以及最终实现，均由作者本人承担责任。
//
// Ziyang Tan
// 2026-06-19
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifndef BBPL_PC_KEYBOARD_SERVICE_HPP
#define BBPL_PC_KEYBOARD_SERVICE_HPP

#include <sys/event.h>
#include <sys/time.h>
#include <termios.h>
#include <unistd.h>

#include <algorithm>
#include <array>
#include <atomic>
#include <cctype>
#include <cerrno>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <stop_token>
#include <string_view>

#include "../../piano_controller.hpp"

class PcKeyboard {
public:
    PcKeyboard() = default;
    PcKeyboard(const PcKeyboard&) = delete;
    PcKeyboard& operator=(const PcKeyboard&) = delete;
    PcKeyboard(PcKeyboard&&) = delete;
    PcKeyboard& operator=(PcKeyboard&&) = delete;

    void set_note_duration(std::chrono::milliseconds duration) noexcept {
        const auto clamped = std::clamp(duration.count(), 20LL, 2'000LL);
        note_duration_ms_.store(static_cast<int>(clamped));
    }

    // Blocking service body. kqueue sleeps until stdin, the next note-off, or a
    // stop-token EVFILT_USER wakeup; there is no periodic polling thread.
    void run(std::stop_token stop_token = {}) {
        TerminalGuard terminal;
        KqueueGuard queue;
        register_events(queue.get());

        std::stop_callback wake_on_stop(stop_token,
 [descriptor = queue.get()] {
            struct kevent event{};
            EV_SET(&event,
                   k_stop_event_id,
                   EVFILT_USER,
                   0,
                   NOTE_TRIGGER,
                   0,
                   nullptr);
            static_cast<void>(kevent(descriptor,
                                     &event,
                                     1,
                                     nullptr,
                                     0,
                                     nullptr));
        });

        print_help();
        try {
            while (!stop_token.stop_requested()) {
                release_due_notes();
                const timespec timeout = next_timeout();
                struct kevent event{};
                const int count = kevent(queue.get(),
                                         nullptr,
                                         0,
                                         &event,
                                         1,
                                         &timeout);
                if (count < 0) {
                    if (errno == EINTR) {
                        continue;
                    }
                    throw std::runtime_error("kqueue wait failed.");
                }
                if (count == 0) {
                    continue;
                }
                if (event.filter == EVFILT_USER && event.ident == k_stop_event_id) {
                    break;
                }
                if (event.filter == EVFILT_READ && event.ident == STDIN_FILENO &&
                    !read_available_input()) {
                    break;
                }
            }
        } catch (...) {
            release_all_notes();
            reset_pedals();
            throw;
        }

        release_all_notes();
        reset_pedals();
    }

private:
    using Clock = std::chrono::steady_clock;
    static constexpr uintptr_t k_stop_event_id = 1;
    static constexpr int k_lowest_base_note = 24;
    static constexpr int k_highest_note = 108;
    static constexpr int k_keys_per_bank = 10;

    class TerminalGuard {
    public:
        TerminalGuard() {
            if (!isatty(STDIN_FILENO)) {
                throw std::runtime_error(
                                         "PC keyboard requires an interactive terminal.");
            }
            if (tcgetattr(STDIN_FILENO, &original_) != 0) {
                throw std::runtime_error("Failed to read terminal mode.");
            }
            termios raw = original_;
            raw.c_lflag &= static_cast<tcflag_t>(~(ICANON | ECHO));
            raw.c_cc[VMIN] = 1;
            raw.c_cc[VTIME] = 0;
            if (tcsetattr(STDIN_FILENO, TCSANOW, &raw) != 0) {
                throw std::runtime_error("Failed to enter raw terminal mode.");
            }
            installed_ = true;
        }

        ~TerminalGuard() {
            if (installed_) {
                static_cast<void>(tcsetattr(STDIN_FILENO, TCSANOW, &original_));
            }
        }

    private:
        termios original_{};
        bool installed_ = false;
    };

    class KqueueGuard {
    public:
        KqueueGuard() : descriptor_(kqueue()) {
            if (descriptor_ < 0) {
                throw std::runtime_error("Failed to create kqueue.");
            }
        }
        ~KqueueGuard() { close(descriptor_); }
        int get() const noexcept { return descriptor_; }

    private:
        int descriptor_;
    };

    struct ActiveNote {
        int note = 0;
        Clock::time_point release_time{};
        bool active = false;
    };

    struct KeyMapping {
        int offset = -1;
        double velocity = 0.0;
    };

    static void register_events(int queue) {
        std::array<struct kevent, 2> changes{};
        EV_SET(&changes[0], STDIN_FILENO, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0,
               nullptr);
        EV_SET(&changes[1],
               k_stop_event_id,
               EVFILT_USER,
               EV_ADD | EV_CLEAR,
               0,
               0,
               nullptr);
        if (kevent(queue,
                   changes.data(),
                   static_cast<int>(changes.size()),
                   nullptr,
                   0,
                   nullptr) != 0) {
            throw std::runtime_error("Failed to register kqueue events.");
        }
    }

    bool read_available_input() {
        std::array<char, 64> input{};
        const ssize_t count = read(STDIN_FILENO, input.data(), input.size());
        if (count == 0) {
            return false;
        }
        if (count < 0) {
            return errno == EINTR || errno == EAGAIN;
        }
        for (ssize_t index = 0; index < count; ++index) {
            handle_key(input[static_cast<std::size_t>(index)]);
        }
        return true;
    }

    void handle_key(char character) {
        if (character >= '1' && character <= '9') {
            base_note_ = k_lowest_base_note +
            (static_cast<int>(character - '1') * k_keys_per_bank);
            print_line_one_status("PC keyboard base MIDI note: ", base_note_);
            return;
        }
        if (handle_pedal_key(character)) {
            return;
        }

        const KeyMapping mapping = map_key(character);
        const int note = base_note_ + mapping.offset;
        if (mapping.offset < 0 || note < 21 || note > k_highest_note) {
            return;
        }
        trigger_note(note, mapping.velocity);
    }

    void trigger_note(int note, double velocity) {
        const auto now = Clock::now();
        const auto note_duration =
            std::chrono::milliseconds(
                note_duration_ms_.load()
            );
        ActiveNote* free_slot = nullptr;
        for (ActiveNote& active_note : active_notes_) {
            if (active_note.active &&
                active_note.note == note) {
                active_note.release_time =
                    now + note_duration;
                return;
            }

            if (!active_note.active &&
                free_slot == nullptr) {
                free_slot = &active_note;
            }
        }
        if (free_slot == nullptr) {
            return;
        }

        note_on(note, velocity);

        print_line_two_status(
            "NoteOn: MIDI ",
            note,
            ", velocity ",
            velocity
        );

        free_slot->note = note;
        free_slot->release_time =
            now + note_duration;
        free_slot->active = true;
    }
    
    void release_due_notes() {
        const auto now = Clock::now();
        for (ActiveNote& active_note : active_notes_) {
            if (active_note.active && active_note.release_time <= now) {
                note_off(active_note.note, 0.0);
                active_note.active = false;
            }
        }
    }

    void release_all_notes() noexcept {
        for (ActiveNote& active_note : active_notes_) {
            if (active_note.active) {
                note_off(active_note.note, 0.0);
                active_note.active = false;
            }
        }
    }

    timespec next_timeout() const noexcept {
        auto earliest = Clock::time_point::max();
        for (const ActiveNote& active_note : active_notes_) {
            if (active_note.active) {
                earliest = std::min(earliest, active_note.release_time);
            }
        }
        if (earliest == Clock::time_point::max()) {
            // kqueue requires a pointer-or-null API. One hour keeps this value
            // finite; stop requests still wake immediately through EVFILT_USER.
            return timespec{3600, 0};
        }

        const auto remaining =
        std::max(Clock::duration::zero(), earliest - Clock::now());
        const auto nanoseconds =
        std::chrono::duration_cast<std::chrono::nanoseconds>(remaining).count();
        return timespec{static_cast<time_t>(nanoseconds / 1'000'000'000LL),
            static_cast<long>(nanoseconds % 1'000'000'000LL)};
    }

    static KeyMapping map_key(char character) noexcept {
        const char lower =
        static_cast<char>(std::tolower(static_cast<unsigned char>(character)));
        constexpr std::string_view k_top = "qwertyuiop";
        constexpr std::string_view k_middle = "asdfghjkl;";
        constexpr std::string_view k_bottom = "zxcvbnm,./";

        if (const auto index = k_top.find(lower); index != std::string_view::npos) {
            return {static_cast<int>(index), 112.0};
        }
        if (const auto index = k_middle.find(lower);
            index != std::string_view::npos) {
            return {static_cast<int>(index), 72.0};
        }
        if (const auto index = k_bottom.find(lower);
            index != std::string_view::npos) {
            return {static_cast<int>(index), 40.0};
        }
        return {};
    }

    bool handle_pedal_key(char character) {
        switch (character) {
            case '-':
                soft_pedal_down_ = !soft_pedal_down_;
                if (soft_pedal_down_) {
                    softpedal_control(1.0);
                    print_line("PedalPressed: Soft pedal (una corda) has been pressed.");
                } else {
                    softpedal_control(0.0);
                    print_line("PedalReleased: Soft pedal (una corda) has been released.");
                }
                return true;

            case '=':
                harmonic_pedal_down_ = !harmonic_pedal_down_;
                if (harmonic_pedal_down_) {
                    harmonicpedal_control(1.0);
                    print_line("PedalPressed: Harmonic pedal has been pressed.");
                } else {
                    harmonicpedal_control(0.0);
                    print_line("PedalReleased: Harmonic pedal has been released.");
                }
                return true;

            case '[':
                sostenuto_pedal_down_ = !sostenuto_pedal_down_;
                if (sostenuto_pedal_down_) {
                    sostenutopedal_control(1.0);
                    print_line("PedalPressed: Sostenuto pedal has been pressed.");
                } else {
                    sostenutopedal_control(0.0);
                    print_line("PedalReleased: Sostenuto pedal has been released.");
                }
                return true;

            case ']':
                sustain_pedal_down_ = !sustain_pedal_down_;
                if (sustain_pedal_down_) {
                    sustainpedal_control(1.0);
                    print_line("PedalPressed: Sustain pedal has been pressed.");
                } else {
                    sustainpedal_control(0.0);
                    print_line("PedalReleased: Sustain pedal has been released.");
                }
                return true;
                return true;
            default:
                return false;
        }
    }

    void reset_pedals() noexcept {
        soft_pedal_down_ = false;
        harmonic_pedal_down_ = false;
        sostenuto_pedal_down_ = false;
        sustain_pedal_down_ = false;
        softpedal_control(0.0);
        harmonicpedal_control(0.0);
        sostenutopedal_control(0.0);
        sustainpedal_control(0.0);
    }

    static void print_help() {
        std::cout
        << "PC keyboard started (Ctrl-C exits).\n"
        << "  Q-P / A-; / Z-/ : velocity 112 / 72 / 40\n"
        << "  1-9               : select pitch bank\n"
        << "  - = [ ]           : soft / harmonic / sostenuto / sustain\n";
    }
    
    inline void print_line(std::string_view message) {
        std::cout
        << "\r\033[2K"
        << message
        << std::flush;
    }
    inline void print_line_one_status(std::string_view message, double value) {
        std::cout
        << "\r\033[2K"
        << message << value
        << std::flush;
    }
    inline void print_line_two_status(std::string_view message_1, double value_1,
                                      std::string_view message_2, double value_2) {
        std::cout
        << "\r\033[2K"
        << message_1 << value_1
        << message_2 << value_2
        << std::flush;
    }

    std::atomic<int> note_duration_ms_{350};
    int base_note_ = 60;
    std::array<ActiveNote, 30> active_notes_{};
    bool soft_pedal_down_ = false;
    bool harmonic_pedal_down_ = false;
    bool sostenuto_pedal_down_ = false;
    bool sustain_pedal_down_ = false;
};

#endif
