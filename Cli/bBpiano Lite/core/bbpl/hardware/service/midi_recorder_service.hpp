#ifndef BBPL_MIDI_RECORDER_SERVICE_HPP
#define BBPL_MIDI_RECORDER_SERVICE_HPP

#include <algorithm>
#include <array>
#include <chrono>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <limits>
#include <mutex>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "midi_inputhub_service.hpp"

class MidiRecorder {
 public:
  struct Event {
    std::uint64_t time_microseconds = 0;
    std::array<std::uint8_t, 3> message{};
    std::uint8_t message_size = 0;
  };

  explicit MidiRecorder(MidiInputHub& input_hub) noexcept
      : input_hub_(input_hub) {}

  MidiRecorder(const MidiRecorder&) = delete;
  MidiRecorder& operator=(const MidiRecorder&) = delete;
  MidiRecorder(MidiRecorder&&) = delete;
  MidiRecorder& operator=(MidiRecorder&&) = delete;

  ~MidiRecorder() { cancel(); }

  // Empty output_path selects a deterministic timestamped name in the current
  // working directory. MidiInputHub lifecycle remains coordinator-owned.
  void start(std::filesystem::path output_path = {}) {
    std::lock_guard lifecycle_lock(lifecycle_mutex_);
    if (subscription_) {
      return;
    }

    {
      std::lock_guard event_lock(event_mutex_);
      events_.clear();
      events_.reserve(4096);
      output_path_ = std::move(output_path);
      start_time_ = Clock::now();
    }

    subscription_.emplace(
        input_hub_.subscribe([this](MidiInputHub::MidiMessage message) {
          record_message(message);
        }));
  }

  // Stops capture first, then writes on the coordinator thread. No file I/O
  // happens in the CoreMIDI callback.
  std::filesystem::path stop() {
    std::lock_guard lifecycle_lock(lifecycle_mutex_);
    const bool was_recording = subscription_.has_value();
    if (subscription_) {
      subscription_->reset();
      subscription_.reset();
    }

    std::vector<Event> snapshot;
    std::filesystem::path requested_path;
    {
      std::lock_guard event_lock(event_mutex_);
      snapshot = events_;
      requested_path = output_path_;
    }
    if (!was_recording && snapshot.empty()) {
      return last_output_path_;
    }

    const std::filesystem::path final_path =
        requested_path.empty() ? std::filesystem::current_path() /
                                     build_default_file_name(snapshot)
                               : requested_path;
    write_midi_file(final_path, snapshot);

    {
      std::lock_guard event_lock(event_mutex_);
      events_.clear();
      output_path_.clear();
      last_output_path_ = final_path;
    }
    return final_path;
  }

  // Cancels without creating a file; used only for rollback/destruction.
  void cancel() noexcept {
    std::lock_guard lifecycle_lock(lifecycle_mutex_);
    if (subscription_) {
      subscription_->reset();
      subscription_.reset();
    }
    std::lock_guard event_lock(event_mutex_);
    events_.clear();
    output_path_.clear();
  }

  bool is_recording() const noexcept {
    std::lock_guard lock(lifecycle_mutex_);
    return subscription_.has_value();
  }

 private:
  using Clock = std::chrono::steady_clock;
  static constexpr std::uint16_t k_ticks_per_quarter = 480;
  static constexpr std::uint32_t k_microseconds_per_quarter = 500'000;

  void record_message(MidiInputHub::MidiMessage message) {
    if (!is_supported_message(message)) {
      return;
    }

    Event event;
    event.time_microseconds = static_cast<std::uint64_t>(
        std::chrono::duration_cast<std::chrono::microseconds>(Clock::now() -
                                                              start_time_)
            .count());
    event.message_size = static_cast<std::uint8_t>(message.size());
    std::copy(message.begin(), message.end(), event.message.begin());

    std::lock_guard lock(event_mutex_);
    events_.push_back(std::move(event));
  }

  static bool is_supported_message(MidiInputHub::MidiMessage message) noexcept {
    if (message.empty() || message[0] >= 0xF0) {
      return false;
    }
    switch (message[0] & 0xF0) {
      case 0x80:
      case 0x90:
      case 0xA0:
      case 0xB0:
      case 0xE0:
        return message.size() == 3;
      case 0xC0:
      case 0xD0:
        return message.size() == 2;
      default:
        return false;
    }
  }

  static std::string build_default_file_name(const std::vector<Event>& events) {
    const std::time_t now = std::time(nullptr);
    std::tm local_time{};
    localtime_r(&now, &local_time);

    std::size_t note_count = 0;
    std::size_t pedal_count = 0;
    for (const Event& event : events) {
      if (event.message_size < 3) {
        continue;
      }
      const std::uint8_t type = event.message[0] & 0xF0;
      if (type == 0x90 && event.message[2] != 0) {
        ++note_count;
      } else if (type == 0xB0 && is_pedal_controller(event.message[1])) {
        ++pedal_count;
      }
    }
    const std::uint64_t seconds =
        events.empty() ? 0 : events.back().time_microseconds / 1'000'000ULL;

    std::ostringstream name;
    name << std::put_time(&local_time, "%Y-%m-%d_%H-%M-%S") << "_" << note_count
         << "-notes_" << pedal_count << "-pedals_" << seconds << "-seconds.mid";
    return name.str();
  }

  static bool is_pedal_controller(std::uint8_t controller) noexcept {
    return controller == MidiInputHub::k_sustain_pedal_controller ||
           controller == MidiInputHub::k_sostenuto_pedal_controller ||
           controller == MidiInputHub::k_soft_pedal_controller ||
           controller == MidiInputHub::k_harmonic_pedal_controller;
  }

  static void append_variable_length(std::vector<std::uint8_t>& output,
                                     std::uint32_t value) {
    std::uint8_t bytes[5]{};
    int index = 4;
    bytes[index] = static_cast<std::uint8_t>(value & 0x7F);
    while ((value >>= 7) != 0 && index > 0) {
      bytes[--index] = static_cast<std::uint8_t>((value & 0x7F) | 0x80);
    }
    for (; index < 5; ++index) {
      output.push_back(bytes[index]);
    }
  }

  static std::uint32_t time_to_ticks(std::uint64_t microseconds) noexcept {
    const long double ticks = static_cast<long double>(microseconds) *
                              k_ticks_per_quarter / k_microseconds_per_quarter;
    return ticks >= std::numeric_limits<std::uint32_t>::max()
               ? std::numeric_limits<std::uint32_t>::max()
               : static_cast<std::uint32_t>(ticks);
  }

  static void append_pedal_releases(std::vector<std::uint8_t>& track) {
    const std::uint8_t controllers[] = {
        MidiInputHub::k_sustain_pedal_controller,
        MidiInputHub::k_sostenuto_pedal_controller,
        MidiInputHub::k_soft_pedal_controller,
        MidiInputHub::k_harmonic_pedal_controller};
    for (std::uint8_t controller : controllers) {
      append_variable_length(track, 0);
      track.insert(track.end(), {0xB0, controller, 0});
    }
  }

  static void write_uint16_be(std::ostream& output, std::uint16_t value) {
    output.put(static_cast<char>((value >> 8) & 0xFF));
    output.put(static_cast<char>(value & 0xFF));
  }

  static void write_uint32_be(std::ostream& output, std::uint32_t value) {
    output.put(static_cast<char>((value >> 24) & 0xFF));
    output.put(static_cast<char>((value >> 16) & 0xFF));
    output.put(static_cast<char>((value >> 8) & 0xFF));
    output.put(static_cast<char>(value & 0xFF));
  }

  static void write_midi_file(const std::filesystem::path& output_path,
                              std::vector<Event> events) {
    std::stable_sort(events.begin(), events.end(),
                     [](const Event& left, const Event& right) {
                       return left.time_microseconds < right.time_microseconds;
                     });

    std::vector<std::uint8_t> track;
    track.reserve(events.size() * 4 + 32);
    track.insert(track.end(), {0x00, 0xFF, 0x51, 0x03, 0x07, 0xA1, 0x20});
    std::uint32_t previous_tick = 0;
    for (const Event& event : events) {
      const std::uint32_t tick = time_to_ticks(event.time_microseconds);
      append_variable_length(track, tick - previous_tick);
      previous_tick = tick;
      track.insert(track.end(), event.message.begin(),
                   event.message.begin() + event.message_size);
    }
    append_pedal_releases(track);
    track.insert(track.end(), {0x00, 0xFF, 0x2F, 0x00});

    if (track.size() > std::numeric_limits<std::uint32_t>::max()) {
      throw std::runtime_error("Recorded MIDI track is too large.");
    }
    if (!output_path.parent_path().empty() &&
        !std::filesystem::exists(output_path.parent_path())) {
      throw std::runtime_error("MIDI output directory does not exist: " +
                               output_path.parent_path().string());
    }
    if (std::filesystem::exists(output_path)) {
      throw std::runtime_error("Refusing to overwrite existing MIDI file: " +
                               output_path.string());
    }

    std::filesystem::path temporary_path = output_path;
    temporary_path += ".bbpl-part";
    if (std::filesystem::exists(temporary_path)) {
      throw std::runtime_error("Temporary MIDI output already exists: " +
                               temporary_path.string());
    }

    try {
      std::ofstream output(temporary_path, std::ios::binary);
      if (!output) {
        throw std::runtime_error("Cannot create MIDI output: " +
                                 output_path.string());
      }
      output.write("MThd", 4);
      write_uint32_be(output, 6);
      write_uint16_be(output, 0);
      write_uint16_be(output, 1);
      write_uint16_be(output, k_ticks_per_quarter);
      output.write("MTrk", 4);
      write_uint32_be(output, static_cast<std::uint32_t>(track.size()));
      output.write(reinterpret_cast<const char*>(track.data()),
                   static_cast<std::streamsize>(track.size()));
      output.close();
      if (!output) {
        throw std::runtime_error("Failed while writing MIDI output: " +
                                 output_path.string());
      }
      std::filesystem::rename(temporary_path, output_path);
    } catch (...) {
      std::error_code ignored;
      std::filesystem::remove(temporary_path, ignored);
      throw;
    }
  }

  MidiInputHub& input_hub_;
  mutable std::mutex lifecycle_mutex_;
  mutable std::mutex event_mutex_;
  std::optional<MidiInputHub::Subscription> subscription_;
  std::vector<Event> events_;
  std::filesystem::path output_path_;
  std::filesystem::path last_output_path_;
  Clock::time_point start_time_{};
};

#endif
