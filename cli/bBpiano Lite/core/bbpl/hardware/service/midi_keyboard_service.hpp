#ifndef BBPL_MIDI_KEYBOARD_SERVICE_HPP
#define BBPL_MIDI_KEYBOARD_SERVICE_HPP

#include <algorithm>
#include <cstdint>
#include <optional>

#include "../../piano_controller.hpp"
#include "midi_inputhub_service.hpp"

class MidiKeyboard {
 public:
  explicit MidiKeyboard(MidiInputHub& input_hub) noexcept
      : input_hub_(input_hub) {}

  MidiKeyboard(const MidiKeyboard&) = delete;
//  MidiKeyboard& operator=(const MidiKeyboard&) = delete;
  MidiKeyboard(MidiKeyboard&&) = delete;
  MidiKeyboard& operator=(MidiKeyboard&&) = delete;

  ~MidiKeyboard() { stop(); }

  // MidiInputHub is owned and started/stopped by the service coordinator.
  void start() {
    if (subscription_) {
      return;
    }
    subscription_.emplace(input_hub_.subscribe(
        [](MidiInputHub::MidiMessage message) { handle_message(message); }));
  }

  void stop() noexcept {
    if (!subscription_) {
      return;
    }
    subscription_
        ->reset();  // Synchronizes with an in-flight CoreMIDI callback.
    subscription_.reset();
    reset_engine_state();
  }

  bool is_running() const noexcept { return subscription_.has_value(); }

 private:
  static bool is_playable_note(std::uint8_t note) noexcept {
    return note >= 21 && note <= 108;
  }

  static void handle_message(MidiInputHub::MidiMessage message) {
    if (message.empty()) {
      return;
    }

    const std::uint8_t type = message[0] & 0xF0;
    if (type == 0x80 || type == 0x90 || type == 0xA0) {
      if (message.size() < 3 || !is_playable_note(message[1])) {
        return;
      }

      const int note = static_cast<int>(message[1]);
      const double raw_value = static_cast<double>(message[2]);
      if (type == 0x80 || (type == 0x90 && message[2] == 0)) {
        note_off(note, raw_value);
      } else if (type == 0x90) {
        note_on(note, raw_value);
      } else {
        note_aftertouch(note, std::clamp(raw_value / 127.0, 0.0, 1.0));
      }
      return;
    }

    if (type != 0xB0 || message.size() < 3) {
      return;
    }

    const double depth =
        std::clamp(static_cast<double>(message[2]) / 127.0, 0.0, 1.0);
    switch (message[1]) {
      case MidiInputHub::k_sustain_pedal_controller:
        sustainpedal_control(depth);
        break;
      case MidiInputHub::k_sostenuto_pedal_controller:
        sostenutopedal_control(depth);
        break;
      case MidiInputHub::k_soft_pedal_controller:
        softpedal_control(depth);
        break;
      case MidiInputHub::k_harmonic_pedal_controller:
        harmonicpedal_control(depth);
        break;
      default:
        break;
    }
  }

  static void reset_engine_state() noexcept {
    // These controller calls must enqueue commands; see integration notes.
    softpedal_control(0.0);
    harmonicpedal_control(0.0);
    sostenutopedal_control(0.0);
    sustainpedal_control(0.0);
    all_silence();
  }

  MidiInputHub& input_hub_;
  std::optional<MidiInputHub::Subscription> subscription_;
};

#endif
