#ifndef BBPL_MIDI_SERVICE_HPP
#define BBPL_MIDI_SERVICE_HPP

#include <algorithm>
#include <chrono>
#include <condition_variable>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <limits>
#include <mutex>
#include <stdexcept>
#include <stop_token>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

#include "../../piano_controller.hpp"

class MidiParseError : public std::runtime_error {
 public:
  using std::runtime_error::runtime_error;
};

class MidiService {
 public:
  struct MidiEvent {
    std::uint64_t tick = 0;
    double time_in_seconds = 0.0;
    std::uint8_t status = 0;
    std::uint8_t data1 = 0;
    std::uint8_t data2 = 0;
    std::string track_name;

    int channel() const noexcept { return static_cast<int>(status & 0x0F); }
    std::uint8_t status_type() const noexcept { return status & 0xF0; }
    bool is_note_on() const noexcept {
      return status_type() == 0x90 && data2 != 0;
    }
    bool is_note_off() const noexcept {
      return status_type() == 0x80 || (status_type() == 0x90 && data2 == 0);
    }
    bool is_poly_aftertouch() const noexcept { return status_type() == 0xA0; }
    bool is_control_change() const noexcept { return status_type() == 0xB0; }
    bool is_pedal_control_change() const noexcept {
      if (!is_control_change()) {
        return false;
      }
      return data1 == 64 || data1 == 66 || data1 == 67 || data1 == 68;
    }
    double normalized_data2() const noexcept {
      return std::clamp(static_cast<double>(data2) / 127.0, 0.0, 1.0);
    }
  };

  enum class Result { completed, cancelled };

  MidiService() = delete;

  static std::vector<MidiEvent> load_events(const std::string& midi_file_path) {
    auto events =
        select_piano_events(parse_midi_file(read_whole_file(midi_file_path)));
    std::stable_sort(events.begin(), events.end(),
                     [](const MidiEvent& left, const MidiEvent& right) {
                       return left.time_in_seconds < right.time_in_seconds;
                     });
    return events;
  }

  // Synchronous by design. The process/service coordinator owns the thread;
  // stop_token is the only cancellation path.
  static Result play(double playback_rate, double start_time,
                     const std::string& midi_file_path,
                     std::stop_token stop_token = {}) {
    if (!(playback_rate > 0.0)) {
      throw std::invalid_argument("playback_rate must be greater than zero.");
    }

    const double safe_start_time = std::max(0.0, start_time);
    std::vector<MidiEvent> events = load_events(midi_file_path);
    const auto first =
        std::lower_bound(events.begin(), events.end(), safe_start_time,
                         [](const MidiEvent& event, double time) {
                           return event.time_in_seconds < time;
                         });

    std::unordered_set<int> active_notes;
    std::mutex wait_mutex;
    std::condition_variable wait_condition;
    std::stop_callback wake_on_stop(
        stop_token, [&wait_condition] { wait_condition.notify_all(); });
    const auto wall_start = std::chrono::steady_clock::now();

    auto cleanup = [&active_notes] {
      for (int note : active_notes) {
        note_off(note, 0.0);
      }
      reset_pedals();
    };

    try {
      for (auto event = first; event != events.end(); ++event) {
        if (stop_token.stop_requested()) {
          cleanup();
          return Result::cancelled;
        }

        const double offset_seconds =
            std::max(0.0, event->time_in_seconds - safe_start_time) /
            playback_rate;
        const auto deadline =
            wall_start +
            std::chrono::duration_cast<std::chrono::steady_clock::duration>(
                std::chrono::duration<double>(offset_seconds));

        {
          std::unique_lock lock(wait_mutex);
          wait_condition.wait_until(lock, deadline, [&stop_token] {
            return stop_token.stop_requested();
          });
        }
        if (stop_token.stop_requested()) {
          cleanup();
          return Result::cancelled;
        }

        dispatch_event(*event);
        if (event->is_note_on() && is_playable_note(event->data1)) {
          active_notes.insert(static_cast<int>(event->data1));
        } else if (event->is_note_off()) {
          active_notes.erase(static_cast<int>(event->data1));
        }
      }
    } catch (...) {
      cleanup();
      throw;
    }

    cleanup();
    return Result::completed;
  }

  static void dispatch_event(const MidiEvent& event) {
    const int note = static_cast<int>(event.data1);
    const double raw_value = static_cast<double>(event.data2);

    if (event.is_note_on()) {
      if (is_playable_note(event.data1)) {
        note_on(note, raw_value);
      }
    } else if (event.is_note_off()) {
      if (is_playable_note(event.data1)) {
        note_off(note, raw_value);
      }
    } else if (event.is_poly_aftertouch()) {
      if (is_playable_note(event.data1)) {
        note_aftertouch(note, event.normalized_data2());
      }
    } else if (event.is_pedal_control_change()) {
      dispatch_pedal_event(event.data1, event.normalized_data2());
    }
  }

  static void reset_pedals() {
    softpedal_control(0.0);
    harmonicpedal_control(0.0);
    sostenutopedal_control(0.0);
    sustainpedal_control(0.0);
  }

 private:
  struct TempoChange {
    std::uint64_t tick = 0;
    double microseconds_per_quarter = 500'000.0;
    std::size_t order = 0;
  };

  class Reader {
   public:
    explicit Reader(std::vector<std::uint8_t> bytes)
        : bytes_(std::move(bytes)) {}

    std::size_t position() const noexcept { return position_; }
    std::size_t size() const noexcept { return bytes_.size(); }

    void set_position(std::size_t position) {
      if (position > bytes_.size()) {
        throw MidiParseError("MIDI chunk exceeds file size.");
      }
      position_ = position;
    }

    std::uint8_t read_byte(
        std::size_t limit = std::numeric_limits<std::size_t>::max()) {
      if (position_ >= bytes_.size() || position_ >= limit) {
        throw MidiParseError("Unexpected end of MIDI data.");
      }
      return bytes_[position_++];
    }

    std::uint16_t read_uint16(
        std::size_t limit = std::numeric_limits<std::size_t>::max()) {
      const std::uint16_t high = read_byte(limit);
      const std::uint16_t low = read_byte(limit);
      return static_cast<std::uint16_t>((high << 8) | low);
    }

    std::uint32_t read_uint32(
        std::size_t limit = std::numeric_limits<std::size_t>::max()) {
      const std::uint32_t b0 = read_byte(limit);
      const std::uint32_t b1 = read_byte(limit);
      const std::uint32_t b2 = read_byte(limit);
      const std::uint32_t b3 = read_byte(limit);
      return (b0 << 24) | (b1 << 16) | (b2 << 8) | b3;
    }

    std::uint32_t read_variable_length(std::size_t limit) {
      std::uint32_t value = 0;
      for (int count = 0; count < 4; ++count) {
        const std::uint8_t byte = read_byte(limit);
        value = (value << 7) | static_cast<std::uint32_t>(byte & 0x7F);
        if ((byte & 0x80) == 0) {
          return value;
        }
      }
      throw MidiParseError("Invalid MIDI variable-length value.");
    }

    std::string read_string(
        std::size_t count,
        std::size_t limit = std::numeric_limits<std::size_t>::max()) {
      require(count, limit);
      const auto begin =
          bytes_.begin() + static_cast<std::ptrdiff_t>(position_);
      position_ += count;
      return std::string(begin, begin + static_cast<std::ptrdiff_t>(count));
    }

    std::vector<std::uint8_t> read_data(std::size_t count, std::size_t limit) {
      require(count, limit);
      const auto begin =
          bytes_.begin() + static_cast<std::ptrdiff_t>(position_);
      position_ += count;
      return {begin, begin + static_cast<std::ptrdiff_t>(count)};
    }

    void skip(std::size_t count,
              std::size_t limit = std::numeric_limits<std::size_t>::max()) {
      require(count, limit);
      position_ += count;
    }

   private:
    void require(std::size_t count, std::size_t limit) const {
      const std::size_t effective_limit = std::min(limit, bytes_.size());
      if (position_ > effective_limit || count > effective_limit - position_) {
        throw MidiParseError("Unexpected end of MIDI data.");
      }
    }

    std::vector<std::uint8_t> bytes_;
    std::size_t position_ = 0;
  };

  static bool is_playable_note(std::uint8_t note) noexcept {
    return note >= 21 && note <= 108;
  }

  static void dispatch_pedal_event(std::uint8_t controller, double depth) {
    switch (controller) {
      case 64:
        sustainpedal_control(depth);
        break;
      case 66:
        sostenutopedal_control(depth);
        break;
      case 67:
        softpedal_control(depth);
        break;
      case 68:
        harmonicpedal_control(depth);
        break;
      default:
        break;
    }
  }

  static std::vector<std::uint8_t> read_whole_file(const std::string& path) {
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file) {
      throw MidiParseError("Cannot open MIDI file: " + path);
    }
    const std::streampos end = file.tellg();
    if (end <= 0) {
      throw MidiParseError("MIDI file is empty: " + path);
    }
    if (static_cast<std::uintmax_t>(end) >
        std::numeric_limits<std::size_t>::max()) {
      throw MidiParseError("MIDI file is too large.");
    }

    std::vector<std::uint8_t> bytes(static_cast<std::size_t>(end));
    file.seekg(0, std::ios::beg);
    file.read(reinterpret_cast<char*>(bytes.data()),
              static_cast<std::streamsize>(bytes.size()));
    if (!file) {
      throw MidiParseError("Failed to read MIDI file: " + path);
    }
    return bytes;
  }

  static std::vector<MidiEvent> parse_midi_file(
      std::vector<std::uint8_t> bytes) {
    Reader reader(std::move(bytes));
    if (reader.read_string(4) != "MThd") {
      throw MidiParseError("Missing MIDI header chunk.");
    }

    const std::uint32_t header_length = reader.read_uint32();
    if (header_length < 6) {
      throw MidiParseError("Invalid MIDI header length.");
    }
    const std::size_t header_end = reader.position() + header_length;
    if (header_end < reader.position() || header_end > reader.size()) {
      throw MidiParseError("MIDI header exceeds file size.");
    }

    const std::uint16_t format = reader.read_uint16(header_end);
    const std::uint16_t track_count = reader.read_uint16(header_end);
    const std::uint16_t division = reader.read_uint16(header_end);
    reader.set_position(header_end);

    if (format > 1) {
      throw MidiParseError("Only MIDI format 0 and 1 are supported.");
    }
    if (track_count == 0 || (format == 0 && track_count != 1)) {
      throw MidiParseError("Invalid MIDI track count.");
    }
    if ((division & 0x8000) != 0 || division == 0) {
      throw MidiParseError("SMPTE or zero MIDI time division is unsupported.");
    }

    std::vector<MidiEvent> events;
    std::vector<TempoChange> tempos{{0, 500'000.0, 0}};
    std::size_t tempo_order = 1;

    for (std::uint16_t track = 0; track < track_count; ++track) {
      if (reader.read_string(4) != "MTrk") {
        throw MidiParseError("Missing MIDI track chunk.");
      }
      const std::size_t track_length = reader.read_uint32();
      const std::size_t track_begin = reader.position();
      if (track_length > reader.size() - track_begin) {
        throw MidiParseError("MIDI track exceeds file size.");
      }
      const std::size_t track_end = track_begin + track_length;

      std::uint64_t absolute_tick = 0;
      std::uint8_t running_status = 0;
      std::string track_name;

      while (reader.position() < track_end) {
        const std::uint32_t delta_tick = reader.read_variable_length(track_end);
        if (delta_tick >
            std::numeric_limits<std::uint64_t>::max() - absolute_tick) {
          throw MidiParseError("MIDI absolute tick overflow.");
        }
        absolute_tick += delta_tick;
        const std::uint8_t first_byte = reader.read_byte(track_end);
        std::uint8_t status = first_byte;
        std::uint8_t data1 = 0;
        bool has_data1 = false;

        if (first_byte < 0x80) {
          if (running_status == 0) {
            throw MidiParseError("MIDI running status has no status byte.");
          }
          status = running_status;
          data1 = first_byte;
          has_data1 = true;
        }

        if (status == 0xFF) {
          running_status = 0;
          const std::uint8_t type = reader.read_byte(track_end);
          const std::size_t length = reader.read_variable_length(track_end);
          const auto data = reader.read_data(length, track_end);
          if (type == 0x03) {
            track_name.assign(data.begin(), data.end());
          } else if (type == 0x51 && data.size() == 3) {
            const double tempo = static_cast<double>(data[0]) * 65'536.0 +
                                 static_cast<double>(data[1]) * 256.0 +
                                 static_cast<double>(data[2]);
            if (tempo > 0.0) {
              tempos.push_back({absolute_tick, tempo, tempo_order++});
            }
          }
          continue;
        }

        if (status == 0xF0 || status == 0xF7) {
          running_status = 0;
          reader.skip(reader.read_variable_length(track_end), track_end);
          continue;
        }
        if (status >= 0xF0) {
          throw MidiParseError("Unsupported system message in MIDI track.");
        }

        running_status = status;
        const std::uint8_t type = status & 0xF0;
        const std::uint8_t first_data =
            has_data1 ? data1 : reader.read_byte(track_end);
        if ((first_data & 0x80) != 0) {
          throw MidiParseError("Invalid MIDI data byte.");
        }

        switch (type) {
          case 0x80:
          case 0x90:
          case 0xA0:
          case 0xB0: {
            const std::uint8_t second_data = reader.read_byte(track_end);
            if ((second_data & 0x80) != 0) {
              throw MidiParseError("Invalid MIDI data byte.");
            }
            events.push_back({absolute_tick, 0.0, status, first_data,
                              second_data, track_name});
            break;
          }
          case 0xE0:
            static_cast<void>(reader.read_byte(track_end));
            break;
          case 0xC0:
          case 0xD0:
            break;
          default:
            throw MidiParseError("Invalid MIDI channel message.");
        }
      }
      reader.set_position(track_end);
    }

    apply_tempo_map(events, tempos, static_cast<double>(division));
    return events;
  }

  static void apply_tempo_map(std::vector<MidiEvent>& events,
                              std::vector<TempoChange>& tempos,
                              double ticks_per_quarter) {
    std::stable_sort(tempos.begin(), tempos.end(),
                     [](const TempoChange& left, const TempoChange& right) {
                       if (left.tick != right.tick) {
                         return left.tick < right.tick;
                       }
                       return left.order < right.order;
                     });

    std::vector<TempoChange> collapsed;
    for (const TempoChange& tempo : tempos) {
      if (!collapsed.empty() && collapsed.back().tick == tempo.tick) {
        collapsed.back() = tempo;
      } else {
        collapsed.push_back(tempo);
      }
    }

    std::stable_sort(events.begin(), events.end(),
                     [](const MidiEvent& left, const MidiEvent& right) {
                       return left.tick < right.tick;
                     });

    std::size_t tempo_index = 0;
    std::uint64_t previous_tick = 0;
    double seconds = 0.0;
    double current_tempo = collapsed.front().microseconds_per_quarter;

    for (MidiEvent& event : events) {
      while (tempo_index + 1 < collapsed.size() &&
             collapsed[tempo_index + 1].tick <= event.tick) {
        const TempoChange& next = collapsed[++tempo_index];
        seconds += static_cast<double>(next.tick - previous_tick) *
                   current_tempo / (1'000'000.0 * ticks_per_quarter);
        previous_tick = next.tick;
        current_tempo = next.microseconds_per_quarter;
      }
      event.time_in_seconds =
          seconds + static_cast<double>(event.tick - previous_tick) *
                        current_tempo / (1'000'000.0 * ticks_per_quarter);
    }
  }

  static std::vector<MidiEvent> select_piano_events(
      const std::vector<MidiEvent>& events) {
    std::vector<MidiEvent> relevant;
    for (const MidiEvent& event : events) {
      if (event.is_note_on() || event.is_note_off() ||
          event.is_poly_aftertouch() || event.is_pedal_control_change()) {
        relevant.push_back(event);
      }
    }

    std::vector<MidiEvent> named;
    for (const MidiEvent& event : relevant) {
      std::string name = event.track_name;
      std::transform(name.begin(), name.end(), name.begin(),
                     [](unsigned char character) {
                       return static_cast<char>(std::tolower(character));
                     });
      if (name.find("piano") != std::string::npos ||
          name.find("keyboard") != std::string::npos ||
          name.find("grand") != std::string::npos) {
        named.push_back(event);
      }
    }
    if (!named.empty()) {
      return named;
    }

    std::vector<MidiEvent> channel_zero;
    for (const MidiEvent& event : relevant) {
      if (event.channel() == 0) {
        channel_zero.push_back(event);
      }
    }
    return channel_zero.empty() ? relevant : channel_zero;
  }
};

#endif
