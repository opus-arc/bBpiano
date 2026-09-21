// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
// [ZERO AI-GENERATED CODE]
// Every line in this file is written and understood by its author. Every result
// is reproducible, every assumption is open to inspection, and every
// implementation stands open to criticism and challenge. AI may be used for
// non-core, replaceable, engineering work; this file, however, contains core
// logic that the author considers necessary to understand firsthand, explain
// line by line, and take full responsibility for, and is therefore implemented
// entirely by hand.
// ---------------------------------------------------------------------------
// [本文件承诺不含任何 AI 生成代码]
// 每一行代码均由作者亲自编写，并确知其意义。一切结果可以复现，一切假设可经受检验，一切实现经得起批评与质疑。
// AI 可用于非核心、可替代的工程工作；
// 本文件承载作者认为必须亲自理解、能够逐行解释并为之负责的核心逻辑，因此刻意保持完全人工实现。
//
// Ziyang Tan
// 2026-09-04
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <filesystem>
#include <iostream>
#include <mutex>
#include <stop_token>
#include <thread>

// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
// [ZERO AI-GENERATED CODE]
// Every line in this file is written and understood by its author. Every result is
// reproducible, every assumption is open to inspection, and every implementation
// stands open to criticism and challenge.
// AI may be used for non-core, replaceable, engineering work;
// this file, however, contains core logic that the author considers
// necessary to understand firsthand, explain line by line, and take full responsibility for,
// and is therefore implemented entirely by hand.
// ---------------------------------------------------------------------------
// [本文件承诺不含任何 AI 生成代码]
// 每一行代码均由作者亲自编写，并确知其意义。一切结果可以复现，一切假设可经受检验，一切实现经得起批评与质疑。
// AI 可用于非核心、可替代的工程工作；
// 本文件承载作者认为必须亲自理解、能够逐行解释并为之负责的核心逻辑，因此刻意保持完全人工实现。
//
// Ziyang Tan
// 2026-09-04
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifndef piano_controller_hpp
#define piano_controller_hpp

#include <stop_token>

// 向无锁队列投递命令

// ======================== ======================== ========================
// Initialize
// 初始化
// ======================== ======================== ========================
void bbpiano_init(double sample_rate);
void bbpiano_shutdown() noexcept;

void eval_init(double sample_rate);
void eval_shutdown() noexcept;

void soundcard_init(double sample_rate);
void soundcard_shutdown() noexcept;

// ======================== ======================== ========================
// Hardware callback and test
// 硬件回调与测试
// ======================== ======================== ========================
void get_next_buffer(float* out, int frame_count, double amplitude_limiter);
void print_engine_rate();

// ======================== ======================== ========================
// Piano gesture
// 钢琴手势
// ======================== ======================== ========================
void note_on(int midi_n, double velocity) noexcept;
void note_off(int midi_n, double velocity) noexcept;
void note_aftertouch(int midi_n, double pressure) noexcept;

// ======================== ======================== ========================
// Piano pedal
// 钢琴踏板
// ======================== ======================== ========================
void softpedal_control(double depth) noexcept;
void harmonicpedal_control(double depth) noexcept;
void sostenutopedal_control(double depth) noexcept;
void sustainpedal_control(double depth) noexcept;

void all_silence() noexcept;

// ======================== ======================== ========================
// Deal with Error
// 报错与线程
// ======================== ======================== ========================
void clear_piano_commands() noexcept;
bool wait_for_stop_or_audio_failure(std::stop_token stop_token) noexcept;
bool audio_render_failed() noexcept;
void report_audio_render_failure() noexcept;
void clear_audio_render_failure() noexcept;

#endif /* piano_controller_hpp */
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
// [ZERO AI-GENERATED CODE]
// Every line in this file is written and understood by its author. Every result
// is reproducible, every assumption is open to inspection, and every
// implementation stands open to criticism and challenge. AI may be used for
// non-core, replaceable, engineering work; this file, however, contains core
// logic that the author considers necessary to understand firsthand, explain
// line by line, and take full responsibility for, and is therefore implemented
// entirely by hand.
// ---------------------------------------------------------------------------
// [本文件承诺不含任何 AI 生成代码]
// 每一行代码均由作者亲自编写，并确知其意义。一切结果可以复现，一切假设可经受检验，一切实现经得起批评与质疑。
// AI 可用于非核心、可替代的工程工作；
// 本文件承载作者认为必须亲自理解、能够逐行解释并为之负责的核心逻辑，因此刻意保持完全人工实现。
//
// Ziyang Tan
// 2026-09-04
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifndef controller_hardware_hpp
#define controller_hardware_hpp

#include <stop_token>
#include <string>

// ======================== ======================== ========================
// Basic configurations
// 基础设置
// ======================== ======================== ========================
static constexpr double sample_rate = 44100.0;

// ======================== ======================== ========================
// Basic support
// 基础支持
// ======================== ======================== ========================
int cli_entry(int argc, char *argv[], const char *version, const char *logo,
              std::stop_token external_stop = {});

// ======================== ======================== ========================
// Services
// 服务
// ======================== ======================== ========================
void midi_service(const std::string &midi_path, std::stop_token stop_token);
void piano_service(std::stop_token stop_token);
void keyboard_service(std::stop_token stop_token);
void export_service(std::string export_midi_path_string,
                    std::stop_token stop_token);
void record_service(std::stop_token stop_token);
void test_service(std::stop_token stop_token);
void internal_test_service(std::stop_token stop_token);

#endif /* controller_hardware_hpp */

// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
// [ZERO AI-GENERATED CODE]
// Every line in this file is written and understood by its author. Every result
// is reproducible, every assumption is open to inspection, and every
// implementation stands open to criticism and challenge. AI may be used for
// non-core, replaceable, engineering work; this file, however, contains core
// logic that the author considers necessary to understand firsthand, explain
// line by line, and take full responsibility for, and is therefore implemented
// entirely by hand.
// ---------------------------------------------------------------------------
// [本文件承诺不含任何 AI 生成代码]
// 每一行代码均由作者亲自编写，并确知其意义。一切结果可以复现，一切假设可经受检验，一切实现经得起批评与质疑。
// AI 可用于非核心、可替代的工程工作；
// 本文件承载作者认为必须亲自理解、能够逐行解释并为之负责的核心逻辑，因此刻意保持完全人工实现。
//
// Ziyang Tan
// 2026-09-03
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

#ifndef cli_helper_hpp
#define cli_helper_hpp

#include <stop_token>
int cli_helper(int argc, char *argv[], const char *version, const char *logo,
               std::stop_token external_stop = {});

#endif /* cli_helper_hpp */

#ifndef BBPL_MIDI_EXPORTER_SERVICE_HPP
#define BBPL_MIDI_EXPORTER_SERVICE_HPP

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <limits>
#include <stdexcept>
#include <stop_token>
#include <string>
#include <vector>

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

class MidiExporter {
 public:
  enum class Result { completed, cancelled };

  MidiExporter() = delete;

  // The coordinator must stop the hardware Soundcard before calling this.
  // Rendering is synchronous and faster than real time; no service thread is
  // created here. A temporary file is atomically renamed on success.
  static Result export_wav(const std::string& midi_file_path,
                           std::filesystem::path output_path = {},
                           std::uint32_t sample_rate = 44'100,
                           double tail_seconds = 5.0,
                           std::stop_token stop_token = {}) {
    if (midi_file_path.empty()) {
      throw std::invalid_argument("No MIDI file path provided.");
    }
    if (sample_rate == 0 ||
        sample_rate >
            std::numeric_limits<std::uint32_t>::max() / sizeof(float) ||
        !std::isfinite(tail_seconds) || tail_seconds < 0.0) {
      throw std::invalid_argument("Invalid WAV rendering configuration.");
    }

    std::vector<MidiService::MidiEvent> events =
        MidiService::load_events(midi_file_path);
    if (output_path.empty()) {
      output_path = std::filesystem::path(midi_file_path);
      output_path.replace_extension(".wav");
    }
    validate_output_path(output_path);

    const double event_end =
        events.empty() ? 0.0 : events.back().time_in_seconds;
    const long double frame_count_value =
        (static_cast<long double>(event_end) + tail_seconds) * sample_rate;
    constexpr std::uint64_t k_max_data_bytes =
        std::numeric_limits<std::uint32_t>::max() - 36ULL;
    if (!std::isfinite(frame_count_value) || frame_count_value < 0.0L ||
        frame_count_value > k_max_data_bytes / sizeof(float)) {
      throw std::runtime_error(
          "WAV exceeds the RIFF 32-bit size limit; RF64 is not implemented.");
    }
    const std::uint64_t total_frames =
        static_cast<std::uint64_t>(std::ceil(frame_count_value));
    const std::uint64_t data_bytes = total_frames * sizeof(float);

    std::filesystem::path temporary_path = output_path;
    temporary_path += ".bbpl-part";
    if (std::filesystem::exists(temporary_path)) {
      throw std::runtime_error("Temporary WAV output already exists: " +
                               temporary_path.string());
    }

    all_silence();
    try {
      std::ofstream output(temporary_path, std::ios::binary);
      if (!output) {
        throw std::runtime_error("Cannot create WAV output: " +
                                 output_path.string());
      }
      write_header(output, sample_rate, static_cast<std::uint32_t>(data_bytes));

      constexpr std::size_t k_buffer_frames = 512;
      std::array<float, k_buffer_frames> buffer{};
      std::uint64_t frame = 0;
      std::size_t event_index = 0;

      while (frame < total_frames) {
        if (stop_token.stop_requested()) {
          output.close();
          all_silence();
          std::error_code ignored;
          std::filesystem::remove(temporary_path, ignored);
          return Result::cancelled;
        }

        while (event_index < events.size() &&
               event_frame(events[event_index], sample_rate) <= frame) {
          MidiService::dispatch_event(events[event_index]);
          ++event_index;
        }

        std::uint64_t segment_end =
            std::min<std::uint64_t>(total_frames, frame + k_buffer_frames);
        if (event_index < events.size()) {
          segment_end = std::min(segment_end,
                                 event_frame(events[event_index], sample_rate));
        }

        if (segment_end == frame) {
          continue;
        }

        const auto frames_to_render =
            static_cast<std::size_t>(segment_end - frame);
        get_next_buffer(buffer.data(), static_cast<int>(frames_to_render), 1.0);
        output.write(
            reinterpret_cast<const char*>(buffer.data()),
            static_cast<std::streamsize>(frames_to_render * sizeof(float)));
        if (!output) {
          throw std::runtime_error("Failed while writing WAV output: " +
                                   output_path.string());
        }
        frame = segment_end;
      }

      all_silence();
      output.close();
      if (!output) {
        throw std::runtime_error("Failed to finalize WAV output: " +
                                 output_path.string());
      }
      std::filesystem::rename(temporary_path, output_path);
      return Result::completed;
    } catch (...) {
      all_silence();
      std::error_code ignored;
      std::filesystem::remove(temporary_path, ignored);
      throw;
    }
  }

 private:
  static std::uint64_t event_frame(const MidiService::MidiEvent& event,
                                   std::uint32_t sample_rate) noexcept {
    return static_cast<std::uint64_t>(
        std::llround(event.time_in_seconds * sample_rate));
  }

  static void validate_output_path(const std::filesystem::path& output_path) {
    if (std::filesystem::exists(output_path)) {
      throw std::runtime_error("Refusing to overwrite existing WAV file: " +
                               output_path.string());
    }
    const std::filesystem::path parent = output_path.parent_path();
    if (!parent.empty() && !std::filesystem::exists(parent)) {
      throw std::runtime_error("WAV output directory does not exist: " +
                               parent.string());
    }
  }

  static void write_uint16_le(std::ostream& output, std::uint16_t value) {
    output.put(static_cast<char>(value & 0xFF));
    output.put(static_cast<char>((value >> 8) & 0xFF));
  }

  static void write_uint32_le(std::ostream& output, std::uint32_t value) {
    output.put(static_cast<char>(value & 0xFF));
    output.put(static_cast<char>((value >> 8) & 0xFF));
    output.put(static_cast<char>((value >> 16) & 0xFF));
    output.put(static_cast<char>((value >> 24) & 0xFF));
  }

  static void write_header(std::ostream& output, std::uint32_t sample_rate,
                           std::uint32_t data_bytes) {
    constexpr std::uint16_t k_audio_format_float = 3;
    constexpr std::uint16_t k_channels = 1;
    constexpr std::uint16_t k_bits_per_sample = 32;
    constexpr std::uint16_t k_block_align = sizeof(float);
    constexpr std::uint32_t k_format_chunk_size = 16;
    const std::uint32_t byte_rate = sample_rate * k_block_align;
    const std::uint32_t riff_size =
        4 + (8 + k_format_chunk_size) + (8 + data_bytes);

    output.write("RIFF", 4);
    write_uint32_le(output, riff_size);
    output.write("WAVEfmt ", 8);
    write_uint32_le(output, k_format_chunk_size);
    write_uint16_le(output, k_audio_format_float);
    write_uint16_le(output, k_channels);
    write_uint32_le(output, sample_rate);
    write_uint32_le(output, byte_rate);
    write_uint16_le(output, k_block_align);
    write_uint16_le(output, k_bits_per_sample);
    output.write("data", 4);
    write_uint32_le(output, data_bytes);
  }
};

#endif
#ifndef BBPL_MIDI_KEYBOARD_SERVICE_HPP
#define BBPL_MIDI_KEYBOARD_SERVICE_HPP

#include <algorithm>
#include <cstdint>
#include <optional>

#ifndef BBPL_MIDI_INPUTHUB_SERVICE_HPP
#define BBPL_MIDI_INPUTHUB_SERVICE_HPP

#include <CoreMIDI/CoreMIDI.h>

#include <algorithm>
#include <array>
#include <condition_variable>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>
#include <span>
#include <stdexcept>
#include <utility>
#include <vector>

class MidiInputHub {
 public:
  using MidiMessage = std::span<const std::uint8_t>;
  using MessageHandler = std::function<void(MidiMessage)>;

 private:
  struct HandlerSlot {
    explicit HandlerSlot(MessageHandler new_handler)
        : handler(std::move(new_handler)) {}

    void invoke(MidiMessage message) noexcept {
      {
        std::lock_guard lock(mutex);
        if (!enabled) {
          return;
        }
        ++active_calls;
      }

      try {
        handler(message);
      } catch (...) {
        // Never let an exception cross the CoreMIDI C callback boundary.
      }

      {
        std::lock_guard lock(mutex);
        --active_calls;
      }
      condition.notify_all();
    }

    void disable_and_synchronize() noexcept {
      std::unique_lock lock(mutex);
      enabled = false;
      condition.wait(lock, [this] { return active_calls == 0; });
      handler = {};
    }

    bool is_enabled() const noexcept {
      std::lock_guard lock(mutex);
      return enabled;
    }

    mutable std::mutex mutex;
    std::condition_variable condition;
    MessageHandler handler;
    std::size_t active_calls = 0;
    bool enabled = true;
  };

 public:
  class Subscription {
   public:
    Subscription() noexcept = default;
    Subscription(const Subscription&) = delete;
    Subscription& operator=(const Subscription&) = delete;

    Subscription(Subscription&& other) noexcept
        : slot_(std::exchange(other.slot_, {})) {}

    Subscription& operator=(Subscription&& other) noexcept {
      if (this != &other) {
        reset();
        slot_ = std::exchange(other.slot_, {});
      }
      return *this;
    }

    ~Subscription() { reset(); }

    void reset() noexcept {
      if (slot_) {
        slot_->disable_and_synchronize();
        slot_.reset();
      }
    }

    explicit operator bool() const noexcept { return static_cast<bool>(slot_); }

   private:
    friend class MidiInputHub;
    explicit Subscription(std::shared_ptr<HandlerSlot> slot)
        : slot_(std::move(slot)) {}

    std::shared_ptr<HandlerSlot> slot_;
  };

  static constexpr std::uint8_t k_sustain_pedal_controller = 64;
  static constexpr std::uint8_t k_sostenuto_pedal_controller = 66;
  static constexpr std::uint8_t k_soft_pedal_controller = 67;
  static constexpr std::uint8_t k_harmonic_pedal_controller = 68;

  MidiInputHub() = default;
  MidiInputHub(const MidiInputHub&) = delete;
  MidiInputHub& operator=(const MidiInputHub&) = delete;
  MidiInputHub(MidiInputHub&&) = delete;
  MidiInputHub& operator=(MidiInputHub&&) = delete;

  ~MidiInputHub() { stop(); }

  Subscription subscribe(MessageHandler handler) {
    if (!handler) {
      throw std::invalid_argument("Invalid MIDI message handler.");
    }

    auto slot = std::make_shared<HandlerSlot>(std::move(handler));
    std::lock_guard lock(state_mutex_);
    erase_disabled_handlers_locked();
    handlers_.push_back(slot);
    return Subscription(std::move(slot));
  }

  void start() {
    std::lock_guard lifecycle_lock(lifecycle_mutex_);
    {
      std::lock_guard state_lock(state_mutex_);
      if (running_) {
        return;
      }
    }

    const ItemCount source_count = MIDIGetNumberOfSources();
    if (source_count == 0) {
      throw std::runtime_error("No MIDI input device found.");
    }

    MIDIClientRef client = 0;
    MIDIPortRef input_port = 0;
    std::vector<std::unique_ptr<SourceConnection>> connections;

    OSStatus status = MIDIClientCreate(CFSTR("bbpl MIDI Input Hub"), nullptr,
                                       nullptr, &client);
    if (status != noErr || client == 0) {
      throw std::runtime_error("Failed to create CoreMIDI client.");
    }

    status = MIDIInputPortCreate(client, CFSTR("bbpl MIDI Input Port"),
                                 &MidiInputHub::midi_read_proc, nullptr,
                                 &input_port);
    if (status != noErr || input_port == 0) {
      MIDIClientDispose(client);
      throw std::runtime_error("Failed to create CoreMIDI input port.");
    }

    connections.reserve(static_cast<std::size_t>(source_count));
    for (ItemCount index = 0; index < source_count; ++index) {
      const MIDIEndpointRef source = MIDIGetSource(index);
      if (source == 0) {
        continue;
      }

      auto connection = std::make_unique<SourceConnection>();
      connection->hub = this;
      connection->source = source;
      status = MIDIPortConnectSource(input_port, source, connection.get());
      if (status == noErr) {
        connections.push_back(std::move(connection));
      }
    }

    if (connections.empty()) {
      MIDIPortDispose(input_port);
      MIDIClientDispose(client);
      throw std::runtime_error("No MIDI input device could be opened.");
    }

    {
      std::lock_guard state_lock(state_mutex_);
      client_ = client;
      input_port_ = input_port;
      connections_ = std::move(connections);
      accepting_callbacks_ = true;
      running_ = true;
    }
  }

  void stop() noexcept {
    std::lock_guard lifecycle_lock(lifecycle_mutex_);

    MIDIClientRef client = 0;
    MIDIPortRef input_port = 0;
    {
      std::lock_guard state_lock(state_mutex_);
      if (!running_) {
        return;
      }
      accepting_callbacks_ = false;
      running_ = false;
      client = client_;
      input_port = input_port_;
    }

    for (const auto& connection : connections_) {
      if (input_port != 0 && connection && connection->source != 0) {
        MIDIPortDisconnectSource(input_port, connection->source);
      }
    }
    if (input_port != 0) {
      MIDIPortDispose(input_port);
    }
    if (client != 0) {
      MIDIClientDispose(client);
    }

    {
      std::unique_lock state_lock(state_mutex_);
      callback_condition_.wait(state_lock,
                               [this] { return active_callbacks_ == 0; });
      client_ = 0;
      input_port_ = 0;
      connections_.clear();
    }
  }

  bool is_running() const noexcept {
    std::lock_guard lock(state_mutex_);
    return running_;
  }

 private:
  struct ParsedMessage {
    std::array<std::uint8_t, 3> bytes{};
    std::size_t size = 0;
  };

  struct ParserState {
    std::mutex mutex;
    std::uint8_t running_status = 0;
    std::uint8_t current_status = 0;
    std::array<std::uint8_t, 2> data{};
    std::size_t expected_data = 0;
    std::size_t collected_data = 0;
    bool in_system_exclusive = false;
  };

  struct SourceConnection {
    MidiInputHub* hub = nullptr;
    MIDIEndpointRef source = 0;
    ParserState parser;
  };

  class CallbackGuard {
   public:
    explicit CallbackGuard(MidiInputHub& hub) noexcept
        : hub_(&hub), entered_(hub.begin_callback()) {}

    ~CallbackGuard() {
      if (entered_) {
        hub_->end_callback();
      }
    }

    explicit operator bool() const noexcept { return entered_; }

   private:
    MidiInputHub* hub_;
    bool entered_;
  };

  static void midi_read_proc(const MIDIPacketList* packet_list, void*,
                             void* source_connection) noexcept {
    auto* connection = static_cast<SourceConnection*>(source_connection);
    if (packet_list == nullptr || connection == nullptr ||
        connection->hub == nullptr) {
      return;
    }

    CallbackGuard callback(*connection->hub);
    if (!callback) {
      return;
    }

    const MIDIPacket* packet = &packet_list->packet[0];
    for (UInt32 index = 0; index < packet_list->numPackets; ++index) {
      connection->hub->parse_packet(connection->parser, packet->data,
                                    packet->length);
      packet = MIDIPacketNext(packet);
    }
  }

  bool begin_callback() noexcept {
    std::lock_guard lock(state_mutex_);
    if (!accepting_callbacks_) {
      return false;
    }
    ++active_callbacks_;
    return true;
  }

  void end_callback() noexcept {
    {
      std::lock_guard lock(state_mutex_);
      --active_callbacks_;
    }
    callback_condition_.notify_all();
  }

  static std::size_t data_bytes_for_status(std::uint8_t status) noexcept {
    switch (status & 0xF0) {
      case 0x80:
      case 0x90:
      case 0xA0:
      case 0xB0:
      case 0xE0:
        return 2;
      case 0xC0:
      case 0xD0:
        return 1;
      default:
        break;
    }

    switch (status) {
      case 0xF1:
      case 0xF3:
        return 1;
      case 0xF2:
        return 2;
      case 0xF6:
        return 0;
      default:
        return 0;
    }
  }

  void parse_packet(ParserState& parser, const Byte* data,
                    UInt16 length) noexcept {
    std::vector<ParsedMessage> parsed;
    parsed.reserve(static_cast<std::size_t>(length) / 2 + 1);

    {
      std::lock_guard parser_lock(parser.mutex);
      for (UInt16 index = 0; index < length; ++index) {
        const std::uint8_t byte = static_cast<std::uint8_t>(data[index]);

        // MIDI real-time bytes can appear anywhere and do not disturb
        // running status or a partially collected channel message.
        if (byte >= 0xF8) {
          ParsedMessage message;
          message.bytes[0] = byte;
          message.size = 1;
          parsed.push_back(message);
          continue;
        }

        if (parser.in_system_exclusive) {
          if (byte == 0xF7) {
            parser.in_system_exclusive = false;
          }
          continue;
        }

        if ((byte & 0x80) != 0) {
          parser.collected_data = 0;
          parser.current_status = 0;

          if (byte == 0xF0) {
            parser.in_system_exclusive = true;
            parser.running_status = 0;
            continue;
          }
          if (byte == 0xF7) {
            parser.running_status = 0;
            continue;
          }

          parser.current_status = byte;
          parser.expected_data = data_bytes_for_status(byte);
          if (byte < 0xF0) {
            parser.running_status = byte;
          } else {
            parser.running_status = 0;
          }

          if (parser.expected_data == 0) {
            ParsedMessage message;
            message.bytes[0] = byte;
            message.size = 1;
            parsed.push_back(message);
            parser.current_status = 0;
          }
          continue;
        }

        if (parser.current_status == 0) {
          if (parser.running_status == 0) {
            continue;
          }
          parser.current_status = parser.running_status;
          parser.expected_data = data_bytes_for_status(parser.current_status);
          parser.collected_data = 0;
        }

        if (parser.collected_data < parser.data.size()) {
          parser.data[parser.collected_data++] = byte;
        }

        if (parser.collected_data == parser.expected_data) {
          ParsedMessage message;
          message.bytes[0] = parser.current_status;
          for (std::size_t offset = 0; offset < parser.expected_data;
               ++offset) {
            message.bytes[offset + 1] = parser.data[offset];
          }
          message.size = parser.expected_data + 1;
          parsed.push_back(message);
          parser.current_status = 0;
          parser.collected_data = 0;
        }
      }
    }

    for (const ParsedMessage& message : parsed) {
      dispatch(MidiMessage(message.bytes.data(), message.size));
    }
  }

  void dispatch(MidiMessage message) noexcept {
    std::vector<std::shared_ptr<HandlerSlot>> handlers;
    {
      std::lock_guard lock(state_mutex_);
      handlers.reserve(handlers_.size());
      for (const auto& weak_slot : handlers_) {
        if (auto slot = weak_slot.lock()) {
          handlers.push_back(std::move(slot));
        }
      }
    }

    for (const auto& handler : handlers) {
      handler->invoke(message);
    }
  }

  void erase_disabled_handlers_locked() {
    handlers_.erase(
        std::remove_if(handlers_.begin(), handlers_.end(),
                       [](const std::weak_ptr<HandlerSlot>& weak_slot) {
                         const auto slot = weak_slot.lock();
                         return !slot || !slot->is_enabled();
                       }),
        handlers_.end());
  }

  mutable std::mutex lifecycle_mutex_;
  mutable std::mutex state_mutex_;
  std::condition_variable callback_condition_;
  bool running_ = false;
  bool accepting_callbacks_ = false;
  std::size_t active_callbacks_ = 0;

  MIDIClientRef client_ = 0;
  MIDIPortRef input_port_ = 0;
  std::vector<std::unique_ptr<SourceConnection>> connections_;
  std::vector<std::weak_ptr<HandlerSlot>> handlers_;
};

#endif

class MidiKeyboard {
public:
  explicit MidiKeyboard(MidiInputHub &input_hub) noexcept
      : input_hub_(input_hub) {}

  MidiKeyboard(const MidiKeyboard &) = delete;
  //  MidiKeyboard& operator=(const MidiKeyboard&) = delete;
  MidiKeyboard(MidiKeyboard &&) = delete;
  MidiKeyboard &operator=(MidiKeyboard &&) = delete;

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
    subscription_->reset(); // Synchronizes with an in-flight CoreMIDI callback.
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

  MidiInputHub &input_hub_;
  std::optional<MidiInputHub::Subscription> subscription_;
};

#endif
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

namespace {

void wait_for_stop(std::stop_token stop_token) {
  std::mutex mutex;
  std::condition_variable_any condition;
  std::unique_lock lock(mutex);
  condition.wait(lock, stop_token, [] { return false; });
}

} // namespace

// ======================== ======================== ========================
// Basic support
// 基础支持
// ======================== ======================== ========================
int cli_entry(int argc, char *argv[], const char *version, const char *logo,
              std::stop_token external_stop) {
  return cli_helper(argc, argv, version, logo, external_stop);
}

// ======================== ======================== ========================
// Services
// 服务
// ======================== ======================== ========================
void midi_service(const std::string &midi_path, std::stop_token stop_token) {
  std::cout << "Playing MIDI: " << midi_path << '\n';
  const MidiService::Result result =
      MidiService::play(1.0, 0.0, midi_path, stop_token);
  if (result == MidiService::Result::completed) {
    std::cout << "MIDI playback completed.\n";
  }
}
void piano_service(std::stop_token stop_token) {
  MidiInputHub input_hub;
  MidiKeyboard keyboard(input_hub);
  keyboard.start();
  input_hub.start();

  std::cout << "MIDI piano started (Ctrl-C exits).\n";

  wait_for_stop(stop_token);

  input_hub.stop();
  keyboard.stop();
}
void keyboard_service(std::stop_token stop_token) {
  PcKeyboard keyboard;
  keyboard.run(stop_token);
}
void export_service(std::string midi_path, std::stop_token stop_token) {
  std::filesystem::path output_path(midi_path);
  output_path.replace_extension(".wav");
  const MidiExporter::Result result = MidiExporter::export_wav(
      midi_path, output_path, static_cast<std::uint32_t>(sample_rate), 5.0,
      stop_token);
  if (result == MidiExporter::Result::completed) {
    std::cout << "Exported WAV: " << output_path << '\n';
  }
}
void record_service(std::stop_token stop_token) {
  MidiInputHub input_hub;
  MidiKeyboard keyboard(input_hub);
  MidiRecorder recorder(input_hub);

  keyboard.start();
  recorder.start();
  input_hub.start();
  std::cout << "MIDI recording started (Ctrl-C saves and exits).\n";

  wait_for_stop(stop_token);

  input_hub.stop();
  const std::filesystem::path output_path = recorder.stop();
  keyboard.stop();
  std::cout << "Recorded MIDI: " << output_path << '\n';
}
void test_service(std::stop_token stop_token) {
  std::cout << "Test service started.\n\n";

  for (int midi = 21; midi <= 108; ++midi) {
    note_on(midi, 110.0);
  }

  std::this_thread::sleep_for(std::chrono::milliseconds(500));

  print_engine_rate();

  //    wait_for_stop(stop_token);
}

void internal_test_service(std::stop_token stop_token) {
  std::cout << "Internal test started (Ctrl-C exits).\n";

  sustainpedal_control(0.6);
  note_on(69, 110);
  wait_for_stop(stop_token);
  //    note_off(69, 0.0);
  all_silence();
}
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
// [ZERO AI-GENERATED CODE]
// Every line in this file is written and understood by its author. Every result is
// reproducible, every assumption is open to inspection, and every implementation
// stands open to criticism and challenge.
// AI may be used for non-core, replaceable, engineering work;
// this file, however, contains core logic that the author considers
// necessary to understand firsthand, explain line by line, and take full responsibility for,
// and is therefore implemented entirely by hand.
// ---------------------------------------------------------------------------
// [本文件承诺不含任何 AI 生成代码]
// 每一行代码均由作者亲自编写，并确知其意义。一切结果可以复现，一切假设可经受检验，一切实现经得起批评与质疑。
// AI 可用于非核心、可替代的工程工作；
// 本文件承载作者认为必须亲自理解、能够逐行解释并为之负责的核心逻辑，因此刻意保持完全人工实现。
//
// Ziyang Tan
// 2026-09-04
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

#include <stop_token>
#include <atomic>
#include <cstdint>
#include <memory>
#include <stdexcept>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
// [ZERO AI-GENERATED CODE]
// Every line in this file is written and understood by its author. Every result is
// reproducible, every assumption is open to inspection, and every implementation
// stands open to criticism and challenge.
// AI may be used for non-core, replaceable, engineering work;
// this file, however, contains core logic that the author considers
// necessary to understand firsthand, explain line by line, and take full responsibility for,
// and is therefore implemented entirely by hand.
// ---------------------------------------------------------------------------
// [本文件承诺不含任何 AI 生成代码]
// 每一行代码均由作者亲自编写，并确知其意义。一切结果可以复现，一切假设可经受检验，一切实现经得起批评与质疑。
// AI 可用于非核心、可替代的工程工作；
// 本文件承载作者认为必须亲自理解、能够逐行解释并为之负责的核心逻辑，因此刻意保持完全人工实现。
//
// Ziyang Tan
// 2026-09-04
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

#ifndef piano_model_hpp
#define piano_model_hpp

#include <iostream>
#include <array>

// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
// [ZERO AI-GENERATED CODE]
// Every line in this file is written and understood by its author. Every result is
// reproducible, every assumption is open to inspection, and every implementation
// stands open to criticism and challenge.
// AI may be used for non-core, replaceable, engineering work;
// this file, however, contains core logic that the author considers
// necessary to understand firsthand, explain line by line, and take full responsibility for,
// and is therefore implemented entirely by hand.
// ---------------------------------------------------------------------------
// [本文件承诺不含任何 AI 生成代码]
// 每一行代码均由作者亲自编写，并确知其意义。一切结果可以复现，一切假设可经受检验，一切实现经得起批评与质疑。
// AI 可用于非核心、可替代的工程工作；
// 本文件承载作者认为必须亲自理解、能够逐行解释并为之负责的核心逻辑，因此刻意保持完全人工实现。
//
// Ziyang Tan
// 2026-09-04
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

#ifndef key_model_hpp
#define key_model_hpp

#include <array>

// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
// [ZERO AI-GENERATED CODE]
// Every line in this file is written and understood by its author. Every result is
// reproducible, every assumption is open to inspection, and every implementation
// stands open to criticism and challenge.
// AI may be used for non-core, replaceable, engineering work;
// this file, however, contains core logic that the author considers
// necessary to understand firsthand, explain line by line, and take full responsibility for,
// and is therefore implemented entirely by hand.
// ---------------------------------------------------------------------------
// [本文件承诺不含任何 AI 生成代码]
// 每一行代码均由作者亲自编写，并确知其意义。一切结果可以复现，一切假设可经受检验，一切实现经得起批评与质疑。
// AI 可用于非核心、可替代的工程工作；
// 本文件承载作者认为必须亲自理解、能够逐行解释并为之负责的核心逻辑，因此刻意保持完全人工实现。
//
// Ziyang Tan
// 2026-09-04
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

#ifndef string_model_hpp
#define string_model_hpp

#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <cmath>
#include <numbers>

// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
// [ZERO AI-GENERATED CODE]
// Every line in this file is written and understood by its author. Every result is
// reproducible, every assumption is open to inspection, and every implementation
// stands open to criticism and challenge.
// AI may be used for non-core, replaceable, engineering work;
// this file, however, contains core logic that the author considers
// necessary to understand firsthand, explain line by line, and take full responsibility for,
// and is therefore implemented entirely by hand.
// ---------------------------------------------------------------------------
// [本文件承诺不含任何 AI 生成代码]
// 每一行代码均由作者亲自编写，并确知其意义。一切结果可以复现，一切假设可经受检验，一切实现经得起批评与质疑。
// AI 可用于非核心、可替代的工程工作；
// 本文件承载作者认为必须亲自理解、能够逐行解释并为之负责的核心逻辑，因此刻意保持完全人工实现。
//
// Ziyang Tan
// 2026-04-06
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


#ifndef damper_model_hpp
#define damper_model_hpp

#include <iostream>


class Damper {
    
    float z1 = 0.0f;
    float z2 = 0.0f;

public:
    
    inline void process(float &x) {
        // ==========================
        // Temporary Damper Controls
        // ==========================
        constexpr float lowLoss   = 0.020f; // 低频耗散：0.020~0.070
        constexpr float highLoss  = 0.25f;  // 高频抓取：0.25~0.70
        constexpr float damperMix = 0.38f;  // 毛毡低通占比：0.30~0.70

        constexpr float loopGain = 1.0f - lowLoss;
        constexpr float wet = damperMix * highLoss;
        constexpr float dry = 1.0f - wet;

        constexpr float dryGain = loopGain * dry;
        constexpr float wetGain = loopGain * wet;

        // Gentle second-order low-pass damper color.
        constexpr float b0 = 0.292893218813f;
        constexpr float b1 = 0.585786437627f;
        constexpr float b2 = 0.292893218813f;
        constexpr float a2 = 0.171572875254f;

        const float y = b0 * x + z1;

        z1 = b1 * x + z2;
        z2 = b2 * x - a2 * y;

        x = dryGain * x + wetGain * y;
    }
    
    inline void system_reset() noexcept {
        z1 = 0.0f;
        z2 = 0.0f;
    }
    
    inline float state_energy() const noexcept {
        return 0.5f * (z1 * z1 + z2 * z2);
    }
};




#endif /* damper_model_hpp */
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
// [ZERO AI-GENERATED CODE]
// Every line in this file is written and understood by its author. Every result is
// reproducible, every assumption is open to inspection, and every implementation
// stands open to criticism and challenge.
// AI may be used for non-core, replaceable, engineering work;
// this file, however, contains core logic that the author considers
// necessary to understand firsthand, explain line by line, and take full responsibility for,
// and is therefore implemented entirely by hand.
// ---------------------------------------------------------------------------
// [本文件承诺不含任何 AI 生成代码]
// 每一行代码均由作者亲自编写，并确知其意义。一切结果可以复现，一切假设可经受检验，一切实现经得起批评与质疑。
// AI 可用于非核心、可替代的工程工作；
// 本文件承载作者认为必须亲自理解、能够逐行解释并为之负责的核心逻辑，因此刻意保持完全人工实现。
//
// Ziyang Tan
// 2026-04-06
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifndef fractional_filter_hpp
#define fractional_filter_hpp

#include <iostream>

class FractionalFilter {
    float a1 = 0.0;
    
    float x1 = 0.0;
    float y1 = 0.0;
    
public:
    
    bool bypass = false;

    FractionalFilter(double phase_delay_samples,
                     double target_omega) {
        if (std::abs(phase_delay_samples) < 1.0e-12) {
            bypass = true;
            a1 = 0.0f;
            return;
        }

        const double q =
            std::tan(0.5 * target_omega * phase_delay_samples) /
            std::tan(0.5 * target_omega);

        const double coefficient = (1.0 - q) / (1.0 + q);

        if (!std::isfinite(coefficient) ||
            std::abs(coefficient) >= 1.0) {
            throw std::runtime_error(
                "fractional_filter: invalid allpass coefficient");
        }

        a1 = static_cast<float>(coefficient);
    }
    
    inline void process(float& x) {
        if (bypass) {
            return;
        }
        // y = a1 * x + x1 - a1 * y1;
        const float y =
            x1 + a1 * (x - y1);

        x1 = x;
        y1 = y;
        x = y;
    }
    
    inline void system_reset() {
        x1 = 0.0;
        y1 = 0.0;
    }
    
    inline float state_energy() const noexcept {
        return 0.5f * (x1 * x1 + y1 * y1);
    }
};




#endif /* fractional_filter_hpp */
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
// 2026-04-06
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

#ifndef loss_filter_hpp
#define loss_filter_hpp

#include <algorithm>
#include <array>
#include <complex>
#include <cstddef>
#include <numbers>

// External physics-first prototype: sigma=b1+b2*f^2; C3-C5 scope.
class LossFilter {
    inline static constexpr int kMidiMin = 21;
    inline static constexpr int kMidiMax = 108;
    inline static constexpr std::size_t kSectionCount = 2;
    struct LossShelf { float b0; float b1; float a1; };
    struct LossPreset { int midi; double referenceF1; double phaseDelaySamples;
        double groupDelaySamples; std::array<LossShelf, kSectionCount> sections; };
public:
    LossPreset lossPreset;
private:
    std::array<float, kSectionCount> state_{};
public:
    explicit LossFilter(int midi) noexcept : lossPreset(kPresets[
        static_cast<std::size_t>(std::clamp(midi, kMidiMin, kMidiMax) - kMidiMin)]) {}
    [[nodiscard]] inline double get_group_delay() const noexcept { return lossPreset.groupDelaySamples; }
    [[nodiscard]] inline double get_phase_delay(double sampleRate, double frequency) const {
        const double omega = 2.0 * std::numbers::pi_v<double> * frequency / sampleRate;
        const std::complex<double> z1 = std::polar(1.0, -omega);
        std::complex<double> response{1.0, 0.0};
        for (const auto& c : lossPreset.sections)
            response *= (double(c.b0) + double(c.b1) * z1) / (1.0 + double(c.a1) * z1);
        return -std::arg(response) / omega;
    }
    inline void process(float& x) noexcept {
        for (std::size_t i = 0; i < kSectionCount; ++i) {
            const auto& c = lossPreset.sections[i];
            const float y = c.b0 * x + state_[i];
            state_[i] = c.b1 * x - c.a1 * y; x = y;
        }
    }
    inline void system_reset() noexcept { state_.fill(0.0f); }
    [[nodiscard]] inline float state_energy() const noexcept {
        float energy = 0.0f; for (float value : state_) energy += value * value;
        return energy / static_cast<float>(kSectionCount);
    }
private:
    inline static constexpr std::array<LossPreset, 88> kPresets = {{
        // MIDI 21; global; p95 T60 realization error 1.247%
        LossPreset{21, 27.5, 0.90716426692764396, 0.9071646747176394, {{
            LossShelf{0.509180427f, 0.508661687f, 0.0483981334f},
            LossShelf{0.524460435f, 0.52394706f, 0.0484074801f},
        }}},
        // MIDI 22; global; p95 T60 realization error 1.209%
        LossPreset{22, 29.13523509488062, 0.88551598492877714, 0.88551653056390844, {{
            LossShelf{0.515856087f, 0.515351951f, 0.0604129694f},
            LossShelf{0.530497193f, 0.529971659f, 0.0604688823f},
        }}},
        // MIDI 23; global; p95 T60 realization error 1.173%
        LossPreset{23, 30.867706328507751, 0.86378779457930455, 0.86378849770897626, {{
            LossShelf{0.522680938f, 0.522195697f, 0.0727941841f},
            LossShelf{0.536697924f, 0.536072493f, 0.0727704167f},
        }}},
        // MIDI 24; global; p95 T60 realization error 1.134%
        LossPreset{24, 32.703195662574828, 0.84231887734651623, 0.84231975921181745, {{
            LossShelf{0.529564381f, 0.529080033f, 0.085329771f},
            LossShelf{0.542902589f, 0.542405844f, 0.0853083879f},
        }}},
        // MIDI 25; global; p95 T60 realization error 1.100%
        LossPreset{25, 34.64782887210901, 0.82080496635912426, 0.82080605026171982, {{
            LossShelf{0.536565423f, 0.536063969f, 0.0981385782f},
            LossShelf{0.549319744f, 0.548812091f, 0.0981318802f},
        }}},
        // MIDI 26; global; p95 T60 realization error 1.060%
        LossPreset{26, 36.70809598967594, 0.79922932279552239, 0.79923063493358293, {{
            LossShelf{0.54370755f, 0.543204606f, 0.111300826f},
            LossShelf{0.55590862f, 0.555393159f, 0.111301772f},
        }}},
        // MIDI 27; global; p95 T60 realization error 1.015%
        LossPreset{27, 38.890872965260115, 0.77695290386536875, 0.77695446585983385, {{
            LossShelf{0.551802397f, 0.549461842f, 0.124579713f},
            LossShelf{0.562767565f, 0.561819613f, 0.124587208f},
        }}},
        // MIDI 28; global; p95 T60 realization error 0.980%
        LossPreset{28, 41.203444614108747, 0.75690961413037727, 0.75691146524411568, {{
            LossShelf{0.558250427f, 0.557452202f, 0.137990847f},
            LossShelf{0.569241643f, 0.568750918f, 0.137992606f},
        }}},
        // MIDI 29; global; p95 T60 realization error 0.937%
        LossPreset{29, 43.653528929125486, 0.73630926537057295, 0.73631143434880675, {{
            LossShelf{0.565373719f, 0.564894021f, 0.151573971f},
            LossShelf{0.576047182f, 0.575548828f, 0.151596025f},
        }}},
        // MIDI 30; global; p95 T60 realization error 0.901%
        LossPreset{30, 46.2493028389543, 0.71603221279778084, 0.71603473319048061, {{
            LossShelf{0.572652698f, 0.572174609f, 0.165191963f},
            LossShelf{0.582844019f, 0.58235693f, 0.165200919f},
        }}},
        // MIDI 31; global; p95 T60 realization error 0.855%
        LossPreset{31, 48.999429497718666, 0.69589490920829777, 0.69589781890593205, {{
            LossShelf{0.580047488f, 0.579477608f, 0.178995475f},
            LossShelf{0.589761376f, 0.589222431f, 0.178983793f},
        }}},
        // MIDI 32; global; p95 T60 realization error 0.808%
        LossPreset{32, 51.913087197493141, 0.6762882890346601, 0.67629162994047298, {{
            LossShelf{0.587343633f, 0.586869538f, 0.192821503f},
            LossShelf{0.596657455f, 0.596169353f, 0.192826778f},
        }}},
        // MIDI 33; global; p95 T60 realization error 0.757%
        LossPreset{33, 55, 0.65677078335117789, 0.65677460032584234, {{
            LossShelf{0.594782948f, 0.594311178f, 0.206880808f},
            LossShelf{0.603679717f, 0.603197217f, 0.206876919f},
        }}},
        // MIDI 34; global; p95 T60 realization error 0.703%
        LossPreset{34, 58.270470189761241, 0.63765584921914897, 0.63766019039365374, {{
            LossShelf{0.602217615f, 0.601748049f, 0.220966235f},
            LossShelf{0.610726357f, 0.610236168f, 0.220962524f},
        }}},
        // MIDI 35; global; p95 T60 realization error 0.641%
        LossPreset{35, 61.735412657015502, 0.61891035299060482, 0.61891527074279018, {{
            LossShelf{0.609672725f, 0.609206378f, 0.235127836f},
            LossShelf{0.617786109f, 0.617287874f, 0.235073969f},
        }}},
        // MIDI 36; global; p95 T60 realization error 0.604%
        LossPreset{36, 65.406391325149656, 0.46606000189624142, 0.46606333375157766, {{
            LossShelf{0.709973693f, 0.508443058f, 0.233750343f},
            LossShelf{0.706111848f, 0.527553976f, 0.233665884f},
        }}},
        // MIDI 37; global; p95 T60 realization error 0.592%
        LossPreset{37, 69.295657744218019, 0.33795031888478599, 0.3379514202006888, {{
            LossShelf{0.789180994f, 0.390327096f, 0.193514511f},
            LossShelf{0.798136592f, 0.395159841f, 0.193296418f},
        }}},
        // MIDI 38; global; p95 T60 realization error 0.586%
        LossPreset{38, 73.416191979351879, 0.32446968201521958, 0.32447051448061071, {{
            LossShelf{0.641469777f, 0.441583872f, 0.095186308f},
            LossShelf{0.992804527f, 0.972382426f, 0.965186954f},
        }}},
        // MIDI 39; global; p95 T60 realization error 0.584%
        LossPreset{39, 77.781745930520231, 0.26545615584581045, 0.26545572753212443, {{
            LossShelf{0.706958234f, 0.365230888f, 0.0835227966f},
            LossShelf{0.996231437f, 0.986721754f, 0.982953131f},
        }}},
        // MIDI 40; global; p95 T60 realization error 0.573%
        LossPreset{40, 82.406889228217494, 0.23121818140255349, 0.23121689314889357, {{
            LossShelf{0.745844185f, 0.317866057f, 0.0743207335f},
            LossShelf{0.996882737f, 0.988090754f, 0.98497349f},
        }}},
        // MIDI 41; global; p95 T60 realization error 0.388%
        LossPreset{41, 87.307057858250971, 0.20170706983317438, 0.20170534215666122, {{
            LossShelf{0.777087629f, 0.288775146f, 0.0759222656f},
            LossShelf{0.997336626f, 0.987483203f, 0.984819829f},
        }}},
        // MIDI 42; global; p95 T60 realization error 0.378%
        LossPreset{42, 92.4986056779086, 0.18298007617707771, 0.18297774073523468, {{
            LossShelf{0.798847556f, 0.261485577f, 0.0697784498f},
            LossShelf{0.996821523f, 0.988175392f, 0.984996915f},
        }}},
        // MIDI 43; global; p95 T60 realization error 0.363%
        LossPreset{43, 97.998858995437331, 0.16574132548992879, 0.16573837139231343, {{
            LossShelf{0.816609859f, 0.239585862f, 0.0650762916f},
            LossShelf{0.999991238f, 0.983839512f, 0.98383075f},
        }}},
        // MIDI 44; global; p95 T60 realization error 0.351%
        LossPreset{44, 103.82617439498628, 0.15340556480886153, 0.15340213410908995, {{
            LossShelf{0.830932021f, 0.223347992f, 0.0626475513f},
            LossShelf{0.99898237f, 0.986017644f, 0.985000014f},
        }}},
        // MIDI 45; global; p95 T60 realization error 0.342%
        LossPreset{45, 110, 0.14057175715261291, 0.14056772174027904, {{
            LossShelf{0.84485811f, 0.205899343f, 0.0586304404f},
            LossShelf{0.99999243f, 0.985005498f, 0.984997928f},
        }}},
        // MIDI 46; global; p95 T60 realization error 0.361%
        LossPreset{46, 116.54094037952248, 0.13194577695036819, 0.13194142342631751, {{
            LossShelf{0.853956044f, 0.199678212f, 0.0610868856f},
            LossShelf{0.999991536f, 0.98500824f, 0.984999716f},
        }}},
        // MIDI 47; global; p95 T60 realization error 0.434%
        LossPreset{47, 123.47082531403103, 0.122932070583298, 0.12292724873682467, {{
            LossShelf{0.864486277f, 0.188451856f, 0.0599704273f},
            LossShelf{0.998938501f, 0.985935986f, 0.984874487f},
        }}},
        // MIDI 48; global; p95 T60 realization error 0.511%
        LossPreset{48, 130.81278265029931, 0.11726177523485393, 0.11725675281697417, {{
            LossShelf{0.871660352f, 0.185513124f, 0.0638398975f},
            LossShelf{0.996441066f, 0.988557756f, 0.984998822f},
        }}},
        // MIDI 49; global; p95 T60 realization error 0.593%
        LossPreset{49, 138.59131548843604, 0.11023193956390301, 0.11022650456498929, {{
            LossShelf{0.970262229f, 0.0248138309f, 0.00100000005f},
            LossShelf{0.90311116f, 0.219625294f, 0.122736461f},
        }}},
        // MIDI 50; global; p95 T60 realization error 0.680%
        LossPreset{50, 146.83238395870379, 0.10448517554549977, 0.10447932426321985, {{
            LossShelf{0.958856881f, 0.0365479961f, 0.00100000005f},
            LossShelf{0.919616461f, 0.249251649f, 0.168868139f},
        }}},
        // MIDI 51; global; p95 T60 realization error 0.748%
        LossPreset{51, 155.56349186104046, 0.098940300779251977, 0.098934106275699582, {{
            LossShelf{0.957032382f, 0.0386828519f, 0.00100000005f},
            LossShelf{0.927161217f, 0.265097588f, 0.19225882f},
        }}},
        // MIDI 52; global; p95 T60 realization error 0.761%
        LossPreset{52, 164.81377845643496, 0.093876942006254085, 0.093870367412676708, {{
            LossShelf{0.956007719f, 0.0400018543f, 0.00100000005f},
            LossShelf{0.933470905f, 0.282402754f, 0.215873659f},
        }}},
        // MIDI 53; global; p95 T60 realization error 0.944%
        LossPreset{53, 174.61411571650194, 0.088216497057697338, 0.088209468102949093, {{
            LossShelf{0.956085384f, 0.0401989892f, 0.00100000005f},
            LossShelf{0.939637244f, 0.295529902f, 0.235167161f},
        }}},
        // MIDI 54; global; p95 T60 realization error 1.643%
        LossPreset{54, 184.9972113558172, 0.081874532355640517, 0.081866728552633966, {{
            LossShelf{0.948426247f, 0.048101902f, 0.00100000005f},
            LossShelf{0.953352094f, 0.394542992f, 0.347895116f},
        }}},
        // MIDI 55; global; p95 T60 realization error 0.806%
        LossPreset{55, 195.99771799087463, 0.079403861189346711, 0.079395778395922129, {{
            LossShelf{0.956778407f, 0.0400209539f, 0.00100000005f},
            LossShelf{0.948613226f, 0.32784009f, 0.276453346f},
        }}},
        // MIDI 56; global; p95 T60 realization error 1.107%
        LossPreset{56, 207.65234878997256, 0.074875933368343084, 0.074867620877405297, {{
            LossShelf{0.948016644f, 0.0731995255f, 0.0252859648f},
            LossShelf{0.962163746f, 0.395742744f, 0.357906491f},
        }}},
        // MIDI 57; global; p95 T60 realization error 1.902%
        LossPreset{57, 220, 0.068620238112223605, 0.068610706070286867, {{
            LossShelf{0.955820799f, 0.0414119139f, 0.00100000005f},
            LossShelf{0.961351275f, 0.414186478f, 0.375537753f},
        }}},
        // MIDI 58; global; p95 T60 realization error 1.408%
        LossPreset{58, 233.08188075904496, 0.067523041778097467, 0.067514426673541947, {{
            LossShelf{0.938335717f, 0.122939162f, 0.0650494993f},
            LossShelf{0.980198801f, 0.571277678f, 0.551476419f},
        }}},
        // MIDI 59; global; p95 T60 realization error 1.488%
        LossPreset{59, 246.94165062806206, 0.06373878282173423, 0.063729639280263137, {{
            LossShelf{0.94180274f, 0.120574035f, 0.0659451708f},
            LossShelf{0.981056929f, 0.582652271f, 0.563709199f},
        }}},
        // MIDI 60; global; p95 T60 realization error 2.298%
        LossPreset{60, 261.62556530059862, 0.056928784569556183, 0.056916971741435812, {{
            LossShelf{0.961704791f, 0.0361174941f, 0.00100000005f},
            LossShelf{0.969245374f, 0.445581794f, 0.414827168f},
        }}},
        // MIDI 61; global; p95 T60 realization error 3.162%
        LossPreset{61, 277.18263097687208, 0.051863156817875206, 0.05184992702763528, {{
            LossShelf{0.962329268f, 0.0356558673f, 0.00100000005f},
            LossShelf{0.974796534f, 0.495570719f, 0.470367283f},
        }}},
        // MIDI 62; global; p95 T60 realization error 1.672%
        LossPreset{62, 293.66476791740757, 0.052558724607467883, 0.052546743102426727, {{
            LossShelf{0.9568578f, 0.0852884501f, 0.0450923853f},
            LossShelf{0.979442894f, 0.502439857f, 0.481882781f},
        }}},
        // MIDI 63; global; p95 T60 realization error 1.752%
        LossPreset{63, 311.12698372208092, 0.049443333938828764, 0.049430413027113804, {{
            LossShelf{0.958348155f, 0.0859873146f, 0.0471233912f},
            LossShelf{0.981708527f, 0.52803278f, 0.509741306f},
        }}},
        // MIDI 64; global; p95 T60 realization error 0.761%
        LossPreset{64, 329.62755691286992, 0.05018487445041131, 0.050172868054033259, {{
            LossShelf{0.952816129f, 0.125764444f, 0.0812796876f},
            LossShelf{0.986026585f, 0.609295309f, 0.595321953f},
        }}},
        // MIDI 65; global; p95 T60 realization error 1.974%
        LossPreset{65, 349.22823143300388, 0.043864353550868297, 0.043849830886102087, {{
            LossShelf{0.964000463f, 0.0780902132f, 0.0445734337f},
            LossShelf{0.982624471f, 0.514371037f, 0.496995509f},
        }}},
        // MIDI 66; global; p95 T60 realization error 1.821%
        LossPreset{66, 369.9944227116344, 0.041445699304470202, 0.041429139620925384, {{
            LossShelf{0.971147656f, 0.0412886031f, 0.0147097185f},
            LossShelf{0.97836256f, 0.448060066f, 0.426422626f},
        }}},
        // MIDI 67; global; p95 T60 realization error 2.022%
        LossPreset{67, 391.99543598174927, 0.038569751150702061, 0.038551444027183399, {{
            LossShelf{0.973727226f, 0.0304478798f, 0.0063054515f},
            LossShelf{0.979394138f, 0.439392835f, 0.418787003f},
        }}},
        // MIDI 68; global; p95 T60 realization error 0.871%
        LossPreset{68, 415.30469757994513, 0.038647813187822573, 0.038630939741655525, {{
            LossShelf{0.965877533f, 0.0969027132f, 0.0648946837f},
            LossShelf{0.987163246f, 0.537800133f, 0.524963379f},
        }}},
        // MIDI 69; global; p95 T60 realization error 2.153%
        LossPreset{69, 440, 0.034213136651369561, 0.034192177687860578, {{
            LossShelf{0.977191687f, 0.0219166316f, 0.00100000005f},
            LossShelf{0.981147885f, 0.437750757f, 0.418898672f},
        }}},
        // MIDI 70; global; p95 T60 realization error 2.031%
        LossPreset{70, 466.16376151808993, 0.031707762542134407, 0.031684544575275947, {{
            LossShelf{0.977934003f, 0.0212783217f, 0.00100000005f},
            LossShelf{0.983647823f, 0.447832286f, 0.43148008f},
        }}},
        // MIDI 71; global; p95 T60 realization error 2.339%
        LossPreset{71, 493.88330125612413, 0.030318248694925849, 0.03029388714829187, {{
            LossShelf{0.979133189f, 0.0201799199f, 0.00100000005f},
            LossShelf{0.983928025f, 0.459363192f, 0.443291217f},
        }}},
        // MIDI 72; global; p95 T60 realization error 1.130%
        LossPreset{72, 523.25113060119725, 0.03068623959455101, 0.030665106224438483, {{
            LossShelf{0.972732961f, 0.0948506445f, 0.069271937f},
            LossShelf{0.989834726f, 0.542254806f, 0.532089531f},
        }}},
        // MIDI 73; global; p95 T60 realization error 1.764%
        LossPreset{73, 554.36526195374415, 0.027104148923911456, 0.027076669008337221, {{
            LossShelf{0.981550395f, 0.0179468431f, 0.00100000005f},
            LossShelf{0.985468745f, 0.444626749f, 0.430095524f},
        }}},
        // MIDI 74; global; p95 T60 realization error 2.059%
        LossPreset{74, 587.32953583481515, 0.025603578913498359, 0.025574067243458867, {{
            LossShelf{0.982260883f, 0.0173193868f, 0.00100000005f},
            LossShelf{0.986480057f, 0.468735278f, 0.455215335f},
        }}},
        // MIDI 75; global; p95 T60 realization error 2.483%
        LossPreset{75, 622.25396744416184, 0.024063781460123581, 0.02403193743460982, {{
            LossShelf{0.982919574f, 0.0167379603f, 0.00100000005f},
            LossShelf{0.987605214f, 0.499471009f, 0.487076193f},
        }}},
        // MIDI 76; global; p95 T60 realization error 1.836%
        LossPreset{76, 659.25511382573984, 0.022281722725889672, 0.022246635253493741, {{
            LossShelf{0.983857989f, 0.0158759598f, 0.00100000005f},
            LossShelf{0.989221215f, 0.463819087f, 0.453040302f},
        }}},
        // MIDI 77; global; p95 T60 realization error 2.560%
        LossPreset{77, 698.45646286600777, 0.02065037867038455, 0.02061290564519579, {{
            LossShelf{0.9846977f, 0.0151007511f, 0.00100000005f},
            LossShelf{0.990332127f, 0.482459813f, 0.47279191f},
        }}},
        // MIDI 78; global; p95 T60 realization error 2.057%
        LossPreset{78, 739.9888454232688, 0.020043458871884919, 0.020004698848767131, {{
            LossShelf{0.98559016f, 0.0142779136f, 0.00100000005f},
            LossShelf{0.989940584f, 0.493299633f, 0.483240217f},
        }}},
        // MIDI 79; global; p95 T60 realization error 2.348%
        LossPreset{79, 783.99087196349853, 0.018325303177284917, 0.018281455438323522, {{
            LossShelf{0.985981226f, 0.0139489817f, 0.00100000005f},
            LossShelf{0.991916418f, 0.506072938f, 0.497989357f},
        }}},
        // MIDI 80; global; p95 T60 realization error 2.481%
        LossPreset{80, 830.60939515989025, 0.017645326943717989, 0.017600578528719345, {{
            LossShelf{0.986990869f, 0.0129980929f, 0.00100000005f},
            LossShelf{0.991523027f, 0.503933728f, 0.495456755f},
        }}},
        // MIDI 81; global; p95 T60 realization error 2.342%
        LossPreset{81, 880, 0.016392722264951056, 0.016343443718932864, {{
            LossShelf{0.987505317f, 0.0125398571f, 0.00100000005f},
            LossShelf{0.992678523f, 0.5085181f, 0.501196623f},
        }}},
        // MIDI 82; global; p95 T60 realization error 20.144%
        LossPreset{82, 932.32752303617985, 0.0094970911571389555, 0.0094607660781394938, {{
            LossShelf{0.991669536f, 0.00827072188f, 0.00100000005f},
            LossShelf{0.996464968f, 0.579506099f, 0.575971127f},
        }}},
        // MIDI 83; global; p95 T60 realization error 2.250%
        LossPreset{83, 987.76660251224826, 0.014365968890251529, 0.014308583761182969, {{
            LossShelf{0.988678813f, 0.0114687765f, 0.00100000005f},
            LossShelf{0.994058967f, 0.518969297f, 0.513028204f},
        }}},
        // MIDI 84; global; p95 T60 realization error 2.225%
        LossPreset{84, 1046.5022612023945, 0.013574570699782013, 0.013514183071725415, {{
            LossShelf{0.989387631f, 0.0108065158f, 0.00100000005f},
            LossShelf{0.994288981f, 0.508853912f, 0.503142893f},
        }}},
        // MIDI 85; global; p95 T60 realization error 2.370%
        LossPreset{85, 1108.7305239074883, 0.01276677222496244, 0.012702772695009284, {{
            LossShelf{0.990014732f, 0.0102229388f, 0.00100000005f},
            LossShelf{0.994642735f, 0.502920568f, 0.497563332f},
        }}},
        // MIDI 86; global; p95 T60 realization error 1.076%
        LossPreset{86, 1174.6590716696303, 0.012941280600085283, 0.012880002647250057, {{
            LossShelf{0.991223514f, 0.00906440616f, 0.00100000005f},
            LossShelf{0.993042648f, 0.424177974f, 0.417220592f},
        }}},
        // MIDI 87; global; p95 T60 realization error 1.883%
        LossPreset{87, 1244.5079348883237, 0.012221098748060795, 0.012160903879548815, {{
            LossShelf{0.989729166f, 0.0512266159f, 0.0416597314f},
            LossShelf{0.995333135f, 0.543007791f, 0.538340986f},
        }}},
        // MIDI 88; global; p95 T60 realization error 69.952%
        LossPreset{88, 1318.5102276514797, 0.0017922484159481496, 0.001798657881747373, {{
            LossShelf{0.996675551f, 0.442208976f, 0.440436333f},
            LossShelf{0.999548852f, 0.985142946f, 0.984691739f},
        }}},
        // MIDI 89; global; p95 T60 realization error 30.988%
        LossPreset{89, 1396.9129257320155, 0.0047712965714784674, 0.0047233363066292609, {{
            LossShelf{0.995141327f, 0.00506559992f, 0.00100000005f},
            LossShelf{0.998724103f, 0.749601364f, 0.748325467f},
        }}},
        // MIDI 90; global; p95 T60 realization error 1.658%
        LossPreset{90, 1479.9776908465376, 0.010812578606363645, 0.010746295946221391, {{
            LossShelf{0.990956485f, 0.0596992671f, 0.0512560122f},
            LossShelf{0.995483577f, 0.628141522f, 0.6236251f},
        }}},
        // MIDI 91; global; p95 T60 realization error 1.057%
        LossPreset{91, 1567.9817439269971, 0.0098216176121961995, 0.0097395917746298487, {{
            LossShelf{0.993366241f, 0.00709735323f, 0.00100000005f},
            LossShelf{0.994622886f, 0.432546169f, 0.427169055f},
        }}},
        // MIDI 92; global; p95 T60 realization error 1.527%
        LossPreset{92, 1661.2187903197805, 0.00902255340091874, 0.0089306525999325211, {{
            LossShelf{0.992716491f, 0.0256401431f, 0.018873984f},
            LossShelf{0.996320605f, 0.527242482f, 0.523563087f},
        }}},
        // MIDI 93; global; p95 T60 realization error 28.820%
        LossPreset{93, 1760, 0.0055945008864219641, 0.005506444387682401, {{
            LossShelf{0.994621933f, 0.0057295477f, 0.00100000005f},
            LossShelf{0.998482108f, 0.669281781f, 0.667763829f},
        }}},
        // MIDI 94; global; p95 T60 realization error 1.350%
        LossPreset{94, 1864.6550460723597, 0.0082852901255033654, 0.0081898274417011689, {{
            LossShelf{0.9935866f, 0.0297955833f, 0.0238480493f},
            LossShelf{0.996219933f, 0.50925529f, 0.505475223f},
        }}},
        // MIDI 95; global; p95 T60 realization error 33.279%
        LossPreset{95, 1975.5332050244961, 0.0055404491964765375, 0.0054367339517353563, {{
            LossShelf{0.994855464f, 0.00554319937f, 0.00100000005f},
            LossShelf{0.998185754f, 0.72871834f, 0.726904094f},
        }}},
        // MIDI 96; global; p95 T60 realization error 2.385%
        LossPreset{96, 2093.004522404789, 0.060685177171099142, 0.060692289086111587, {{
            LossShelf{0.97908479f, 0.0214933921f, 0.00100000005f},
            LossShelf{0.925710559f, 0.922291398f, 0.848001957f},
        }}},
        // MIDI 97; global; p95 T60 realization error 2.467%
        LossPreset{97, 2217.4610478149766, 0.060749840930865204, 0.060774294471047235, {{
            LossShelf{0.980520725f, 0.0200777538f, 0.00100000005f},
            LossShelf{0.923189104f, 0.920074701f, 0.843263745f},
        }}},
        // MIDI 98; global; p95 T60 realization error 12.865%
        LossPreset{98, 2349.3181433392601, 0.016905258493571152, 0.016756016971397454, {{
            LossShelf{0.98256427f, 0.0179850217f, 0.00100000005f},
            LossShelf{0.99999243f, 0.985007584f, 0.985000014f},
        }}},
        // MIDI 99; global; p95 T60 realization error 2.520%
        LossPreset{99, 2489.0158697766474, 0.04549384227279344, 0.045464904972224253, {{
            LossShelf{0.981879115f, 0.018758826f, 0.00100000005f},
            LossShelf{0.947710276f, 0.935872495f, 0.883582771f},
        }}},
        // MIDI 100; global; p95 T60 realization error 22.404%
        LossPreset{100, 2637.0204553029598, 0.014840154729723729, 0.014673957114837209, {{
            LossShelf{0.984619975f, 0.0159272701f, 0.00100000005f},
            LossShelf{0.99999243f, 0.985007584f, 0.985000014f},
        }}},
        // MIDI 101; global; p95 T60 realization error 12.934%
        LossPreset{101, 2793.8258514640311, 0.01452019461730175, 0.014337431487034582, {{
            LossShelf{0.984981537f, 0.015616239f, 0.00100000005f},
            LossShelf{0.99999243f, 0.985007584f, 0.985000014f},
        }}},
        // MIDI 102; global; p95 T60 realization error 32.378%
        LossPreset{102, 2959.9553816930752, 0.010612211603816275, 0.010460440954997697, {{
            LossShelf{0.988836408f, 0.0116895735f, 0.00100000005f},
            LossShelf{0.99999243f, 0.985007584f, 0.985000014f},
        }}},
        // MIDI 103; global; p95 T60 realization error 30.049%
        LossPreset{103, 3135.9634878539946, 0.010844919341281791, 0.01067088188757064, {{
            LossShelf{0.988635302f, 0.0119341016f, 0.00100000005f},
            LossShelf{0.99999243f, 0.985007584f, 0.985000014f},
        }}},
        // MIDI 104; global; p95 T60 realization error 18.126%
        LossPreset{104, 3322.437580639561, 0.011659108656308941, 0.011449516347192946, {{
            LossShelf{0.987855554f, 0.012767286f, 0.00100000005f},
            LossShelf{0.99999243f, 0.985007584f, 0.985000014f},
        }}},
        // MIDI 105; global; p95 T60 realization error 16.822%
        LossPreset{105, 3520, 0.01138980654730897, 0.011159682642585593, {{
            LossShelf{0.988144815f, 0.0125085283f, 0.00100000005f},
            LossShelf{0.99999243f, 0.985007584f, 0.985000014f},
        }}},
        // MIDI 106; global; p95 T60 realization error 3.112%
        LossPreset{106, 3729.3100921447194, 0.01253811026852846, 0.012254592508744652, {{
            LossShelf{0.987052023f, 0.0136855589f, 0.00100000005f},
            LossShelf{0.99999243f, 0.985007584f, 0.985000014f},
        }}},
        // MIDI 107; global; p95 T60 realization error 12.815%
        LossPreset{107, 3951.0664100489921, 0.015514637692152404, 0.0154273199751744, {{
            LossShelf{0.981381297f, 0.19368723f, 0.175470486f},
            LossShelf{0.99999243f, 0.985007584f, 0.985000014f},
        }}},
        // MIDI 108; global; p95 T60 realization error 3.636%
        LossPreset{108, 4186.009044809578, 0.011231121314898345, 0.01090946437169898, {{
            LossShelf{0.988356352f, 0.012397076f, 0.00100000005f},
            LossShelf{0.99999243f, 0.985007584f, 0.985000014f},
        }}},
    }};
};

#endif

#ifndef dispersion_filter_hpp
#define dispersion_filter_hpp

#include <array>
#include <cmath>
#include <cstddef>

// Generated outside the checkout by fit_full_dispersion.py.
// The stored loop delay already includes current loss + dispersion phase.
class DispersionFilter {
  inline static constexpr std::size_t kMaxSections = 4;
  inline static constexpr std::size_t kPresetCount = 237;
  struct SOS {
    float a1;
    float a2;
  };
  struct Preset {
    int midi;
    double referenceF1;
    double B;
    double sampleRate;
    double loopDelaySamples;
    std::size_t sectionCount;
    std::array<SOS, kMaxSections> sections;
  };
  Preset preset_;
  std::array<float, kMaxSections> x1_{};
  std::array<float, kMaxSections> x2_{};
  std::array<float, kMaxSections> y1_{};
  std::array<float, kMaxSections> y2_{};

public:
  explicit DispersionFilter(double frequency) noexcept
      : preset_(lookup(frequency)) {}
  inline void process(float &x) noexcept {
    for (std::size_t i = 0; i < preset_.sectionCount; ++i) {
      const auto &c = preset_.sections[i];
      const float y = x2_[i] + c.a1 * (x1_[i] - y1_[i]) + c.a2 * (x - y2_[i]);
      x2_[i] = x1_[i];
      x1_[i] = x;
      y2_[i] = y1_[i];
      y1_[i] = y;
      x = y;
    }
  }
  inline void system_reset() noexcept {
    x1_.fill(0.0f);
    x2_.fill(0.0f);
    y1_.fill(0.0f);
    y2_.fill(0.0f);
  }
  [[nodiscard]] inline double state_energy() const noexcept {
    double e = 0.0;
    for (std::size_t i = 0; i < preset_.sectionCount; ++i)
      e += double(x1_[i]) * x1_[i] + double(x2_[i]) * x2_[i] +
           double(y1_[i]) * y1_[i] + double(y2_[i]) * y2_[i];
    return e;
  }
  [[nodiscard]] inline double get_loop_delay_samples() const noexcept {
    return preset_.loopDelaySamples;
  }
  [[nodiscard]] inline double design_sample_rate() const noexcept {
    return preset_.sampleRate;
  }

private:
  [[nodiscard]] inline static const Preset &lookup(double f0) noexcept {
    std::size_t best = 0;
    double distance = std::abs(kPresets[0].referenceF1 - f0);
    for (std::size_t i = 1; i < kPresets.size(); ++i) {
      const double candidate = std::abs(kPresets[i].referenceF1 - f0);
      if (candidate < distance) {
        distance = candidate;
        best = i;
      }
    }
    return kPresets[best];
  }
  inline static constexpr std::array<Preset, 237> kPresets = {{
      // string 0, A0_string_000, RMS 0.303336 cent, max 0.579536 cent
      Preset{21,
             27.3782,
             5.4850291759431087e-05,
             44100,
             1541.7305100913354,
             1,
             {{SOS{-1.90728378f, 0.909946561f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 1, A#0_string_001, RMS 0.289625 cent, max 0.554029 cent
      Preset{22,
             29.0101,
             5.3600800116909449e-05,
             44100,
             1455.264909384322,
             1,
             {{SOS{-1.90071738f, 0.903761387f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 2, B0_string_002, RMS 0.278767 cent, max 0.532373 cent
      Preset{23,
             30.7393,
             5.2581980993204519e-05,
             44100,
             1374.6424138246502,
             1,
             {{SOS{-1.8939029f, 0.897369862f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}}}},
      // string 3, C1_string_003, RMS 0.271279 cent, max 0.51593 cent
      Preset{24,
             32.5716,
             5.17816694448717e-05,
             44100,
             1297.3661782899435,
             1,
             {{SOS{-1.88674212f, 0.890680254f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 4, C#1_string_004, RMS 0.265345 cent, max 0.505404 cent
      Preset{25,
             34.5131,
             5.1190427994123997e-05,
             44100,
             1224.671388938563,
             1,
             {{SOS{-1.87943876f, 0.883891225f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 5, D1_string_005, RMS 0.260476 cent, max 0.49552 cent
      Preset{26,
             36.5703,
             5.0801325194743499e-05,
             44100,
             1156.6907988106329,
             1,
             {{SOS{-1.8719455f, 0.87696135f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}}}},
      // string 6, D#1_string_006, RMS 0.258065 cent, max 0.49157 cent
      Preset{27,
             38.7502,
             5.0609819378936862e-05,
             44100,
             1091.6784149089171,
             1,
             {{SOS{-1.86419332f, 0.869826496f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 7, E1_string_007, RMS 0.258056 cent, max 0.493291 cent
      Preset{28,
             41.06,
             5.0613704795535706e-05,
             44100,
             1029.9373047879021,
             1,
             {{SOS{-1.85628712f, 0.862589061f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 8, F1_string_008, RMS 0.259696 cent, max 0.496916 cent
      Preset{29,
             43.5075,
             5.081302372431709e-05,
             44100,
             971.98672922011406,
             1,
             {{SOS{-1.84819758f, 0.855223536f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 9, F#1_string_009, RMS 0.264486 cent, max 0.505085 cent
      Preset{30,
             46.1008,
             5.1210071831366423e-05,
             44100,
             917.11638324109629,
             1,
             {{SOS{-1.84008586f, 0.847881615f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 10, G1_string_010, RMS 0.270218 cent, max 0.517023 cent
      Preset{31,
             48.8248,
             5.180354900715938e-05,
             44100,
             865.80304148471339,
             1,
             {{SOS{-1.83175099f, 0.840378582f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 11, G1_string_011, RMS 0.270727 cent, max 0.517098 cent
      Preset{31,
             48.8488,
             5.1809506247468992e-05,
             44100,
             865.36617035449808,
             1,
             {{SOS{-1.83174014f, 0.840369821f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 12, G#1_string_012, RMS 0.27983 cent, max 0.533993 cent
      Preset{32,
             51.7351,
             5.2610545216462892e-05,
             44100,
             816.5033262965826,
             1,
             {{SOS{-1.82346487f, 0.832967877f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 13, G#1_string_013, RMS 0.279903 cent, max 0.533821 cent
      Preset{32,
             51.7605,
             5.2618306901262375e-05,
             44100,
             816.10064392066192,
             1,
             {{SOS{-1.82339191f, 0.832902849f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 14, A1_string_014, RMS 0.290593 cent, max 0.55597 cent
      Preset{33,
             54.8189,
             5.3636409104809483e-05,
             44100,
             770.97618189053412,
             1,
             {{SOS{-1.81493628f, 0.825384736f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 15, A1_string_015, RMS 0.290853 cent, max 0.556366 cent
      Preset{33,
             54.8458,
             5.3646069605784835e-05,
             44100,
             770.59380513931922,
             1,
             {{SOS{-1.81488287f, 0.825337946f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 16, A#1_string_016, RMS 0.304663 cent, max 0.584989 cent
      Preset{34,
             58.084,
             5.4892370405986803e-05,
             44100,
             727.34574204677892,
             1,
             {{SOS{-1.80638218f, 0.817827463f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 17, A#1_string_017, RMS 0.305146 cent, max 0.584728 cent
      Preset{34,
             58.1126,
             5.4904088538290463e-05,
             44100,
             726.97922677321367,
             1,
             {{SOS{-1.80635965f, 0.817808688f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 18, B1_string_018, RMS 0.322236 cent, max 0.621089 cent
      Preset{35,
             61.5437,
             5.6394372616627355e-05,
             44100,
             685.65388992961891,
             1,
             {{SOS{-1.79784334f, 0.810333908f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 19, B1_string_019, RMS 0.323066 cent, max 0.621722 cent
      Preset{35,
             61.5739,
             5.6408210716939515e-05,
             44100,
             685.3045228564747,
             1,
             {{SOS{-1.79786325f, 0.810353398f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 20, C2_string_020, RMS 0.344641 cent, max 0.663914 cent
      Preset{36,
             65.2094,
             5.8160810311740388e-05,
             44100,
             647.17713547280141,
             1,
             {{SOS{-1.78948474f, 0.803052485f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 21, C2_string_021, RMS 0.345186 cent, max 0.663604 cent
      Preset{36,
             65.2414,
             5.8176980625087649e-05,
             44100,
             646.85176856332339,
             1,
             {{SOS{-1.78946221f, 0.803034186f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 22, C#2_string_022, RMS 0.371761 cent, max 0.714386 cent
      Preset{37,
             69.0934,
             6.0213806022023323e-05,
             44100,
             610.39644925454104,
             1,
             {{SOS{-1.78124762f, 0.795928895f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 23, C#2_string_023, RMS 0.371965 cent, max 0.714723 cent
      Preset{37,
             69.1273,
             6.0232509095508895e-05,
             44100,
             610.09456927639189,
             1,
             {{SOS{-1.78117132f, 0.795863211f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 24, D2_string_024, RMS 0.402342 cent, max 0.778582 cent
      Preset{38,
             73.2088,
             6.2579636542420321e-05,
             44100,
             575.69150124584189,
             1,
             {{SOS{-1.77267659f, 0.788554311f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 25, D2_string_025, RMS 0.403731 cent, max 0.779185 cent
      Preset{38,
             73.2447,
             6.2601105219795922e-05,
             44100,
             575.39312308055673,
             1,
             {{SOS{-1.77275443f, 0.788625002f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 26, D2_string_026, RMS 0.40474 cent, max 0.779172 cent
      Preset{38,
             73.2807,
             6.2622647886681856e-05,
             44100,
             575.09871134776449,
             1,
             {{SOS{-1.77278173f, 0.788651168f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 27, D#2_string_027, RMS 0.441168 cent, max 0.851583 cent
      Preset{39,
             77.5692,
             6.5289084468286726e-05,
             44100,
             542.85526282450405,
             1,
             {{SOS{-1.76506317f, 0.782082379f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 28, D#2_string_028, RMS 0.441769 cent, max 0.851666 cent
      Preset{39,
             77.6074,
             6.5313716505639545e-05,
             44100,
             542.58205968392144,
             1,
             {{SOS{-1.76502848f, 0.782054305f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 29, D#2_string_029, RMS 0.442158 cent, max 0.851687 cent
      Preset{39,
             77.6455,
             6.5338299358686647e-05,
             44100,
             542.31181894593988,
             1,
             {{SOS{-1.76496768f, 0.782003224f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 30, E2_string_030, RMS 0.48156 cent, max 0.939626 cent
      Preset{40,
             82.1895,
             6.8378564983695274e-05,
             44100,
             511.7028347978403,
             1,
             {{SOS{-1.75697994f, 0.775241733f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 31, E2_string_031, RMS 0.483538 cent, max 0.940713 cent
      Preset{40,
             82.2299,
             6.8406550346842109e-05,
             44100,
             511.43316165908902,
             1,
             {{SOS{-1.7571075f, 0.775355577f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}}}},
      // string 32, E2_string_032, RMS 0.485025 cent, max 0.940987 cent
      Preset{40,
             82.2702,
             6.8434483060773974e-05,
             44100,
             511.16892606758256,
             1,
             {{SOS{-1.75717556f, 0.775417507f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 33, F2_string_033, RMS 0.0257284 cent, max 0.0400841 cent
      Preset{41,
             87.0849,
             7.1890266102344605e-05,
             44100,
             467.63522064770319,
             2,
             {{SOS{-1.71961462f, 0.758091688f}, SOS{-1.74344909f, 0.761718035f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 34, F2_string_034, RMS 0.025903 cent, max 0.0404687 cent
      Preset{41,
             87.1277,
             7.1922034626807979e-05,
             44100,
             467.38034294030035,
             2,
             {{SOS{-1.74358439f, 0.761837125f}, SOS{-1.71972156f, 0.758194506f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 35, F2_string_035, RMS 0.0260294 cent, max 0.0406954 cent
      Preset{41,
             87.1705,
             7.1953821532202286e-05,
             44100,
             467.13208985764447,
             2,
             {{SOS{-1.74365556f, 0.761900365f}, SOS{-1.71976578f, 0.758244991f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 36, F#2_string_036, RMS 0.0310119 cent, max 0.0485944 cent
      Preset{42,
             92.2719,
             7.5873719965610862e-05,
             44100,
             440.54259396528943,
             2,
             {{SOS{-1.73672414f, 0.756019533f}, SOS{-1.71059048f, 0.7519328f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 37, F#2_string_037, RMS 0.0310624 cent, max 0.048718 cent
      Preset{42,
             92.3172,
             7.5909690237425587e-05,
             44100,
             440.32054014149577,
             2,
             {{SOS{-1.73666394f, 0.755968809f}, SOS{-1.71050513f, 0.751875341f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 38, F#2_string_038, RMS 0.0310946 cent, max 0.04877 cent
      Preset{42,
             92.3625,
             7.594568091914677e-05,
             44100,
             440.10007447267259,
             2,
             {{SOS{-1.71040261f, 0.751802862f}, SOS{-1.73658466f, 0.755901456f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 39, G2_string_039, RMS 0.0374536 cent, max 0.0588096 cent
      Preset{43,
             97.7678,
             8.0386576743260001e-05,
             44100,
             415.00789633402809,
             2,
             {{SOS{-1.72976196f, 0.750146508f}, SOS{-1.70123148f, 0.745624542f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 40, G2_string_040, RMS 0.0375887 cent, max 0.0589602 cent
      Preset{43,
             97.8158,
             8.0427313370761084e-05,
             44100,
             414.79181753560545,
             2,
             {{SOS{-1.72978604f, 0.750168979f}, SOS{-1.70122385f, 0.745634556f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 41, G2_string_041, RMS 0.0376837 cent, max 0.0589433 cent
      Preset{43,
             97.8639,
             8.0468157850107554e-05,
             44100,
             414.57884517223556,
             2,
             {{SOS{-1.72976816f, 0.750155151f}, SOS{-1.70117569f, 0.745610416f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 42, G#2_string_042, RMS 0.0456337 cent, max 0.0724291 cent
      Preset{44,
             103.591,
             8.5496148564465589e-05,
             44100,
             390.76785299239674,
             2,
             {{SOS{-1.72403705f, 0.74538517f}, SOS{-1.69254625f, 0.740105987f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 43, G#2_string_043, RMS 0.0457557 cent, max 0.0727027 cent
      Preset{44,
             103.642,
             8.5542394832813175e-05,
             44100,
             390.56648219051255,
             2,
             {{SOS{-1.69250095f, 0.740086138f}, SOS{-1.72402477f, 0.745376408f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 44, G#2_string_044, RMS 0.04584 cent, max 0.0725804 cent
      Preset{44,
             103.693,
             8.5588667164501184e-05,
             44100,
             390.36759668382945,
             2,
             {{SOS{-1.6924243f, 0.740039587f}, SOS{-1.72397971f, 0.745339334f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 45, A2_string_045, RMS 0.0569123 cent, max 0.0919153 cent
      Preset{45,
             109.761,
             9.1281010195021513e-05,
             44100,
             368.06996703882902,
             2,
             {{SOS{-1.71835983f, 0.740683317f}, SOS{-1.68398094f, 0.734822631f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 46, A2_string_046, RMS 0.0551549 cent, max 0.0915537 cent
      Preset{45,
             109.815,
             9.133333879731857e-05,
             44100,
             367.98798158442258,
             2,
             {{SOS{-1.71672058f, 0.739276588f}, SOS{-1.68243909f, 0.733532131f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 47, A2_string_047, RMS 0.0558061 cent, max 0.0918669 cent
      Preset{45,
             109.869,
             9.1385697045794626e-05,
             44100,
             367.76400986847068,
             2,
             {{SOS{-1.71716464f, 0.739658654f}, SOS{-1.68283749f, 0.733879328f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 48, A#2_string_048, RMS 0.070916 cent, max 0.115992 cent
      Preset{46,
             116.295,
             9.7829378225853023e-05,
             44100,
             346.51591724918137,
             2,
             {{SOS{-1.7135973f, 0.736794472f}, SOS{-1.6760515f, 0.730251372f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 49, A#2_string_049, RMS 0.0710526 cent, max 0.115961 cent
      Preset{46,
             116.352,
             9.7888438760023548e-05,
             44100,
             346.33941344991752,
             2,
             {{SOS{-1.71356142f, 0.736765802f}, SOS{-1.67597783f, 0.730211318f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 50, A#2_string_050, RMS 0.0711617 cent, max 0.116422 cent
      Preset{46,
             116.41,
             9.7948570054725719e-05,
             44100,
             346.16153923271332,
             2,
             {{SOS{-1.71349943f, 0.73671484f}, SOS{-1.67587984f, 0.73015058f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 51, B2_string_051, RMS 0.0896014 cent, max 0.149265 cent
      Preset{47,
             123.218,
             0.00010525133288627102,
             44100,
             326.15791582683482,
             2,
             {{SOS{-1.70950055f, 0.73349911f}, SOS{-1.66855502f, 0.726238549f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 52, B2_string_052, RMS 0.0889452 cent, max 0.15148 cent
      Preset{47,
             123.279,
             0.0001053189775995625,
             44100,
             326.00006034973956,
             2,
             {{SOS{-1.70898354f, 0.733052075f}, SOS{-1.66824913f, 0.72594434f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 53, B2_string_053, RMS 0.0866355 cent, max 0.149782 cent
      Preset{47,
             123.339,
             0.00010538555204720589,
             44100,
             325.93954192659555,
             2,
             {{SOS{-1.70741618f, 0.731720984f}, SOS{-1.66655993f, 0.724586189f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 54, C3_string_054, RMS 0.112518 cent, max 0.194317 cent
      Preset{48,
             130.553,
             0.00011367230448717747,
             44100,
             306.92120419999026,
             2,
             {{SOS{-1.7061435f, 0.730855882f}, SOS{-1.66162205f, 0.722863913f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 55, C3_string_055, RMS 0.112983 cent, max 0.194249 cent
      Preset{48,
             130.617,
             0.00011374835315072723,
             44100,
             306.75740024921924,
             2,
             {{SOS{-1.70623505f, 0.730936348f}, SOS{-1.66166723f, 0.722928584f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 56, C3_string_056, RMS 0.113354 cent, max 0.19434 cent
      Preset{48,
             130.682,
             0.00011382563612761529,
             44100,
             306.59420970337823,
             2,
             {{SOS{-1.70627701f, 0.730974615f}, SOS{-1.66166234f, 0.722951889f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 57, C#3_string_057, RMS 0.144373 cent, max 0.253708 cent
      Preset{49,
             138.325,
             0.00012324025444501751,
             44100,
             288.72989675985536,
             2,
             {{SOS{-1.70381606f, 0.7291044f}, SOS{-1.65560985f, 0.720463634f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 58, C#3_string_058, RMS 0.144841 cent, max 0.253743 cent
      Preset{49,
             138.393,
             0.00012332696434276942,
             44100,
             288.57756617350378,
             2,
             {{SOS{-1.7038666f, 0.729150057f}, SOS{-1.65561128f, 0.720494568f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 59, C#3_string_059, RMS 0.145209 cent, max 0.254043 cent
      Preset{49,
             138.461,
             0.00012341372685106358,
             44100,
             288.42759507611714,
             2,
             {{SOS{-1.65557218f, 0.720491052f}, SOS{-1.70387566f, 0.729160428f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 60, D3_string_060, RMS 0.18075 cent, max 0.334292 cent
      Preset{50,
             146.559,
             0.00013412721454709035,
             44100,
             271.6302366314066,
             2,
             {{SOS{-1.70046377f, 0.726493001f}, SOS{-1.64856255f, 0.717350602f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 61, D3_string_061, RMS 0.182699 cent, max 0.335727 cent
      Preset{50,
             146.631,
             0.00013422590355159281,
             44100,
             271.46127700433595,
             2,
             {{SOS{-1.70097804f, 0.726932049f}, SOS{-1.64903319f, 0.717769146f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 62, D3_string_062, RMS 0.184354 cent, max 0.336906 cent
      Preset{50,
             146.703,
             0.00013432465389000837,
             44100,
             271.29872031551616,
             2,
             {{SOS{-1.70138562f, 0.727280855f}, SOS{-1.64939523f, 0.71810025f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 63, D#3_string_063, RMS 0.237475 cent, max 0.445796 cent
      Preset{51,
             155.284,
             0.00014653930235788255,
             44100,
             255.34876719705056,
             2,
             {{SOS{-1.70043147f, 0.72671622f}, SOS{-1.6447041f, 0.717114031f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 64, D#3_string_064, RMS 0.239452 cent, max 0.4474 cent
      Preset{51,
             155.36,
             0.00014665148916490561,
             44100,
             255.19747770911891,
             2,
             {{SOS{-1.70081687f, 0.72704643f}, SOS{-1.6450429f, 0.717430353f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 65, D#3_string_065, RMS 0.241107 cent, max 0.448145 cent
      Preset{51,
             155.436,
             0.00014676374729231839,
             44100,
             255.05086523647654,
             2,
             {{SOS{-1.70111585f, 0.727303326f}, SOS{-1.6452924f, 0.717673898f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 66, E3_string_066, RMS 0.209884 cent, max 0.364883 cent
      Preset{52,
             164.528,
             0.00016071595204200263,
             44100,
             240.26494508485547,
             2,
             {{SOS{-1.68809927f, 0.716412425f}, SOS{-1.62984002f, 0.706115425f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 67, E3_string_067, RMS 0.210224 cent, max 0.364908 cent
      Preset{52,
             164.609,
             0.00016084498160109391,
             44100,
             240.14081229761734,
             2,
             {{SOS{-1.68806255f, 0.716384232f}, SOS{-1.62975192f, 0.706074893f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 68, E3_string_068, RMS 0.210502 cent, max 0.365102 cent
      Preset{52,
             164.689,
             0.00016097250147062826,
             44100,
             240.01891853813606,
             2,
             {{SOS{-1.68800771f, 0.716340601f}, SOS{-1.62964988f, 0.706020772f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 69, F3_string_069, RMS 0.401601 cent, max 0.800494 cent
      Preset{53,
             174.322,
             0.00017694240546449472,
             44100,
             225.52405521060365,
             2,
             {{SOS{-1.70074975f, 0.727513492f}, SOS{-1.63737762f, 0.717504442f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 70, F3_string_070, RMS 0.405954 cent, max 0.803333 cent
      Preset{53,
             174.408,
             0.00017709056309902615,
             44100,
             225.3776672269214,
             2,
             {{SOS{-1.70137f, 0.728043079f}, SOS{-1.63795972f, 0.718029499f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 71, F3_string_071, RMS 0.40969 cent, max 0.806067 cent
      Preset{53,
             174.493,
             0.0001772370966770329,
             44100,
             225.23866630124314,
             2,
             {{SOS{-1.70187414f, 0.728474557f}, SOS{-1.63842142f, 0.718456686f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 72, F#3_string_072, RMS 0.0602271 cent, max 0.12049 cent
      Preset{54,
             184.699,
             0.0001951909802984074,
             44100,
             203.9999694512602,
             3,
             {{SOS{-1.68527603f, 0.71306771f}, SOS{-1.64684045f, 0.707494318f},
               SOS{-1.53819752f, 0.681497335f}, SOS{0.0f, 0.0f}}}},
      // string 73, F#3_string_073, RMS 0.0556948 cent, max 0.0939756 cent
      Preset{54,
             184.79,
             0.00019535414519352071,
             44100,
             203.90586837843782,
             3,
             {{SOS{-1.68091476f, 0.709328413f}, SOS{-1.64433408f, 0.704797864f},
               SOS{-1.53855598f, 0.680709898f}, SOS{0.0f, 0.0f}}}},
      // string 74, F#3_string_074, RMS 0.0569624 cent, max 0.0969018 cent
      Preset{54,
             184.881,
             0.00019551736416016424,
             44100,
             203.73809291804122,
             3,
             {{SOS{-1.68201184f, 0.710252106f}, SOS{-1.64541388f, 0.705712736f},
               SOS{-1.53948498f, 0.681490779f}, SOS{0.0f, 0.0f}}}},
      // string 75, G3_string_075, RMS 0.121181 cent, max 0.188393 cent
      Preset{55,
             195.694,
             0.00021529375640180309,
             44100,
             200.09579898333064,
             2,
             {{SOS{-1.64323115f, 0.679390848f}, SOS{-1.57884014f, 0.66754061f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 76, G3_string_076, RMS 0.405968 cent, max 0.806521 cent
      Preset{55,
             195.79,
             0.00021547270321706786,
             44100,
             199.99988417289157,
             2,
             {{SOS{-1.64257419f, 0.67884165f}, SOS{-1.57848203f, 0.667183518f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 77, G3_string_077, RMS 0.404257 cent, max 0.806931 cent
      Preset{55,
             195.886,
             0.00021565170887217493,
             44100,
             199.99992166490415,
             2,
             {{SOS{-1.63336277f, 0.671094298f}, SOS{-1.57512319f, 0.662976563f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 78, G#3_string_078, RMS 0.406967 cent, max 0.759406 cent
      Preset{56,
             207.344,
             0.00023743618266172512,
             44100,
             187.3630276126153,
             2,
             {{SOS{-1.67523205f, 0.706587732f}, SOS{-1.60162377f, 0.694542706f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 79, G#3_string_079, RMS 0.410834 cent, max 0.76198 cent
      Preset{56,
             207.445,
             0.00023763187926342927,
             44100,
             187.24404108287766,
             2,
             {{SOS{-1.60212827f, 0.695002556f}, SOS{-1.67578137f, 0.70705086f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 80, G#3_string_080, RMS 0.414254 cent, max 0.763687 cent
      Preset{56,
             207.547,
             0.00023782957807628698,
             44100,
             187.12795321240651,
             2,
             {{SOS{-1.67624247f, 0.707440495f}, SOS{-1.60253906f, 0.695388138f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 81, A3_string_081, RMS 0.0575796 cent, max 0.0969481 cent
      Preset{57,
             219.687,
             0.00026181988958400476,
             44100,
             168.53956139982969,
             3,
             {{SOS{-1.65688801f, 0.689693034f}, SOS{-1.61376131f, 0.683596373f},
               SOS{-1.49376166f, 0.657053471f}, SOS{0.0f, 0.0f}}}},
      // string 82, A3_string_082, RMS 0.0577836 cent, max 0.0973579 cent
      Preset{57,
             219.794,
             0.00026203536410030967,
             44100,
             168.44816431987141,
             3,
             {{SOS{-1.65694106f, 0.689739704f}, SOS{-1.61377382f, 0.683634996f},
               SOS{-1.49365652f, 0.657052994f}, SOS{0.0f, 0.0f}}}},
      // string 83, A3_string_083, RMS 0.057952 cent, max 0.0976922 cent
      Preset{57,
             219.902,
             0.00026225292323324228,
             44100,
             168.35743312568883,
             3,
             {{SOS{-1.61374927f, 0.683642805f}, SOS{-1.49351573f, 0.657023668f},
               SOS{-1.65695775f, 0.689756215f}, SOS{0.0f, 0.0f}}}},
      // string 84, A#3_string_084, RMS 0.00957085 cent, max 0.0153135 cent
      Preset{58,
             232.768,
             0.00028867597150438904,
             44100,
             158.92431841039709,
             3,
             {{SOS{-1.59445465f, 0.638638198f}, SOS{-1.55410087f, 0.632223368f},
               SOS{-1.45113933f, 0.613824725f}, SOS{0.0f, 0.0f}}}},
      // string 85, A#3_string_085, RMS 0.00964078 cent, max 0.0154433 cent
      Preset{58,
             232.882,
             0.00028891453989226564,
             44100,
             158.83001858622475,
             3,
             {{SOS{-1.59466314f, 0.638807058f}, SOS{-1.55426526f, 0.632377625f},
               SOS{-1.45120609f, 0.613937795f}, SOS{0.0f, 0.0f}}}},
      // string 86, A#3_string_086, RMS 0.161757 cent, max 0.27161 cent
      Preset{58,
             232.997,
             0.00028915527950406771,
             44100,
             165.99998973351111,
             2,
             {{SOS{-1.62173986f, 0.662646532f}, SOS{-1.54332674f, 0.647634983f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 87, B3_string_087, RMS 0.125595 cent, max 0.201106 cent
      Preset{59,
             246.628,
             0.00031824458244894441,
             44100,
             156.11193406127686,
             2,
             {{SOS{-1.60553157f, 0.649590433f}, SOS{-1.52692068f, 0.634756386f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 88, B3_string_088, RMS 0.125741 cent, max 0.201368 cent
      Preset{59,
             246.749,
             0.00031850768825534171,
             44100,
             156.03149962532618,
             2,
             {{SOS{-1.60544384f, 0.649523377f}, SOS{-1.5267669f, 0.634672523f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 89, B3_string_089, RMS 0.00567654 cent, max 0.009136 cent
      Preset{59,
             246.871,
             0.00031877305486527742,
             44100,
             149.09717087616855,
             3,
             {{SOS{-1.57424152f, 0.622724235f}, SOS{-1.53125119f, 0.614853621f},
               SOS{-1.42772758f, 0.597330272f}, SOS{0.0f, 0.0f}}}},
      // string 90, C4_string_090, RMS 0.172845 cent, max 0.26676 cent
      Preset{60,
             261.314,
             0.00035079667538140852,
             44100,
             138.5691297424554,
             3,
             {{SOS{-1.66556501f, 0.697691381f}, SOS{-1.61332083f, 0.691656649f},
               SOS{-1.461689f, 0.658281744f}, SOS{0.0f, 0.0f}}}},
      // string 91, C4_string_091, RMS 0.173515 cent, max 0.26684 cent
      Preset{60,
             261.442,
             0.0003510858324722655,
             44100,
             138.49168762457796,
             3,
             {{SOS{-1.66567636f, 0.697787702f}, SOS{-1.61338758f, 0.691751063f},
               SOS{-1.46159673f, 0.658326507f}, SOS{0.0f, 0.0f}}}},
      // string 92, C4_string_092, RMS 0.174072 cent, max 0.267255 cent
      Preset{60,
             261.571,
             0.00035137734297986819,
             44100,
             138.4151738964589,
             3,
             {{SOS{-1.66574478f, 0.69784826f}, SOS{-1.61340928f, 0.691807687f},
               SOS{-1.46146131f, 0.658335209f}, SOS{0.0f, 0.0f}}}},
      // string 93, C#4_string_093, RMS 0.266235 cent, max 0.539119 cent
      Preset{61,
             276.874,
             0.00038662562228112331,
             44100,
             130.00001382085082,
             3,
             {{SOS{-1.59736741f, 0.673822582f}, SOS{-1.4578017f, 0.655451834f},
               SOS{-1.61741745f, 0.657155693f}, SOS{0.0f, 0.0f}}}},
      // string 94, C#4_string_094, RMS 0.026769 cent, max 0.0525883 cent
      Preset{
          61,
          277.01,
          0.00038694476329759826,
          44100,
          124.09310649079725,
          4,
          {{SOS{-1.63671756f, 0.672853768f}, SOS{-1.5985738f, 0.668106735f},
            SOS{-1.50252497f, 0.64973855f}, SOS{-1.32154179f, 0.611172438f}}}},
      // string 95, C#4_string_095, RMS 0.211685 cent, max 0.394605 cent
      Preset{61,
             277.146,
             0.00038726400718821016,
             44100,
             129.9530814447281,
             3,
             {{SOS{-1.65810633f, 0.69178021f}, SOS{-1.60251451f, 0.6858567f},
               SOS{-1.44025612f, 0.651507139f}, SOS{0.0f, 0.0f}}}},
      // string 96, D4_string_096, RMS 0.156115 cent, max 0.240728 cent
      Preset{62,
             293.361,
             0.00042605830918566789,
             44100,
             129.35295333954386,
             2,
             {{SOS{-1.57941461f, 0.629579604f}, SOS{-1.48713899f, 0.612355947f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 97, D4_string_097, RMS 0.157757 cent, max 0.243908 cent
      Preset{62,
             293.505,
             0.00042640927717448165,
             44100,
             129.26582510990727,
             2,
             {{SOS{-1.5799855f, 0.63003397f}, SOS{-1.48766851f, 0.612800539f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 98, D4_string_098, RMS 0.159241 cent, max 0.246783 cent
      Preset{62,
             293.649,
             0.0004267603578166185,
             44100,
             129.18146586011756,
             2,
             {{SOS{-1.58047521f, 0.630424619f}, SOS{-1.48811162f, 0.613182008f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 99, D#4_string_099, RMS 0.00265012 cent, max 0.00545429 cent
      Preset{63,
             310.829,
             0.00046944827536626285,
             44100,
             116.00000438657698,
             3,
             {{SOS{-1.45243406f, 0.530127525f}, SOS{-1.42921555f, 0.534075439f},
               SOS{-1.34516895f, 0.535162151f}, SOS{0.0f, 0.0f}}}},
      // string 100, D#4_string_100, RMS 0.0528508 cent, max 0.0948257 cent
      Preset{63,
             310.982,
             0.00046983553170952373,
             44100,
             121.99993500479322,
             2,
             {{SOS{-1.53170836f, 0.592331111f}, SOS{-1.4398011f, 0.573955417f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 101, D#4_string_101, RMS 0.0450923 cent, max 0.0675585 cent
      Preset{63,
             311.134,
             0.00047022037995252336,
             44100,
             121.99319178197217,
             2,
             {{SOS{-1.52481818f, 0.586901307f}, SOS{-1.43683302f, 0.570421636f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 102, E4_string_102, RMS 0.468553 cent, max 0.609696 cent
      Preset{64,
             329.337,
             0.00051718777559689097,
             44100,
             122.20711562041862,
             1,
             {{SOS{-1.49957097f, 0.57270956f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}}}},
      // string 103, E4_string_103, RMS 0.468972 cent, max 0.610323 cent
      Preset{64,
             329.499,
             0.00051761353584727128,
             44100,
             122.14478925812629,
             1,
             {{SOS{-1.49944794f, 0.572623312f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 104, E4_string_104, RMS 0.469347 cent, max 0.610817 cent
      Preset{64,
             329.661,
             0.00051803943206598496,
             44100,
             122.08262930576529,
             1,
             {{SOS{-1.49931812f, 0.572531819f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 105, F4_string_105, RMS 0.192525 cent, max 0.271998 cent
      Preset{65,
             348.948,
             0.00056970865048497213,
             44100,
             106.9192410536036,
             2,
             {{SOS{-1.55440784f, 0.610868633f}, SOS{-1.44662583f, 0.590984166f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 106, F4_string_106, RMS 0.193667 cent, max 0.274068 cent
      Preset{65,
             349.119,
             0.00057017523572402582,
             44100,
             106.85493509045318,
             2,
             {{SOS{-1.55466354f, 0.61107254f}, SOS{-1.44680882f, 0.591177285f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 107, F4_string_107, RMS 0.194697 cent, max 0.276 cent
      Preset{65,
             349.291,
             0.00057064469874168445,
             44100,
             106.79162289241843,
             2,
             {{SOS{-1.55487216f, 0.61123997f}, SOS{-1.44694018f, 0.591332257f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 108, F#4_string_108, RMS 0.122554 cent, max 0.180908 cent
      Preset{66,
             369.726,
             0.00062747715154224781,
             44100,
             100.50569474405552,
             2,
             {{SOS{-1.52664959f, 0.589478195f}, SOS{-1.41796839f, 0.569149554f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 109, F#4_string_109, RMS 0.122841 cent, max 0.181411 cent
      Preset{66,
             369.908,
             0.00062799265582099805,
             44100,
             100.45112288604224,
             2,
             {{SOS{-1.52663231f, 0.589469731f}, SOS{-1.41786146f, 0.569120705f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 110, F#4_string_110, RMS 0.123091 cent, max 0.181874 cent
      Preset{66,
             370.09,
             0.00062850832366120868,
             44100,
             100.39715358720066,
             2,
             {{SOS{-1.52659214f, 0.589443803f}, SOS{-1.41773021f, 0.569073379f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 111, G4_string_111, RMS 0.00285912 cent, max 0.00471064 cent
      Preset{
          67,
          391.742,
          0.00069101348515588257,
          44100,
          84.042107012581454,
          4,
          {{SOS{-1.37467241f, 0.473442465f}, SOS{-1.41170871f, 0.515360773f},
            SOS{-1.33792949f, 0.516312838f}, SOS{-1.16216171f, 0.490651101f}}}},
      // string 112, G4_string_112, RMS 0.328742 cent, max 0.557564 cent
      Preset{67,
             391.934,
             0.00069157794011855437,
             44100,
             94.000629782208435,
             2,
             {{SOS{-1.53065419f, 0.592750967f}, SOS{-1.424667f, 0.580176115f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 113, G4_string_113, RMS 0.347536 cent, max 0.800163 cent
      Preset{67,
             392.127,
             0.0006921455140092512,
             44100,
             87.999996408722282,
             3,
             {{SOS{-1.56325436f, 0.616463542f}, SOS{-1.49227667f, 0.606119335f},
               SOS{-1.3159287f, 0.583127975f}, SOS{0.0f, 0.0f}}}},
      // string 114, G#4_string_114, RMS 0.327126 cent, max 0.48476 cent
      Preset{68,
             415.068,
             0.00076087878767108698,
             44100,
             95.999984059809321,
             1,
             {{SOS{-1.43051875f, 0.523545504f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 115, G#4_string_115, RMS 0.278979 cent, max 0.420003 cent
      Preset{68,
             415.272,
             0.00076150118321163189,
             44100,
             95.999985547659207,
             1,
             {{SOS{-1.42509258f, 0.519396424f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 116, G#4_string_116, RMS 0.240831 cent, max 0.346775 cent
      Preset{68,
             415.476,
             0.00076212377448834864,
             44100,
             95.99997995382931,
             1,
             {{SOS{-1.4207859f, 0.516199052f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}}}},
      // string 117, A4_string_117, RMS 0.292297 cent, max 0.453262 cent
      Preset{69,
             439.784,
             0.00083769948617548108,
             44100,
             82.680025255656361,
             2,
             {{SOS{-1.5217613f, 0.587205887f}, SOS{-1.38840675f, 0.56369406f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 118, A4_string_118, RMS 0.293425 cent, max 0.455499 cent
      Preset{69,
             440,
             0.00083838330327871036,
             44100,
             82.632238133499257,
             2,
             {{SOS{-1.52188933f, 0.587310255f}, SOS{-1.38842547f, 0.563779712f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 119, A4_string_119, RMS 0.294423 cent, max 0.457424 cent
      Preset{69,
             440.216,
             0.00083906733450199965,
             44100,
             82.585183566179964,
             2,
             {{SOS{-1.52198219f, 0.587387621f}, SOS{-1.38840663f, 0.563836813f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 120, A#4_string_120, RMS 0.0193325 cent, max 0.0380325 cent
      Preset{70,
             465.994,
             0.00092222624045597843,
             44100,
             72.843882909627879,
             3,
             {{SOS{-1.41895306f, 0.507745922f}, SOS{-1.35627747f, 0.499199718f},
               SOS{-1.19910359f, 0.479445487f}, SOS{0.0f, 0.0f}}}},
      // string 121, A#4_string_121, RMS 0.0193414 cent, max 0.0380186 cent
      Preset{70,
             466.223,
             0.0009229784278811439,
             44100,
             72.795308617155314,
             3,
             {{SOS{-1.41943979f, 0.508101642f}, SOS{-1.35654366f, 0.499460816f},
               SOS{-1.19913054f, 0.479602635f}, SOS{0.0f, 0.0f}}}},
      // string 122, A#4_string_122, RMS 0.147035 cent, max 0.255843 cent
      Preset{70,
             466.452,
             0.00092373085009990236,
             44100,
             77.999436553685769,
             2,
             {{SOS{-1.4712956f, 0.549401104f}, SOS{-1.33555019f, 0.525074363f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 123, B4_string_123, RMS 0.227632 cent, max 0.374794 cent
      Preset{71,
             493.767,
             0.0010151496048357825,
             44100,
             72.760638514413927,
             2,
             {{SOS{-1.48457658f, 0.559686422f}, SOS{-1.34177971f, 0.534435153f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 124, B4_string_124, RMS 0.228712 cent, max 0.376877 cent
      Preset{71,
             494.01,
             0.0010159776288841188,
             44100,
             72.716769766165967,
             2,
             {{SOS{-1.48476493f, 0.559833646f}, SOS{-1.34186065f, 0.534566283f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 125, B4_string_125, RMS 0.229685 cent, max 0.378784 cent
      Preset{71,
             494.252,
             0.0010168025017225687,
             44100,
             72.673856364473664,
             2,
             {{SOS{-1.48491418f, 0.559951723f}, SOS{-1.34189999f, 0.534666657f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 126, C5_string_126, RMS 0.251286 cent, max 0.326529 cent
      Preset{72,
             523.195,
             0.0011172856755547671,
             44100,
             75.066398476934694,
             1,
             {{SOS{-1.36788511f, 0.480399638f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 127, C5_string_127, RMS 0.2525 cent, max 0.328077 cent
      Preset{72,
             523.452,
             0.0011181940327211753,
             44100,
             75.022645528941837,
             1,
             {{SOS{-1.36810052f, 0.480552822f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 128, C5_string_128, RMS 0.253635 cent, max 0.329446 cent
      Preset{72,
             523.709,
             0.0011191026712183991,
             44100,
             74.979414294388164,
             1,
             {{SOS{-1.36828232f, 0.480683237f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 129, C#5_string_129, RMS 0.0246811 cent, max 0.0488617 cent
      Preset{73,
             554.376,
             0.0012295302954572266,
             44100,
             64.559449572422196,
             2,
             {{SOS{-1.36328685f, 0.471692443f}, SOS{-1.24414563f, 0.452196807f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 130, C#5_string_130, RMS 0.0246772 cent, max 0.0488694 cent
      Preset{73,
             554.649,
             0.0012305310187171968,
             44100,
             64.523316603035099,
             2,
             {{SOS{-1.36324668f, 0.471669734f}, SOS{-1.2440114f, 0.452152878f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 131, C#5_string_131, RMS 0.0246697 cent, max 0.0488625 cent
      Preset{73,
             554.921,
             0.0012315283841869743,
             44100,
             64.487689042277765,
             2,
             {{SOS{-1.36318839f, 0.471634626f}, SOS{-1.24385619f, 0.452094942f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 132, D5_string_132, RMS 0.012961 cent, max 0.0183351 cent
      Preset{74,
             587.416,
             0.0013528730659360278,
             44100,
             60.399025589222802,
             2,
             {{SOS{-1.36316907f, 0.472321451f}, SOS{-1.22963083f, 0.448741466f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 133, D5_string_133, RMS 0.0129611 cent, max 0.0183193 cent
      Preset{74,
             587.705,
             0.0013539716118675233,
             44100,
             60.366490280458905,
             2,
             {{SOS{-1.36304235f, 0.472239703f}, SOS{-1.22939694f, 0.448633254f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 134, D5_string_134, RMS 0.0129587 cent, max 0.018294 cent
      Preset{74,
             587.994,
             0.0013550704960375276,
             44100,
             60.334198055545087,
             2,
             {{SOS{-1.36290383f, 0.472149909f}, SOS{-1.22914863f, 0.448515564f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 135, D#5_string_135, RMS 0.0955768 cent, max 0.155643 cent
      Preset{75,
             622.426,
             0.0014883936924585063,
             44100,
             56.278005347913229,
             2,
             {{SOS{-1.3905021f, 0.492356181f}, SOS{-1.23628032f, 0.466606855f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 136, D#5_string_136, RMS 0.0956436 cent, max 0.155793 cent
      Preset{75,
             622.732,
             0.0014895997064160145,
             44100,
             56.248408298437333,
             2,
             {{SOS{-1.3903302f, 0.49224183f}, SOS{-1.23597872f, 0.466460019f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 137, D#5_string_137, RMS 0.0956958 cent, max 0.155909 cent
      Preset{75,
             623.037,
             0.0014908021470083413,
             44100,
             56.21909016283729,
             2,
             {{SOS{-1.39014769f, 0.492119968f}, SOS{-1.23566687f, 0.466305375f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 138, E5_string_138, RMS 0.07113 cent, max 0.122653 cent
      Preset{76,
             659.521,
             0.0016372643994772614,
             44100,
             53.647361274657662,
             2,
             {{SOS{-1.18815577f, 0.352928519f}, SOS{-1.1768254f, 0.39876619f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 139, E5_string_139, RMS 0.0708675 cent, max 0.121622 cent
      Preset{76,
             659.845,
             0.0016385882132663129,
             44100,
             53.59509538480301,
             2,
             {{SOS{-1.18917024f, 0.35353145f}, SOS{-1.17800963f, 0.399410337f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 140, E5_string_140, RMS 0.0706177 cent, max 0.12064 cent
      Preset{76,
             660.169,
             0.0016399124310108822,
             44100,
             53.544757083904777,
             2,
             {{SOS{-1.19008827f, 0.354077548f}, SOS{-1.1790998f, 0.4000099f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 141, F5_string_141, RMS 0.166513 cent, max 0.280247 cent
      Preset{77,
             698.828,
             0.0018007898642832853,
             44100,
             49.336102616369587,
             2,
             {{SOS{-1.36842954f, 0.477932394f}, SOS{-1.19453776f, 0.45108068f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 142, F5_string_142, RMS 0.168894 cent, max 0.284458 cent
      Preset{77,
             699.171,
             0.0018022425396217285,
             44100,
             49.289120820251838,
             2,
             {{SOS{-1.36975622f, 0.478861898f}, SOS{-1.19566774f, 0.451920271f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 143, F5_string_143, RMS 0.171169 cent, max 0.288479 cent
      Preset{77,
             699.515,
             0.0018036998933031727,
             44100,
             49.243813709814127,
             2,
             {{SOS{-1.37097216f, 0.479715824f}, SOS{-1.19669259f, 0.452694923f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 144, F#5_string_144, RMS 0.00728235 cent, max 0.013368 cent
      Preset{78,
             740.477,
             0.0019803801748292742,
             44100,
             46.385955556054128,
             2,
             {{SOS{-1.3047446f, 0.434676617f}, SOS{-1.14055443f, 0.404868633f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 145, F#5_string_145, RMS 0.00728352 cent, max 0.0133713 cent
      Preset{78,
             740.841,
             0.0019819779269423863,
             44100,
             46.360979727365887,
             2,
             {{SOS{-1.30455792f, 0.43455863f}, SOS{-1.14026606f, 0.404729933f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 146, F#5_string_146, RMS 0.00728936 cent, max 0.0133789 cent
      Preset{78,
             741.205,
             0.0019835761632297278,
             44100,
             46.336178618789248,
             2,
             {{SOS{-1.30436265f, 0.434435099f}, SOS{-1.13996518f, 0.404583931f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 147, G5_string_147, RMS 0.0101651 cent, max 0.0141914 cent
      Preset{79,
             784.608,
             0.0021775887830562946,
             44100,
             43.878387309525671,
             2,
             {{SOS{-1.23793519f, 0.391294152f}, SOS{-1.08687913f, 0.370977789f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 148, G5_string_148, RMS 0.0105197 cent, max 0.0147091 cent
      Preset{79,
             784.994,
             0.0021793445621545167,
             44100,
             43.810871665373767,
             2,
             {{SOS{-1.24066925f, 0.392985702f}, SOS{-1.08938134f, 0.372332752f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 149, G5_string_149, RMS 0.0108596 cent, max 0.0151843 cent
      Preset{79,
             785.379,
             0.0021810963210037405,
             44100,
             43.747821982763718,
             2,
             {{SOS{-1.24313259f, 0.394516289f}, SOS{-1.09167647f, 0.373601317f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 150, G#5_string_150, RMS 0.0278778 cent, max 0.0348581 cent
      Preset{80,
             831.37,
             0.0023941198237539172,
             44100,
             40.521560503631257,
             2,
             {{SOS{-1.27978539f, 0.41938895f}, SOS{-1.09849524f, 0.387336731f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 151, G#5_string_151, RMS 0.0280042 cent, max 0.0350191 cent
      Preset{80,
             831.778,
             0.0023960427315172735,
             44100,
             40.498605091523558,
             2,
             {{SOS{-1.27966249f, 0.419314116f}, SOS{-1.0982877f, 0.38725394f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 152, G#5_string_152, RMS 0.0280598 cent, max 0.0351175 cent
      Preset{80,
             832.187,
             0.0023979709317304625,
             44100,
             40.475825688329657,
             2,
             {{SOS{-1.2795248f, 0.419229865f}, SOS{-1.09806073f, 0.387159497f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 153, A5_string_153, RMS 0.0419296 cent, max 0.0681118 cent
      Preset{81,
             880.918,
             0.002631825353009612,
             44100,
             38.000001040391886,
             2,
             {{SOS{-1.23303115f, 0.388890773f}, SOS{-1.06836414f, 0.36557433f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 154, A5_string_154, RMS 0.225141 cent, max 0.313689 cent
      Preset{81,
             881.351,
             0.0026339395379290798,
             44100,
             38.000003048429278,
             2,
             {{SOS{-1.26055777f, 0.408567339f}, SOS{-1.04223216f, 0.356890619f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 155, A5_string_155, RMS 0.0181603 cent, max 0.0335723 cent
      Preset{81,
             881.784,
             0.0026360543561619763,
             44100,
             28.810582430470763,
             3,
             {{SOS{-1.67943501f, 0.716469049f}, SOS{-1.56717443f, 0.715458035f},
               SOS{-1.34266305f, 0.737543881f}, SOS{0.0f, 0.0f}}}},
      // string 156, A#5_string_156, RMS 0.0324604 cent, max 0.0449759 cent
      Preset{82,
             933.564,
             0.0028934788772695488,
             44100,
             35.999398633048123,
             2,
             {{SOS{-1.18066537f, 0.357276201f}, SOS{-1.00815642f, 0.338214397f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 157, A#5_string_157, RMS 0.00421121 cent, max 0.00559774 cent
      Preset{82,
             934.023,
             0.0028958005795701332,
             44100,
             25.005968735839822,
             3,
             {{SOS{-1.69301069f, 0.728176415f}, SOS{-1.56494498f, 0.706582487f},
               SOS{-1.39924335f, 0.731660843f}, SOS{0.0f, 0.0f}}}},
      // string 158, A#5_string_158, RMS 0.0275921 cent, max 0.0377536 cent
      Preset{82,
             934.481,
             0.0028981179144138163,
             44100,
             35.999398278276473,
             2,
             {{SOS{-1.16977155f, 0.350440681f}, SOS{-1.00435591f, 0.333384663f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 159, B5_string_159, RMS 0.0457757 cent, max 0.067602 cent
      Preset{83,
             989.356,
             0.0031807169710454416,
             44100,
             33.752701631736031,
             2,
             {{SOS{-1.05635417f, 0.278971046f}, SOS{-1.00954318f, 0.31854564f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 160, B5_string_160, RMS 0.0449569 cent, max 0.0664552 cent
      Preset{83,
             989.842,
             0.0031832632707381259,
             44100,
             33.702724011203713,
             2,
             {{SOS{-1.05832803f, 0.280014575f}, SOS{-1.01163399f, 0.319453597f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 161, B5_string_161, RMS 0.044167 cent, max 0.0653449 cent
      Preset{83,
             990.328,
             0.0031858103268383163,
             44100,
             33.655269372935571,
             2,
             {{SOS{-1.06012034f, 0.280963808f}, SOS{-1.01359892f, 0.320326835f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 162, C6_string_162, RMS 0.0465115 cent, max 0.0687169 cent
      Preset{84,
             1048.48,
             0.003495987417455864,
             44100,
             31.207532040951989,
             2,
             {{SOS{-1.07174265f, 0.287158042f}, SOS{-1.01241469f, 0.324895978f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 163, C6_string_163, RMS 0.0458574 cent, max 0.067814 cent
      Preset{84,
             1049,
             0.0034988090267594118,
             44100,
             31.178847348520645,
             2,
             {{SOS{-1.07229316f, 0.287453175f}, SOS{-1.01320815f, 0.325293094f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 164, C6_string_164, RMS 0.0452378 cent, max 0.0669273 cent
      Preset{84,
             1049.51,
             0.0035015771931539356,
             44100,
             31.151420723227311,
             2,
             {{SOS{-1.07278764f, 0.287718326f}, SOS{-1.0139277f, 0.32565701f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 165, C#6_string_165, RMS 0.000316961 cent, max 0.000491167 cent
      Preset{
          85,
          1111.14,
          0.0038419949048638536,
          44100,
          29.046443071307014,
          2,
          {{SOS{-1.08274305f, 0.296697021f}, SOS{-0.978730202f, 0.311012149f},
            SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 166, C#6_string_166, RMS 0.000322641 cent, max 0.000495471 cent
      Preset{
          85,
          1111.69,
          0.0038450794309375725,
          44100,
          29.019016136778021,
          2,
          {{SOS{-1.08440006f, 0.297693133f}, SOS{-0.979093552f, 0.311425745f},
            SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 167, C#6_string_167, RMS 0.000365458 cent, max 0.000519985 cent
      Preset{85,
             1112.23,
             0.0038481075901828548,
             44100,
             28.992943021549337,
             2,
             {{SOS{-1.08588767f, 0.2985892f}, SOS{-0.979406953f, 0.311797619f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 168, D6_string_168, RMS 0.421002 cent, max 0.582999 cent
      Preset{86,
             1177.55,
             0.0042119619561208719,
             44100,
             31.111949802100611,
             1,
             {{SOS{-1.13222432f, 0.338807166f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 169, D6_string_169, RMS 0.423554 cent, max 0.586716 cent
      Preset{86,
             1178.12,
             0.0042151127347369224,
             44100,
             31.090127131861273,
             1,
             {{SOS{-1.13281262f, 0.339163661f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 170, D6_string_170, RMS 0.426062 cent, max 0.590366 cent
      Preset{86,
             1178.7,
             0.0042183183075681406,
             44100,
             31.068234067358759,
             1,
             {{SOS{-1.13337111f, 0.339503378f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 171, D#6_string_171, RMS 0.427066 cent, max 0.575656 cent
      Preset{87,
             1247.92,
             0.0045970393473528538,
             44100,
             29.147572444372653,
             1,
             {{SOS{-1.12069178f, 0.334013283f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 172, D#6_string_172, RMS 0.430445 cent, max 0.580386 cent
      Preset{87,
             1248.53,
             0.0046003399631090302,
             44100,
             29.126057766197953,
             1,
             {{SOS{-1.12137485f, 0.334419966f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 173, D#6_string_173, RMS 0.433769 cent, max 0.585033 cent
      Preset{87,
             1249.15,
             0.004603693977777587,
             44100,
             29.104520805230379,
             1,
             {{SOS{-1.12202537f, 0.334808677f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 174, E6_string_174, RMS 0.0886092 cent, max 0.098425 cent
      Preset{88,
             1322.5,
             0.0049951145405383321,
             44100,
             22.001678087881835,
             2,
             {{SOS{-1.34653533f, 0.471067786f}, SOS{-1.09960759f, 0.470321536f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 175, E6_string_175, RMS 0.088575 cent, max 0.0983826 cent
      Preset{88,
             1323.15,
             0.0049985326872045224,
             44100,
             22.001676889746108,
             2,
             {{SOS{-1.34472454f, 0.469817698f}, SOS{-1.09743679f, 0.468809187f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 176, E6_string_176, RMS 0.0885726 cent, max 0.0983751 cent
      Preset{88,
             1323.8,
             0.0050019499015450716,
             44100,
             22.001675841719194,
             2,
             {{SOS{-1.34290862f, 0.468565524f}, SOS{-1.09526122f, 0.467292696f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 177, F6_string_177, RMS 0.101454 cent, max 0.154036 cent
      Preset{89,
             1401.53,
             0.0054035704786987411,
             44100,
             11.999244280970814,
             3,
             {{SOS{-1.67194998f, 0.715583622f}, SOS{-1.52167797f, 0.692718208f},
               SOS{-1.35547566f, 0.766354263f}, SOS{0.0f, 0.0f}}}},
      // string 178, F6_string_178, RMS 1.45965e-05 cent, max 2.34027e-05 cent
      Preset{89,
             1402.22,
             0.0054070707146882764,
             44100,
             16.000000602940069,
             3,
             {{SOS{-1.28436768f, 0.425174922f}, SOS{-1.14711654f, 0.384320587f},
               SOS{-1.06536663f, 0.517623067f}, SOS{0.0f, 0.0f}}}},
      // string 179, F6_string_179, RMS 4.03056e-05 cent, max 5.57011e-05 cent
      Preset{
          89,
          1402.91,
          0.0054105697677488025,
          44100,
          12.000000949162381,
          4,
          {{SOS{-1.28115773f, 0.418808609f}, SOS{-1.23580074f, 0.420883447f},
            SOS{-1.09668899f, 0.4560467f}, SOS{-0.935927272f, 0.503410339f}}}},
      // string 180, F#6_string_180, RMS 0.000117272 cent, max 0.000145552 cent
      Preset{90,
             1485.29,
             0.0058195612069360638,
             44100,
             24.16759504491073,
             1,
             {{SOS{-1.01932752f, 0.277674139f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 181, F#6_string_181, RMS 0.000144983 cent, max 0.000166255 cent
      Preset{90,
             1486.02,
             0.0058231055893683322,
             44100,
             24.155592417324264,
             1,
             {{SOS{-1.01894295f, 0.277479619f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 182, F#6_string_182, RMS 0.000147571 cent, max 0.00016793 cent
      Preset{90,
             1486.75,
             0.0058266485350518971,
             44100,
             24.143619638163401,
             1,
             {{SOS{-1.0185554f, 0.277283579f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}}}},
      // string 183, G6_string_183, RMS 0.0306367 cent, max 0.0327469 cent
      Preset{91,
             1574.06,
             0.0062398203557380589,
             44100,
             22.684424655220411,
             1,
             {{SOS{-0.987202406f, 0.260481417f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 184, G6_string_184, RMS 0.0306404 cent, max 0.0327705 cent
      Preset{91,
             1574.83,
             0.006243369216496159,
             44100,
             22.671343361306526,
             1,
             {{SOS{-0.987166286f, 0.260477662f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 185, G6_string_185, RMS 0.030637 cent, max 0.0327852 cent
      Preset{91,
             1575.6,
             0.0062469163912102438,
             44100,
             22.658357650952986,
             1,
             {{SOS{-0.987114847f, 0.260465831f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 186, G#6_string_186, RMS 0.000424858 cent, max 0.000465649 cent
      Preset{92,
             1668.12,
             0.0066607089561924035,
             44100,
             21.492102171226474,
             1,
             {{SOS{-0.923197329f, 0.230241418f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 187, G#6_string_187, RMS 0.000425369 cent, max 0.000466139 cent
      Preset{92,
             1668.94,
             0.0066642651179016771,
             44100,
             21.466901841111053,
             1,
             {{SOS{-0.925539911f, 0.231360078f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 188, G#6_string_188, RMS 0.000417382 cent, max 0.000454917 cent
      Preset{92,
             1669.76,
             0.0066678193086544425,
             44100,
             21.442509616138615,
             1,
             {{SOS{-0.927740097f, 0.232417539f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 189, A6_string_189, RMS 0.00064032 cent, max 0.000889239 cent
      Preset{93,
             1767.81,
             0.0070785229884286109,
             44100,
             13.714811665059679,
             2,
             {{SOS{-1.64643347f, 0.722500026f}, SOS{-1.20364571f, 0.722500026f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 190, A6_string_190, RMS 0.00087199 cent, max 0.00117393 cent
      Preset{93,
             1768.68,
             0.0070820399260056689,
             44100,
             13.707084511199966,
             2,
             {{SOS{-1.64638722f, 0.722500026f}, SOS{-1.20303035f, 0.722500026f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 191, A6_string_191, RMS 0.000842733 cent, max 0.00112965 cent
      Preset{93,
             1769.55,
             0.0070855546206974353,
             44100,
             13.699373010697011,
             2,
             {{SOS{-1.64634085f, 0.722500026f}, SOS{-1.20241547f, 0.722500026f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 192, A#6_string_192, RMS 0.00155009 cent, max 0.00200433 cent
      Preset{94,
             1874.6,
             0.0074934700986100814,
             44100,
             18.673239995495855,
             1,
             {{SOS{-0.917059243f, 0.229527399f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 193, A#6_string_193, RMS 0.00154141 cent, max 0.00199729 cent
      Preset{94,
             1875.52,
             0.0074968984776709983,
             44100,
             18.662323841681104,
             1,
             {{SOS{-0.917016745f, 0.229525939f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 194, A#6_string_194, RMS 0.00154059 cent, max 0.0019964 cent
      Preset{94,
             1876.45,
             0.007500361588523464,
             44100,
             18.651374782565242,
             1,
             {{SOS{-0.916957498f, 0.229516298f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 195, B6_string_195, RMS 0.000809134 cent, max 0.00105743 cent
      Preset{95,
             1987.84,
             0.0078968439886875907,
             44100,
             12.026817452720875,
             2,
             {{SOS{-1.63294947f, 0.722500026f}, SOS{-1.0655334f, 0.722500026f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 196, B6_string_196, RMS 0.000831771 cent, max 0.00108194 cent
      Preset{95,
             1988.82,
             0.0079001722277890086,
             44100,
             12.020278126646875,
             2,
             {{SOS{-1.63288331f, 0.722500026f}, SOS{-1.06497121f, 0.722500026f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 197, B6_string_197, RMS 7.01357e-05 cent, max 7.39397e-05 cent
      Preset{95,
             1989.8,
             0.0079034976998747491,
             44100,
             14.000000891882513,
             2,
             {{SOS{-1.4056859f, 0.540458202f}, SOS{-0.912294567f, 0.722661912f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 198, C7_string_198, RMS 0.0072599 cent, max 0.0101332 cent
      Preset{96,
             2107.92,
             0.0082842751064562091,
             88200,
             33.318386347516658,
             1,
             {{SOS{-1.3204248f, 0.447994292f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}}}},
      // string 199, C7_string_199, RMS 0.00725261 cent, max 0.010124 cent
      Preset{96,
             2108.96,
             0.0082874532511910421,
             88200,
             33.296762799490836,
             1,
             {{SOS{-1.32058275f, 0.448110223f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 200, C7_string_200, RMS 0.00721929 cent, max 0.0100807 cent
      Preset{96,
             2109.99,
             0.0082905978744504689,
             88200,
             33.275532810630736,
             1,
             {{SOS{-1.32072508f, 0.448215634f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 201, C#7_string_201, RMS 0.00138066 cent, max 0.00145531 cent
      Preset{97,
             2235.25,
             0.0086513811057734348,
             88200,
             31.318346804009952,
             1,
             {{SOS{-1.29215562f, 0.429868311f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 202, C#7_string_202, RMS 0.00141061 cent, max 0.00147742 cent
      Preset{97,
             2236.35,
             0.0086543623851430551,
             88200,
             31.29772136916305,
             1,
             {{SOS{-1.29234743f, 0.430005938f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 203, C#7_string_203, RMS 0.00132121 cent, max 0.00141256 cent
      Preset{97,
             2237.45,
             0.0086573404646191551,
             88200,
             31.277282384839008,
             1,
             {{SOS{-1.29252338f, 0.430133104f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 204, D7_string_204, RMS 0.000641659 cent, max 0.000776555 cent
      Preset{98,
             2370.28,
             0.0089938977410334853,
             88200,
             22.000012741929531,
             2,
             {{SOS{-1.59982431f, 0.657696545f}, SOS{-1.36219263f, 0.629240453f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 205, D7_string_205, RMS 0.191937 cent, max 0.273961 cent
      Preset{98,
             2371.44,
             0.0089966395612102908,
             88200,
             13.996157562636316,
             3,
             {{SOS{-1.69268107f, 0.723850667f}, SOS{-1.63934112f, 0.759000838f},
               SOS{-1.44566512f, 0.773839831f}, SOS{0.0f, 0.0f}}}},
      // string 206, D7_string_206, RMS 0.000202151 cent, max 0.000238 cent
      Preset{98,
             2372.61,
             0.0089994016309385607,
             88200,
             22.000003729908478,
             2,
             {{SOS{-1.5972755f, 0.655631363f}, SOS{-1.35910559f, 0.626883447f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 207, D#7_string_207, RMS 0.000441831 cent, max 0.000606831 cent
      Preset{99,
             2513.46,
             0.0093076518307531313,
             88200,
             27.777379913662099,
             1,
             {{SOS{-1.22344017f, 0.38829419f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}}}},
      // string 208, D#7_string_208, RMS 0.000426957 cent, max 0.000587101 cent
      Preset{99,
             2514.7,
             0.0093101571409708352,
             88200,
             27.752521509315972,
             1,
             {{SOS{-1.22431958f, 0.388849646f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 209, D#7_string_209, RMS 0.000408467 cent, max 0.000563418 cent
      Preset{99,
             2515.93,
             0.0093126387443926931,
             88200,
             27.72829710779693,
             1,
             {{SOS{-1.22514904f, 0.389375001f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 210, E7_string_210, RMS 0.00257445 cent, max 0.00363256 cent
      Preset{100,
             2665.29,
             0.009588770619505492,
             88200,
             18.905815836152264,
             2,
             {{SOS{-1.66974306f, 0.722500026f}, SOS{-1.38070571f, 0.722500026f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 211, E7_string_211, RMS 0.00244645 cent, max 0.00343289 cent
      Preset{100,
             2666.6,
             0.0095909775822555152,
             88200,
             18.895092046144043,
             2,
             {{SOS{-1.66971457f, 0.722500026f}, SOS{-1.38039565f, 0.722500026f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 212, E7_string_212, RMS 0.00220576 cent, max 0.00299561 cent
      Preset{100,
             2667.91,
             0.0095931809174093739,
             88200,
             18.884372716231621,
             2,
             {{SOS{-1.66968608f, 0.722500026f}, SOS{-1.3800863f, 0.722500026f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 213, F7_string_213, RMS 0.00839405 cent, max 0.00951399 cent
      Preset{101,
             2826.29,
             0.009833681755363196,
             88200,
             18.000583123255574,
             2,
             {{SOS{-1.56310523f, 0.634928048f}, SOS{-1.24395955f, 0.598789275f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 214, F7_string_214, RMS 0.00865963 cent, max 0.00972991 cent
      Preset{101,
             2827.68,
             0.0098355725817048413,
             88200,
             18.000586746003947,
             2,
             {{SOS{-1.56167495f, 0.633785248f}, SOS{-1.24230957f, 0.597527325f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 215, F7_string_215, RMS 0.00889243 cent, max 0.00992166 cent
      Preset{101,
             2829.07,
             0.0098374597092516624,
             88200,
             18.000590786471179,
             2,
             {{SOS{-1.56024241f, 0.632641554f}, SOS{-1.24065709f, 0.596263409f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 216, F#7_string_216, RMS 0.168861 cent, max 0.256157 cent
      Preset{102,
             2997.02,
             0.010039223629270608,
             88200,
             16.000000355382916,
             3,
             {{SOS{-1.36554718f, 0.486091226f}, SOS{-1.01614058f, 0.423193067f},
               SOS{-0.0296824481f, 0.000270999182f}, SOS{0.0f, 0.0f}}}},
      // string 217, F#7_string_217, RMS 0.0768828 cent, max 0.104975 cent
      Preset{102,
             2998.49,
             0.010040768085133125,
             88200,
             10.525596218358849,
             3,
             {{SOS{-1.66923022f, 0.714031577f}, SOS{-1.50187647f, 0.690803409f},
               SOS{-1.23671746f, 0.721973896f}, SOS{0.0f, 0.0f}}}},
      // string 218, F#7_string_218, RMS 0.0652097 cent, max 0.0825746 cent
      Preset{102,
             2999.97,
             0.010042319309200817,
             88200,
             14.160639420037748,
             3,
             {{SOS{-1.64843595f, 0.706556797f}, SOS{-1.32500219f, 0.629247069f},
               SOS{-0.867283881f, 0.724189162f}, SOS{0.0f, 0.0f}}}},
      // string 219, G7_string_219, RMS 0.136353 cent, max 0.226059 cent
      Preset{103,
             3178.06,
             0.010202684190796315,
             88200,
             16.017567723774462,
             2,
             {{SOS{-1.49830008f, 0.589225113f}, SOS{-1.11408174f, 0.545398951f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 220, G7_string_220, RMS 3.72029e-05 cent, max 4.32209e-05 cent
      Preset{103,
             3179.62,
             0.010203867825481026,
             88200,
             16.000000381490487,
             2,
             {{SOS{-1.49889171f, 0.589689612f}, SOS{-1.11514616f, 0.546209872f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 221, G7_string_221, RMS 0.137378 cent, max 0.22782 cent
      Preset{103,
             3181.18,
             0.010205047779361105,
             88200,
             16.017571190214831,
             2,
             {{SOS{-1.49509454f, 0.586748004f}, SOS{-1.11050975f, 0.542654335f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 222, G#7_string_222, RMS 0.000141284 cent, max 0.000176847 cent
      Preset{
          104,
          3370.04,
          0.010321893182107819,
          88200,
          16.000002032792771,
          2,
          {{SOS{-1.27500296f, 0.427498698f}, SOS{-0.888396561f, 0.363244414f},
            SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 223, G#7_string_223, RMS 0.000180191 cent, max 0.00022825 cent
      Preset{
          104,
          3371.69,
          0.010322696893368198,
          88200,
          16.000002677693999,
          2,
          {{SOS{-1.27262473f, 0.425892979f}, SOS{-0.886437476f, 0.361479849f},
            SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 224, G#7_string_224, RMS 0.115843 cent, max 0.146121 cent
      Preset{104,
             3373.35,
             0.010323501853965826,
             88200,
             8.6495971530366376,
             3,
             {{SOS{-1.76842439f, 0.807777464f}, SOS{-1.62427771f, 0.858462095f},
               SOS{-1.04093957f, 0.779230833f}, SOS{0.0f, 0.0f}}}},
      // string 225, A7_string_225, RMS 0.012793 cent, max 0.016194 cent
      Preset{
          105,
          3573.61,
          0.010395243004170114,
          88200,
          6.0001273435570042,
          4,
          {{SOS{-1.39757311f, 0.506655812f}, SOS{-1.33216977f, 0.495056063f},
            SOS{-1.1535207f, 0.566112936f}, SOS{-0.799214065f, 0.607840836f}}}},
      // string 226, A7_string_226, RMS 0.0125826 cent, max 0.015559 cent
      Preset{105,
             3575.37,
             0.010395661433177543,
             88200,
             14.000126790649743,
             2,
             {{SOS{-1.47835219f, 0.581238091f}, SOS{-1.01673913f, 0.535792649f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 227, A7_string_227, RMS 0.0127507 cent, max 0.0162612 cent
      Preset{105,
             3577.12,
             0.010396074005476357,
             88200,
             10.00012784677288,
             3,
             {{SOS{-1.42734838f, 0.522639096f}, SOS{-1.26879501f, 0.537110388f},
               SOS{-0.875723064f, 0.56214875f}, SOS{0.0f, 0.0f}}}},
      // string 228, A#7_string_228, RMS 0.000117122 cent, max 0.000147695 cent
      Preset{106,
             3789.48,
             0.01042174655198037,
             88200,
             14.000001613681457,
             2,
             {{SOS{-1.24373782f, 0.413003951f}, SOS{-0.786167085f, 0.34910962f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 229, A#7_string_229, RMS 0.000116051 cent, max 0.000150047 cent
      Preset{
          106,
          3791.34,
          0.010421769215013135,
          88200,
          14.000001675880899,
          2,
          {{SOS{-1.24137294f, 0.411434382f}, SOS{-0.784190536f, 0.347363144f},
            SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 230, A#7_string_230, RMS 0.000113487 cent, max 0.000145656 cent
      Preset{106,
             3793.21,
             0.010421788656279837,
             88200,
             14.000001588951225,
             2,
             {{SOS{-1.23898542f, 0.409852296f}, SOS{-0.782206714f, 0.34560445f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 231, B7_string_231, RMS 0.0334274 cent, max 0.0494968 cent
      Preset{107,
             4018.39,
             0.010401042854577084,
             88200,
             8.0005738075314419,
             3,
             {{SOS{-1.51870525f, 0.599547863f}, SOS{-1.28559804f, 0.603122532f},
               SOS{-0.789705694f, 0.614614069f}, SOS{0.0f, 0.0f}}}},
      // string 232, B7_string_232, RMS 0.0335115 cent, max 0.0494617 cent
      Preset{107,
             4020.37,
             0.010400669406307886,
             88200,
             8.0005748618613062,
             3,
             {{SOS{-1.51785016f, 0.598956406f}, SOS{-1.28394163f, 0.601855993f},
               SOS{-0.788839817f, 0.613911569f}, SOS{0.0f, 0.0f}}}},
      // string 233, B7_string_233, RMS 0.000212285 cent, max 0.000239672 cent
      Preset{107,
             4022.34,
             0.010400294740517305,
             88200,
             6.0000023601015702,
             4,
             {{SOS{-1.36827624f, 0.509053886f}, SOS{-1.40227294f, 0.545726359f},
               SOS{-1.05850959f, 0.688098848f},
               SOS{-0.376962274f, 0.730586529f}}}},
      // string 234, C8_string_234, RMS 9.10666e-06 cent, max 9.80352e-06 cent
      Preset{
          108,
          4261.13,
          0.010333412023005079,
          88200,
          12.000000207904682,
          2,
          {{SOS{-1.28697085f, 0.451614052f}, SOS{-0.725080848f, 0.397727132f},
            SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 235, C8_string_235, RMS 0.0196701 cent, max 0.030125 cent
      Preset{
          108,
          4263.23,
          0.010332647299167762,
          88200,
          12.00196466743404,
          2,
          {{SOS{-1.28418291f, 0.449678481f}, SOS{-0.722703278f, 0.395609587f},
            SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 236, C8_string_236, RMS 0.0196532 cent, max 0.0301473 cent
      Preset{
          108,
          4265.32,
          0.010331883366577739,
          88200,
          12.001963952874597,
          2,
          {{SOS{-1.28201258f, 0.448183239f}, SOS{-0.720752478f, 0.393981755f},
            SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
  }};
};

#endif

// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
// [ZERO AI-GENERATED CODE]
// Every line in this file is written and understood by its author. Every result is
// reproducible, every assumption is open to inspection, and every implementation
// stands open to criticism and challenge.
// AI may be used for non-core, replaceable, engineering work;
// this file, however, contains core logic that the author considers
// necessary to understand firsthand, explain line by line, and take full responsibility for,
// and is therefore implemented entirely by hand.
// ---------------------------------------------------------------------------
// [本文件承诺不含任何 AI 生成代码]
// 每一行代码均由作者亲自编写，并确知其意义。一切结果可以复现，一切假设可经受检验，一切实现经得起批评与质疑。
// AI 可用于非核心、可替代的工程工作；
// 本文件承载作者认为必须亲自理解、能够逐行解释并为之负责的核心逻辑，因此刻意保持完全人工实现。
//
// Ziyang Tan
// 2026-09-04
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

#ifndef configuration_hpp
#define configuration_hpp

#include <iostream>

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

#ifndef Precompute_f0_hpp
#define Precompute_f0_hpp

#include <algorithm>
#include <array>
#include <cmath>


namespace Parameters::Tuning {

enum class Temperament {
    equal,
    pythagore,
    zarlino,
    meantone,
    well,
    werckmeister
};


constexpr int kMidiMin = 21;   // A0
constexpr int kMidiMax = 108;  // C8
constexpr int kKeyCount = kMidiMax - kMidiMin + 1;

// TODO: 这是什么
static_assert(kKeyCount == 88, "tuning preset table must cover 88 keys");

constexpr int kA4Midi = 69;
constexpr double kA4Frequency = 440.0;


using FrequencyTable = std::array<double, kKeyCount>;
using UnisonOffsetTable = std::array<double, kKeyCount>;
using StringImpedanceTable = std::array<double, kKeyCount>;

enum class StringIndex {
    left = 1,
    center = 2,
    right = 3
};


constexpr double kPreferredUnisonSpreadCents = 1.7;

}  // namespace Parameters::Tuning



class TunningPresets {
    TunningPresets(const TunningPresets&) = delete;
    TunningPresets& operator=(const TunningPresets&) = delete;
public:
    using Temperament = Parameters::Tuning::Temperament;
    using FrequencyTable = Parameters::Tuning::FrequencyTable;
    using StringIndex = Parameters::Tuning::StringIndex;
    using UnisonOffsetTable = Parameters::Tuning::UnisonOffsetTable;
    using StringImpedanceTable = Parameters::Tuning::StringImpedanceTable;


    constexpr TunningPresets() noexcept = default;


    double get_frequency(
        int midi_n,
        Temperament temperament = Temperament::equal,
        StringIndex stringIndex = StringIndex::center
    ) const noexcept
    {
        if (midi_n < Parameters::Tuning::kMidiMin ||
            midi_n > Parameters::Tuning::kMidiMax) {
            return 0.0;
        }

        const int index = midi_n - Parameters::Tuning::kMidiMin;

        const Tables& tables = sharedTables();

        const double baseFrequency =
            getBaseFrequencyByIndex(
                tables,
                index,
                temperament
            );

        switch (stringIndex) {
            case StringIndex::left:
                return baseFrequency - tables.unisonOffsetHz[index];

            case StringIndex::center:
                return baseFrequency;

            case StringIndex::right:
                return baseFrequency + tables.unisonOffsetHz[index];
        }

        return baseFrequency;
    }


    const FrequencyTable& getEqualTemperamentTable() const noexcept
    {
        return sharedTables().equalTemperament;
    }


    double get_characteristic_impedance(int midi_n) const noexcept
    {
        if (midi_n < Parameters::Tuning::kMidiMin ||
            midi_n > Parameters::Tuning::kMidiMax) {
            return 0.0;
        }

        const int index = midi_n - Parameters::Tuning::kMidiMin;
        return sharedTables().characteristicImpedance[index];
    }


    const StringImpedanceTable&
    getCharacteristicImpedanceTable() const noexcept
    {
        return sharedTables().characteristicImpedance;
    }


private:

    struct Tables {
        FrequencyTable equalTemperament{};

        FrequencyTable pythagoreTemperament{};
        FrequencyTable zarlinoTemperament{};
        FrequencyTable meantoneTemperament{};
        FrequencyTable wellTemperament{};
        FrequencyTable werckmeisterTemperament{};

        UnisonOffsetTable unisonOffsetHz{};
        StringImpedanceTable characteristicImpedance{};
    };


    static Tables makeTables()
    {
        Tables tables{};

        computeEqualTemperament(tables.equalTemperament);

        // For now, keep the other temperament tables initialized to ET.
        // They will be replaced by their own formulas later.
        tables.pythagoreTemperament = tables.equalTemperament;
        tables.zarlinoTemperament = tables.equalTemperament;
        tables.meantoneTemperament = tables.equalTemperament;
        tables.wellTemperament = tables.equalTemperament;
        tables.werckmeisterTemperament = tables.equalTemperament;

        computeUnisonOffsets(
            tables.equalTemperament,
            tables.unisonOffsetHz
        );

        return tables;
    }


    static const Tables& sharedTables()
    {
        static const Tables tables = makeTables();
        return tables;
    }


    static double strictEqualFrequency(int midi_n)
    {
        return Parameters::Tuning::kA4Frequency *
            std::pow(
                2.0,
                static_cast<double>(
                    midi_n - Parameters::Tuning::kA4Midi
                ) / 12.0
            );
    }


    static double computeFittedB(double f0_hz)
    {
        constexpr double k1 = -1.3333333333333333;
        constexpr double k2 =  1.3333333333333333;

        constexpr double c0 = -6.5760753629956712;
        constexpr double c1 =  1.9137732290077178;
        constexpr double c2 =  0.27614763151078042;
        constexpr double c3 = -0.28572738557770405;
        constexpr double c4 = -0.30709648310494486;

        const double x = std::log2(f0_hz / 440.0);

        double log_b =
              c0
            + c1 * x
            + c2 * x * x
            + c3 * std::pow(
                std::max(x - k1, 0.0),
                2.0
            )
            + c4 * std::pow(
                std::max(x - k2, 0.0),
                2.0
            );

        log_b = std::clamp(
            log_b,
            std::log(5e-6),
            std::log(2e-2)
        );

        return std::exp(log_b);
    }


    static double getInharmonicityBFromStrictEqual(int midi_n)
    {
        return computeFittedB(
            strictEqualFrequency(midi_n)
        );
    }


    static double octaveTypeRhoForMidi(int lowerMidi)
    {
        // Rigaud-David-Daudet style octave-type model:
        // rho tends toward high octave types in the bass
        // and toward 2:1 in the treble.

        constexpr double kappa = 3.5;
        constexpr double m0 = 60.0;
        constexpr double alpha = 25.0;

        const double x =
            (static_cast<double>(lowerMidi) - m0) /
            alpha;

        return
            (kappa * 0.5) *
            (1.0 - std::erf(x)) +
            1.0;
    }


    static double tuneUpperOctaveFrequency(
        int lowerMidi,
        int upperMidi,
        double lowerF0
    )
    {
        const double rho =
            octaveTypeRhoForMidi(lowerMidi);

        const double rho2 = rho * rho;

        const double lowerB =
            getInharmonicityBFromStrictEqual(lowerMidi);

        const double upperB =
            getInharmonicityBFromStrictEqual(upperMidi);

        return
            2.0 *
            lowerF0 *
            std::sqrt(
                (1.0 + lowerB * 4.0 * rho2) /
                (1.0 + upperB * rho2)
            );
    }


    static double tuneLowerOctaveFrequency(
        int lowerMidi,
        int upperMidi,
        double upperF0
    )
    {
        const double rho =
            octaveTypeRhoForMidi(lowerMidi);

        const double rho2 = rho * rho;

        const double lowerB =
            getInharmonicityBFromStrictEqual(lowerMidi);

        const double upperB =
            getInharmonicityBFromStrictEqual(upperMidi);

        const double octaveStretchRatio =
            std::sqrt(
                (1.0 + lowerB * 4.0 * rho2) /
                (1.0 + upperB * rho2)
            );

        return
            upperF0 /
            (2.0 * octaveStretchRatio);
    }


    static double interpolateAOctaveDeviationCents(
        int midi_n,
        const std::array<int, 8>& aMidiNotes,
        const std::array<double, 8>& aDeviationCents
    )
    {
        if (midi_n <= aMidiNotes.front()) {
            return aDeviationCents.front();
        }

        for (
            std::size_t i = 0;
            i + 1 < aMidiNotes.size();
            ++i
        ) {
            const int leftMidi =
                aMidiNotes[i];

            const int rightMidi =
                aMidiNotes[i + 1];

            if (midi_n <= rightMidi) {
                const double t =
                    static_cast<double>(
                        midi_n - leftMidi
                    ) /
                    static_cast<double>(
                        rightMidi - leftMidi
                    );

                return
                    aDeviationCents[i] +
                    t *
                    (
                        aDeviationCents[i + 1] -
                        aDeviationCents[i]
                    );
            }
        }

        const std::size_t last =
            aMidiNotes.size() - 1;

        const std::size_t prev =
            last - 1;

        const double slope =
            (
                aDeviationCents[last] -
                aDeviationCents[prev]
            ) /
            static_cast<double>(
                aMidiNotes[last] -
                aMidiNotes[prev]
            );

        return
            aDeviationCents[last] +
            slope *
            static_cast<double>(
                midi_n -
                aMidiNotes[last]
            );
    }


    static void computeEqualTemperament(
        FrequencyTable& equalTemperament
    )
    {
        // This is not strict mathematical ET.
        // It is an equal-temperament layout corrected by piano octave stretching.
        //
        // A4 is kept at 440 Hz.
        // The A-octaves are tuned by matching octave-related
        // inharmonic partials, then the deviation from strict ET
        // is interpolated across the full keyboard.

        FrequencyTable strictEqual{};

        for (
            int midi = Parameters::Tuning::kMidiMin;
            midi <= Parameters::Tuning::kMidiMax;
            ++midi
        ) {
            const int index =
                midi -
                Parameters::Tuning::kMidiMin;

            strictEqual[index] =
                strictEqualFrequency(midi);
        }


        constexpr std::array<int, 8> aMidiNotes = {
            21,
            33,
            45,
            57,
            69,
            81,
            93,
            105
        };


        std::array<
            double,
            aMidiNotes.size()
        > tunedAFrequencies{};


        std::array<
            double,
            aMidiNotes.size()
        > tunedADeviationsCents{};


        constexpr int a4AnchorIndex = 4;

        tunedAFrequencies[a4AnchorIndex] =
            Parameters::Tuning::kA4Frequency;


        for (
            int i = a4AnchorIndex + 1;
            i < static_cast<int>(aMidiNotes.size());
            ++i
        ) {
            tunedAFrequencies[i] =
                tuneUpperOctaveFrequency(
                    aMidiNotes[i - 1],
                    aMidiNotes[i],
                    tunedAFrequencies[i - 1]
                );
        }


        for (
            int i = a4AnchorIndex - 1;
            i >= 0;
            --i
        ) {
            tunedAFrequencies[i] =
                tuneLowerOctaveFrequency(
                    aMidiNotes[i],
                    aMidiNotes[i + 1],
                    tunedAFrequencies[i + 1]
                );
        }


        for (
            std::size_t i = 0;
            i < aMidiNotes.size();
            ++i
        ) {
            tunedADeviationsCents[i] =
                1200.0 *
                std::log2(
                    tunedAFrequencies[i] /
                    strictEqualFrequency(
                        aMidiNotes[i]
                    )
                );
        }


        for (
            int midi = Parameters::Tuning::kMidiMin;
            midi <= Parameters::Tuning::kMidiMax;
            ++midi
        ) {
            const int index =
                midi -
                Parameters::Tuning::kMidiMin;

            const double deviationCents =
                interpolateAOctaveDeviationCents(
                    midi,
                    aMidiNotes,
                    tunedADeviationsCents
                );

            equalTemperament[index] =
                strictEqual[index] *
                std::pow(
                    2.0,
                    deviationCents / 1200.0
                );
        }
    }


    static double getBaseFrequencyByIndex(
        const Tables& tables,
        int index,
        Temperament temperament
    ) noexcept
    {
        switch (temperament) {
            case Temperament::equal:
                return
                    tables.equalTemperament[index];

            case Temperament::pythagore:
                return
                    tables.pythagoreTemperament[index];

            case Temperament::zarlino:
                return
                    tables.zarlinoTemperament[index];

            case Temperament::meantone:
                return
                    tables.meantoneTemperament[index];

            case Temperament::well:
                return
                    tables.wellTemperament[index];

            case Temperament::werckmeister:
                return
                    tables.werckmeisterTemperament[index];
        }

        return
            tables.equalTemperament[index];
    }


    static int stringCountForMidi(
        int midi_n
    ) noexcept
    {
        // Steinway D-274:
        //
        // MIDI 21-28 : single string
        // MIDI 29-33 : bichord
        // MIDI 34-108: trichord

        if (midi_n <= 28) {
            return 1;
        }

        if (midi_n <= 33) {
            return 2;
        }

        return 3;
    }


    static double computeUnisonHalfOffsetHz(
        double f0,
        int string_count
    )
    {
        if (string_count <= 1) {
            return 0.0;
        }

        const double spreadRatio =
            std::pow(
                2.0,
                Parameters::Tuning::kPreferredUnisonSpreadCents /
                1200.0
            );

        const double rightFrequency =
            f0 * spreadRatio;

        const double spreadHz =
            rightFrequency - f0;

        return spreadHz * 0.5;
    }


    static void computeUnisonOffsets(
        const FrequencyTable& equalTemperament,
        UnisonOffsetTable& unisonOffsetHz
    )
    {
        for (
            int midi = Parameters::Tuning::kMidiMin;
            midi <= Parameters::Tuning::kMidiMax;
            ++midi
        ) {
            const int index =
                midi -
                Parameters::Tuning::kMidiMin;

            const int string_count =
                stringCountForMidi(midi);

            const double f0 =
                equalTemperament[index];

            unisonOffsetHz[index] =
                computeUnisonHalfOffsetHz(
                    f0,
                    string_count
                );
        }
    }

};


#endif /* Precompute_f0_hpp */

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

#ifndef hammer_preset_hpp
#define hammer_preset_hpp

#include <array>

namespace Parameters::Hammer::LegacyFit {

struct HammerPreset {
    double k_a, k_b, c_a, c_b;
    double p1, p2, p3, p4;
    double mass_kg, release_threshold_m;
};

inline constexpr std::array<HammerPreset, 88> kHammerPresets{{
    HammerPreset{1.90124055258e+15, 1375613.66073, 0.119408639313, 38.0826056266, 4.13048457387, 2.3661379754, 1.84042891408, 0.752770660711, 0.0110001, -0.000149802114365},  // MIDI 21
    HammerPreset{9.59155564422e+15, 1203516.60213, 0.336605009809, 42.7975403467, 4.3217167608, 2.13689865174, 1.87282283511, 0.846590795985, 0.0109264, -0.000166014912801},  // MIDI 22
    HammerPreset{2.60495506681e+16, 4766224.352, 0.680080500023, 43.8041537575, 4.43248325664, 2.02184788517, 2.00582859629, 0.882320819001, 0.0108529, -0.000132863768194},  // MIDI 23
    HammerPreset{1.99698309552e+16, 13924813.291, 0.690558976978, 42.7497316804, 4.41477849949, 2.05016826179, 2.09363392848, 0.869484001743, 0.0107796, -0.000163429243251},  // MIDI 24
    HammerPreset{8.12371683584e+15, 10074715.5664, 0.500826731448, 41.752426156, 4.28611951718, 2.12302448773, 2.04460267567, 0.857263030672, 0.0107065, -0.000136441301655},  // MIDI 25
    HammerPreset{6.28225497142e+15, 9016749.14465, 1.33590430622, 41.6112520286, 4.24669728008, 2.14487920175, 1.88943153814, 0.851462584024, 0.0106336, -0.000136552779455},  // MIDI 26
    HammerPreset{1.07968035525e+16, 2704775.11953, 0.822794057432, 35.3120056655, 4.29546731673, 2.12303565723, 1.77570820998, 0.864135668989, 0.0105609, -0.000135379752219},  // MIDI 27
    HammerPreset{8.9091042996e+15, 2207178.48042, 0.563376468068, 37.0341337886, 4.26124977415, 2.05617607923, 1.82048610449, 0.904995431651, 0.0104884, -0.000116150999085},  // MIDI 28
    HammerPreset{8.53567720522e+15, 16539450.8839, 0.378958708451, 48.1140075828, 4.24924103727, 1.97298528202, 1.89889780684, 0.968287636118, 0.0104161, -0.000160978275395},  // MIDI 29
    HammerPreset{1.97079971635e+16, 11716508.0714, 1.01676591528, 54.7853828661, 4.32921359654, 1.86305537124, 1.92334683335, 1.00177512769, 0.010344, -0.000504397989134},  // MIDI 30
    HammerPreset{2.43941407626e+16, 2509784.20042, 0.91437413231, 51.6672100607, 4.38098336984, 1.69544031575, 1.91200131707, 0.981753094032, 0.0102721, -0.000317725913316},  // MIDI 31
    HammerPreset{1.08083268369e+16, 4265860.33642, 0.977110846001, 49.7863985178, 4.28618459335, 1.5201450234, 1.91014588707, 0.950445267132, 0.0102004, -0.000203022072646},  // MIDI 32
    HammerPreset{7.46766237692e+15, 8141565.71313, 0.642531064534, 44.244546139, 4.21901048949, 1.47609829611, 1.96529179691, 0.939866028905, 0.0101289, -0.000128452961692},  // MIDI 33
    HammerPreset{2.24847450514e+16, 4988494.93476, 0.166314024304, 47.4859641528, 4.35104734796, 1.51202563395, 2.00643838807, 0.925925602717, 0.0100576, -0.000119826877057},  // MIDI 34
    HammerPreset{1.15153139959e+17, 4119685.0189, 0.398230846254, 40.9874728747, 4.56044593576, 1.59089095524, 1.96575748133, 0.892274281148, 0.0099865, -0.000120796129225},  // MIDI 35
    HammerPreset{5.97006130445e+17, 1614025.85489, 1.11643970939, 41.305850937, 4.74783933855, 1.76776079661, 1.85061335816, 0.879405355389, 0.0099156, -0.000119602779202},  // MIDI 36
    HammerPreset{2.93661522715e+18, 303123.896986, 0.732342028387, 41.2024689357, 4.93348828936, 2.00596330823, 1.65453597875, 0.872580344006, 0.0098449, -0.000105054800329},  // MIDI 37
    HammerPreset{1.07745332812e+19, 272586.97748, 2.0533668725, 37.6134048809, 5.09015948451, 2.25323482166, 1.53646772938, 0.833646690938, 0.0097744, -9.22221054168e-05},  // MIDI 38
    HammerPreset{1.63210828375e+19, 208252.936695, 1.48142793424, 33.7012539807, 5.15057732939, 2.41769691578, 1.57750620148, 0.78896669122, 0.0097041, -9.69041021875e-05},  // MIDI 39
    HammerPreset{1.83086253625e+19, 318761.09555, 2.32410984379, 39.2999268334, 5.14246320381, 2.45757009513, 1.61667445683, 0.789028770014, 0.009634, -9.94000349481e-05},  // MIDI 40
    HammerPreset{3.09363114974e+19, 417603.491112, 4.00831322361, 35.2516917672, 5.21462388212, 2.44817823536, 1.62930657254, 0.810520898878, 0.0095641, -0.000141677382787},  // MIDI 41
    HammerPreset{2.70158172408e+20, 1612931.50992, 4.79542548161, 32.2919627132, 5.42617462016, 2.47220686626, 1.7081592721, 0.820639552793, 0.0094944, -8.33410396604e-05},  // MIDI 42
    HammerPreset{1.19205748909e+21, 2364284.45159, 4.8610119504, 32.6651261608, 5.63080749806, 2.49970685481, 1.77884466561, 0.801668586098, 0.0094249, -6.4170804766e-05},  // MIDI 43
    HammerPreset{2.69550334639e+21, 1484457.14381, 5.65780250493, 30.5722817433, 5.70689112757, 2.4186482948, 1.78092483058, 0.76697888569, 0.0093556, -7.44144226127e-05},  // MIDI 44
    HammerPreset{2.30917598291e+21, 1440842.58677, 3.8672163874, 28.1104330807, 5.68725315675, 2.23823261956, 1.77914365649, 0.753684846433, 0.0092865, -6.23805667313e-05},  // MIDI 45
    HammerPreset{8.3853281128e+20, 1315746.52895, 3.74477742445, 30.9201466664, 5.55940564079, 2.13115292583, 1.74361856527, 0.78397726502, 0.0092176, -6.56586095084e-05},  // MIDI 46
    HammerPreset{2.05746243457e+20, 235749.468223, 3.376147697, 29.4134184667, 5.3318978151, 2.11621801165, 1.62679044939, 0.810408836341, 0.0091489, -5.269308545e-05},  // MIDI 47
    HammerPreset{1.44875631151e+19, 77625.9945432, 3.29138819921, 26.6205540825, 5.02336056677, 2.04273528386, 1.50440683859, 0.794756202943, 0.0090804, -6.37478055802e-05},  // MIDI 48
    HammerPreset{1.94197195099e+18, 76335.343837, 5.12458315846, 28.7434163259, 4.78787637622, 1.90868313949, 1.43147535346, 0.779681773405, 0.0090121, -6.2806313395e-05},  // MIDI 49
    HammerPreset{1.09252606132e+18, 44097.5014844, 2.08784844498, 26.4140837749, 4.75777849303, 1.7665349973, 1.45580748673, 0.776330871603, 0.008944, -4.16355793184e-05},  // MIDI 50
    HammerPreset{2.15339417951e+18, 83457.9946799, 4.2219353228, 27.1273467111, 4.78739346586, 1.68226393051, 1.55375237691, 0.788663104904, 0.0088761, -5.72457250329e-05},  // MIDI 51
    HammerPreset{1.99910863391e+18, 196003.572152, 3.59281318911, 31.7845508829, 4.80939333133, 1.65090988139, 1.6513739311, 0.788851264288, 0.0088084, -5.67521213917e-05},  // MIDI 52
    HammerPreset{6.05655546297e+18, 256406.626699, 1.55332182867, 21.1563127178, 4.90671427297, 1.67347519019, 1.68270195371, 0.7608748089, 0.0087409, -3.25426048289e-05},  // MIDI 53
    HammerPreset{1.25811838756e+19, 395796.470854, 0.738748456995, 21.5790650408, 5.0068813294, 1.89946525684, 1.67608432359, 0.752596233909, 0.0086736, -3.81005451984e-05},  // MIDI 54
    HammerPreset{1.04260284885e+19, 387143.601204, 1.84331128269, 27.9860962212, 4.98847216481, 2.1735206265, 1.68104717617, 0.763397936992, 0.0086065, -5.35087040523e-05},  // MIDI 55
    HammerPreset{5.31889066379e+18, 440714.218483, 2.9640174542, 28.2328333013, 4.89380592203, 2.28665214119, 1.68378930656, 0.771083753699, 0.0085396, -6.10062309639e-05},  // MIDI 56
    HammerPreset{3.8991327591e+18, 624999.367894, 1.96576664678, 22.8379379543, 4.84466641641, 2.30547103254, 1.65171092866, 0.800300960365, 0.0084729, -4.26791313419e-05},  // MIDI 57
    HammerPreset{2.81902105616e+18, 240588.237061, 4.60570255449, 26.1595000027, 4.78346141945, 2.36680526719, 1.63177387398, 0.841895192828, 0.0084064, -5.21700552012e-05},  // MIDI 58
    HammerPreset{1.48173314182e+18, 338288.593047, 1.02056918203, 25.23717991, 4.68932935519, 2.46659275508, 1.66364294526, 0.829070378947, 0.0083401, -2.56516875243e-05},  // MIDI 59
    HammerPreset{1.20291840144e+18, 494809.113567, 1.10908032934, 22.0169056935, 4.69763023758, 2.57531514807, 1.73518653837, 0.791491183682, 0.008274, -3.77659084397e-05},  // MIDI 60
    HammerPreset{2.97823105717e+18, 1429908.23488, 2.7775221225, 27.9156157484, 4.80781968524, 2.64854148324, 1.86148438256, 0.793149852153, 0.0082081, -4.2695280883e-05},  // MIDI 61
    HammerPreset{9.33515253331e+18, 3804401.88228, 4.7913686661, 25.1660042169, 4.90881733218, 2.62500877784, 1.97065263392, 0.815715922247, 0.0081424, -4.03540187213e-05},  // MIDI 62
    HammerPreset{3.33080492707e+19, 2369795.75647, 2.98308433682, 25.99899469, 5.03476734492, 2.47031578862, 1.96340698925, 0.831190372432, 0.0080769, -5.06830022518e-05},  // MIDI 63
    HammerPreset{7.05858908872e+19, 1204200.93663, 3.70802288176, 25.8393068801, 5.14725245202, 2.20832738139, 1.85234012253, 0.817900256065, 0.0080116, -5.46829029568e-05},  // MIDI 64
    HammerPreset{5.28088613703e+19, 466769.92944, 2.94548563106, 25.8865441327, 5.1022010374, 1.99599348054, 1.75397644688, 0.799847676124, 0.0079465, -0.000145142204853},  // MIDI 65
    HammerPreset{1.6827420192e+19, 618369.897105, 4.24950414417, 26.2119102199, 4.96939458864, 1.98390068574, 1.73670544127, 0.78487917451, 0.0078816, -0.000115104965335},  // MIDI 66
    HammerPreset{9.06969848954e+18, 744146.436259, 2.6688840612, 25.0071588243, 4.8984659227, 2.08935279964, 1.72980213548, 0.77563580592, 0.0078169, -5.71533919881e-05},  // MIDI 67
    HammerPreset{1.34542366837e+19, 462859.430068, 4.26533788213, 23.9939244608, 4.92771067589, 2.14193759332, 1.72322315434, 0.765569236097, 0.0077524, -4.33189041665e-05},  // MIDI 68
    HammerPreset{3.3479137075e+19, 349539.006332, 2.69579120257, 22.4760237905, 5.03854920411, 2.12131931738, 1.6960995783, 0.75790325128, 0.0076881, -5.24611276553e-05},  // MIDI 69
    HammerPreset{1.88125157346e+20, 462823.272766, 3.29182989912, 21.4138626342, 5.22611414597, 2.09425915356, 1.6819033962, 0.749823099457, 0.007624, -4.28785081806e-05},  // MIDI 70
    HammerPreset{1.19523779118e+21, 262730.399132, 3.02943917924, 24.5967599624, 5.43473834035, 2.14286837587, 1.63414329327, 0.741435774512, 0.0075601, -8.11549738401e-05},  // MIDI 71
    HammerPreset{4.15578594012e+21, 327152.881339, 3.80004688879, 22.2157020775, 5.60002039788, 2.2772175074, 1.57834228461, 0.735617111497, 0.0074964, -7.48231411051e-05},  // MIDI 72
    HammerPreset{6.91932963346e+21, 426419.877375, 3.2653735401, 23.6357916739, 5.66216566821, 2.37752616899, 1.58761438073, 0.746157564391, 0.0074329, -5.51166470958e-05},  // MIDI 73
    HammerPreset{3.98517504672e+21, 267406.158826, 3.0916807582, 21.8643935308, 5.58515053626, 2.41528923023, 1.6909645123, 0.759977525464, 0.0073696, -3.40807076608e-05},  // MIDI 74
    HammerPreset{1.35539073976e+21, 1043628.64232, 3.62941499761, 23.2872028291, 5.43308454427, 2.44830842497, 1.851704686, 0.766285029711, 0.0073065, -3.84660155355e-05},  // MIDI 75
    HammerPreset{1.92622779627e+20, 2256882.81384, 1.54673126708, 21.4998732961, 5.20399927168, 2.41900746695, 1.9377698801, 0.783451063761, 0.0072436, -3.53734170468e-05},  // MIDI 76
    HammerPreset{3.60551896733e+19, 1740069.86187, 2.72593196205, 25.6361249991, 4.98846806538, 2.336881587, 1.87192558247, 0.819054597983, 0.0071809, -2.62197692688e-05},  // MIDI 77
    HammerPreset{2.0319390508e+19, 385017.840365, 3.56597481611, 21.1262935454, 4.90959889104, 2.27376879685, 1.74095522556, 0.833786831943, 0.0071184, -2.08366966287e-05},  // MIDI 78
    HammerPreset{2.70304438908e+19, 333204.351113, 2.93778591247, 22.4657998721, 4.95015522978, 2.28867922868, 1.68783777912, 0.82666779629, 0.0070561, -2.63466522507e-05},  // MIDI 79
    HammerPreset{2.01406157963e+19, 810208.179239, 4.66246104354, 20.6956677999, 4.92739567587, 2.44208327708, 1.74643660529, 0.789432967705, 0.006994, -3.71491330704e-05},  // MIDI 80
    HammerPreset{7.65920808025e+18, 3254646.82757, 5.09475391607, 23.9578580978, 4.82215223877, 2.56470397575, 1.88804256835, 0.759373821503, 0.0069321, -5.65952730224e-05},  // MIDI 81
    HammerPreset{9.69375116876e+18, 3475672.18973, 3.0607607207, 21.3878430914, 4.8128422123, 2.54958789556, 1.97410168331, 0.757344353993, 0.0068704, -3.97501710497e-05},  // MIDI 82
    HammerPreset{3.52350163621e+19, 3008116.8505, 1.71989324322, 18.7741548293, 4.95822815771, 2.48946216533, 1.97840710945, 0.763572355606, 0.0068089, -2.80583328194e-05},  // MIDI 83
    HammerPreset{2.52346163895e+20, 4835980.26307, 2.41705464632, 19.5947137301, 5.1896391866, 2.48587807784, 1.96738727913, 0.759852763097, 0.0067476, -4.10829509604e-05},  // MIDI 84
    HammerPreset{9.11605412669e+20, 4071954.91555, 2.55583148843, 21.5423226242, 5.347430544, 2.53881064712, 1.97375689409, 0.74574036088, 0.0066865, -4.64706395524e-05},  // MIDI 85
    HammerPreset{1.29911829327e+21, 4030239.95118, 4.91704701955, 21.199729105, 5.38066929043, 2.60202721757, 1.95275484686, 0.763473061333, 0.0066256, -4.36511056284e-05},  // MIDI 86
    HammerPreset{1.66401399799e+21, 1662423.01281, 4.55222772533, 21.0436563843, 5.4128217747, 2.59183053025, 1.86763398198, 0.806784113439, 0.0065649, -3.7928141205e-05},  // MIDI 87
    HammerPreset{4.78872087411e+21, 323815.222049, 2.00728185241, 20.6222953605, 5.51498491617, 2.48274729869, 1.72104741638, 0.825016527325, 0.0065044, -5.3056483016e-05},  // MIDI 88
    HammerPreset{1.11431733284e+22, 88306.4846515, 3.02689484137, 24.5394324977, 5.62119113328, 2.30632363875, 1.59139570297, 0.809832449626, 0.0064441, -5.10067899288e-05},  // MIDI 89
    HammerPreset{2.1467483623e+22, 66857.74539, 1.81239841328, 21.2139576723, 5.67581661429, 2.17637473548, 1.5542171282, 0.767797106242, 0.006384, -4.50770759898e-05},  // MIDI 90
    HammerPreset{3.03087346701e+22, 245951.077705, 1.03713583045, 26.2526218185, 5.73455089345, 2.21027557819, 1.64875137876, 0.734227174795, 0.0063241, -5.00803389621e-05},  // MIDI 91
    HammerPreset{4.10988590708e+22, 1267462.13502, 0.570362374096, 21.9323677239, 5.77971601378, 2.34555635797, 1.80519739425, 0.724944535076, 0.0062644, -4.58401271204e-05},  // MIDI 92
    HammerPreset{2.47550109362e+22, 2401181.21523, 2.06870253593, 26.6025610843, 5.6836542421, 2.46012924543, 1.86668315037, 0.727860138668, 0.0062049, -5.46223765275e-05},  // MIDI 93
    HammerPreset{5.09745438073e+21, 1295813.25068, 2.68413985791, 18.765082944, 5.4767812401, 2.48441085215, 1.84351492016, 0.749017042135, 0.0061456, -4.40839881267e-05},  // MIDI 94
    HammerPreset{9.53264159811e+20, 1273793.79349, 4.04450415038, 20.9411480072, 5.29775344247, 2.37402879555, 1.80976881891, 0.784201553926, 0.0060865, -4.30413573051e-05},  // MIDI 95
    HammerPreset{5.00886621257e+20, 654997.951258, 4.54776062685, 21.6198813525, 5.20558028662, 2.18188391562, 1.74017339915, 0.792347378204, 0.0060276, -3.20406072321e-05},  // MIDI 96
    HammerPreset{6.38318441143e+20, 350693.000742, 3.86950635439, 21.1464707465, 5.21889630676, 2.04708460856, 1.67881007955, 0.76863945222, 0.0059689, -3.38654598586e-05},  // MIDI 97
    HammerPreset{6.76824527473e+20, 388313.655311, 4.29273073948, 18.8457290445, 5.23663791848, 2.00910094401, 1.72053329875, 0.750055536868, 0.0059104, -3.00130231674e-05},  // MIDI 98
    HammerPreset{5.27519557108e+20, 1395370.75036, 1.37346258138, 25.2976644765, 5.21336489289, 1.93907677528, 1.80741270252, 0.752896801988, 0.0058521, -5.03653594614e-05},  // MIDI 99
    HammerPreset{4.24032406187e+20, 1387147.09661, 1.25319881825, 25.1594282591, 5.18580099943, 1.81260481036, 1.83665334036, 0.750740469208, 0.005794, -4.99108661826e-05},  // MIDI 100
    HammerPreset{7.40432430877e+20, 1731186.82269, 1.20699664838, 24.2410095938, 5.24537421261, 1.72919044337, 1.84769529021, 0.742154632073, 0.0057361, -4.79571150316e-05},  // MIDI 101
    HammerPreset{3.53702785333e+21, 1296549.72386, 1.41797975196, 24.0219773005, 5.42032229641, 1.76379022909, 1.81346414965, 0.744184015328, 0.0056784, -4.67711580002e-05},  // MIDI 102
    HammerPreset{2.77773610086e+22, 493483.981895, 4.21067971479, 15.6703613767, 5.61085009355, 1.90832208452, 1.70557676263, 0.756992091208, 0.0056209, -2.27752187732e-05},  // MIDI 103
    HammerPreset{3.66101160631e+22, 295164.27301, 3.5910145855, 17.0294442298, 5.67029076271, 2.10900400858, 1.6233261335, 0.753285727058, 0.0055636, -2.07577336328e-05},  // MIDI 104
    HammerPreset{2.66407039403e+22, 333379.555005, 2.26129253675, 16.0621991637, 5.62102584522, 2.24881190183, 1.63207079832, 0.743638675314, 0.0055065, -2.58049619446e-05},  // MIDI 105
    HammerPreset{1.00105475962e+22, 565928.906547, 4.45972613332, 19.0633020496, 5.51171770059, 2.32450129811, 1.69962250219, 0.738250585904, 0.0054496, -4.19822327871e-05},  // MIDI 106
    HammerPreset{1.81498777858e+21, 1734900.42258, 2.68297604694, 15.3194641998, 5.36195538131, 2.42473764446, 1.79906929787, 0.737991989173, 0.0053929, -2.89087289707e-05},  // MIDI 107
    HammerPreset{1.12747490485e+21, 2065125.04097, 3.16029681519, 15.4911603298, 5.27542872582, 2.65325822518, 1.90840588135, 0.755604694289, 0.0053364, -3.19222563179e-05},  // MIDI 108
}};

}  // namespace Parameters::Hammer::LegacyFit

class HammerPresets {
    HammerPresets(const HammerPresets&) = delete;
    HammerPresets& operator=(const HammerPresets&) = delete;
public:
    using Preset = Parameters::Hammer::LegacyFit::HammerPreset;
    using PresetTable = std::array<Preset, 88>;
    constexpr HammerPresets() noexcept = default;
    const Preset* find_preset(int midi_n) const noexcept {
        if (midi_n < 21 || midi_n > 108) return nullptr;
        return &sharedPresets()[static_cast<std::size_t>(midi_n - 21)];
    }
    const PresetTable& getPresetTable() const noexcept { return sharedPresets(); }
private:
    static constexpr const PresetTable& sharedPresets() noexcept {
        return Parameters::Hammer::LegacyFit::kHammerPresets;
    }
};

#endif /* hammer_preset_hpp */
#ifndef string_impedance_preset_hpp
#define string_impedance_preset_hpp

#include <array>
#include <cstddef>

namespace Parameters::String::RT425Wrapped {

struct StringImpedancePreset {
    double characteristic_impedance_kg_s;
};


// Characteristic transverse-wave impedance:
//
//      A  = pi * d^2 / 4
//      mu = rho * A
//      Z0 = sqrt(T0 * mu)
//
// Unit:
//
//      kg / s
//      == N / (m / s)
//
// Source:
// Chabassier & Duruflé,
// "Physical parameters for piano modeling",
// INRIA Technical Report RT-425, Appendix A,
// wrapped-string piano.
//
// MIDI 24-107:
// directly derived from the RT-425 values of
// diameter d, density rho and tension T0.
//
// MIDI 21-23 and MIDI 108:
// RT-425 Appendix A does not provide these four notes.
// They are boundary extrapolations from neighboring
// RT-425 characteristic impedances and are marked below.

inline constexpr std::array<
    StringImpedancePreset,
    88
> kStringImpedancePresets{{

    // RT-425 boundary extrapolation
    StringImpedancePreset{12.3988206919},  // MIDI 21 A0
    StringImpedancePreset{12.8891815684},  // MIDI 22 A#0
    StringImpedancePreset{13.1332105294},  // MIDI 23 B0

    // RT-425 Appendix A
    StringImpedancePreset{13.0839327734},  // MIDI 24 C1
    StringImpedancePreset{12.8890542964},  // MIDI 25
    StringImpedancePreset{12.3334372967},  // MIDI 26
    StringImpedancePreset{11.5713724036},  // MIDI 27
    StringImpedancePreset{10.6439459031},  // MIDI 28
    StringImpedancePreset{9.67353066531},  // MIDI 29
    StringImpedancePreset{8.69475953779},  // MIDI 30
    StringImpedancePreset{7.75861813737},  // MIDI 31
    StringImpedancePreset{6.90352041365},  // MIDI 32
    StringImpedancePreset{6.12894607314},  // MIDI 33
    StringImpedancePreset{5.42385158532},  // MIDI 34
    StringImpedancePreset{4.83877578739},  // MIDI 35
    StringImpedancePreset{4.35742291689},  // MIDI 36
    StringImpedancePreset{3.96601763469},  // MIDI 37
    StringImpedancePreset{3.66669615581},  // MIDI 38
    StringImpedancePreset{3.44305916601},  // MIDI 39
    StringImpedancePreset{3.25519545620},  // MIDI 40

    // transition from wrapped equivalent strings
    // toward ordinary steel strings
    StringImpedancePreset{2.48500589816},  // MIDI 41
    StringImpedancePreset{2.42870645282},  // MIDI 42
    StringImpedancePreset{2.36987715335},  // MIDI 43
    StringImpedancePreset{2.36555046436},  // MIDI 44
    StringImpedancePreset{2.30916471578},  // MIDI 45
    StringImpedancePreset{2.30054038444},  // MIDI 46
    StringImpedancePreset{2.29927159747},  // MIDI 47
    StringImpedancePreset{2.29950268773},  // MIDI 48
    StringImpedancePreset{2.29970769619},  // MIDI 49
    StringImpedancePreset{2.29838304084},  // MIDI 50
    StringImpedancePreset{2.29704308482},  // MIDI 51
    StringImpedancePreset{2.28984360988},  // MIDI 52
    StringImpedancePreset{2.27965770670},  // MIDI 53
    StringImpedancePreset{2.26948218038},  // MIDI 54
    StringImpedancePreset{2.25350012030},  // MIDI 55
    StringImpedancePreset{2.23460609259},  // MIDI 56
    StringImpedancePreset{2.21576495128},  // MIDI 57
    StringImpedancePreset{2.19482505542},  // MIDI 58
    StringImpedancePreset{2.17249421243},  // MIDI 59
    StringImpedancePreset{2.15023958991},  // MIDI 60 C4
    StringImpedancePreset{2.12661526438},  // MIDI 61
    StringImpedancePreset{2.10239760722},  // MIDI 62
    StringImpedancePreset{2.08038575212},  // MIDI 63
    StringImpedancePreset{2.05845122567},  // MIDI 64
    StringImpedancePreset{2.03659430135},  // MIDI 65
    StringImpedancePreset{2.01832734084},  // MIDI 66
    StringImpedancePreset{2.00011508361},  // MIDI 67
    StringImpedancePreset{1.98336683563},  // MIDI 68
    StringImpedancePreset{1.96874177635},  // MIDI 69 A4
    StringImpedancePreset{1.95347555206},  // MIDI 70
    StringImpedancePreset{1.94238693842},  // MIDI 71
    StringImpedancePreset{1.93063556045},  // MIDI 72
    StringImpedancePreset{1.92096807626},  // MIDI 73
    StringImpedancePreset{1.91131151156},  // MIDI 74
    StringImpedancePreset{1.90510595470},  // MIDI 75
    StringImpedancePreset{1.89683187889},  // MIDI 76
    StringImpedancePreset{1.89062632204},  // MIDI 77
    StringImpedancePreset{1.88370791961},  // MIDI 78
    StringImpedancePreset{1.87749789350},  // MIDI 79
    StringImpedancePreset{1.87263363374},  // MIDI 80
    StringImpedancePreset{1.86775947868},  // MIDI 81
    StringImpedancePreset{1.86154052370},  // MIDI 82
    StringImpedancePreset{1.85324858372},  // MIDI 83
    StringImpedancePreset{1.84702962875},  // MIDI 84
    StringImpedancePreset{1.83873768877},  // MIDI 85
    StringImpedancePreset{1.83044574880},  // MIDI 86
    StringImpedancePreset{1.82084619918},  // MIDI 87
    StringImpedancePreset{1.81125761501},  // MIDI 88
    StringImpedancePreset{1.79831447070},  // MIDI 89
    StringImpedancePreset{1.78746145235},  // MIDI 90
    StringImpedancePreset{1.77250647871},  // MIDI 91
    StringImpedancePreset{1.75836059104},  // MIDI 92
    StringImpedancePreset{1.74425126497},  // MIDI 93
    StringImpedancePreset{1.72685989245},  // MIDI 94
    StringImpedancePreset{1.70622057731},  // MIDI 95
    StringImpedancePreset{1.68895544985},  // MIDI 96
    StringImpedancePreset{1.66721274056},  // MIDI 97
    StringImpedancePreset{1.64681393361},  // MIDI 98
    StringImpedancePreset{1.62400957623},  // MIDI 99
    StringImpedancePreset{1.60055424045},  // MIDI 100
    StringImpedancePreset{1.57796485708},  // MIDI 101
    StringImpedancePreset{1.55227685138},  // MIDI 102
    StringImpedancePreset{1.52476666739},  // MIDI 103
    StringImpedancePreset{1.49936735910},  // MIDI 104
    StringImpedancePreset{1.47095957610},  // MIDI 105
    StringImpedancePreset{1.44393685561},  // MIDI 106
    StringImpedancePreset{1.41467684717},  // MIDI 107 B7

    // RT-425 boundary extrapolation
    StringImpedancePreset{1.38608153033},  // MIDI 108 C8
}};

}  // namespace Parameters::String::RT425Wrapped



class StringImpedancePresets {
    StringImpedancePresets(
        const StringImpedancePresets&
    ) = delete;

    StringImpedancePresets&
    operator=(
        const StringImpedancePresets&
    ) = delete;

public:
    using Preset =
        Parameters::String::RT425Wrapped::
            StringImpedancePreset;

    using PresetTable =
        std::array<
            Preset,
            88
        >;

    constexpr StringImpedancePresets() noexcept =
        default;


    const Preset* find_preset(
        int midi_n
    ) const noexcept
    {
        constexpr int kMidiMin = 21;
        constexpr int kMidiMax = 108;

        if (
            midi_n < kMidiMin ||
            midi_n > kMidiMax
        ) {
            return nullptr;
        }

        const std::size_t index =
            static_cast<std::size_t>(
                midi_n - kMidiMin
            );

        return &sharedPresets()[index];
    }


    double get_characteristic_impedance(
        int midi_n
    ) const noexcept
    {
        const Preset* preset =
            find_preset(midi_n);

        if (preset == nullptr) {
            return 0.0;
        }

        return
            preset->
                characteristic_impedance_kg_s;
    }


    const PresetTable&
    getPresetTable() const noexcept
    {
        return sharedPresets();
    }


private:
    static constexpr const PresetTable&
    sharedPresets() noexcept
    {
        return
            Parameters::String::RT425Wrapped::
                kStringImpedancePresets;
    }
};


#endif /* string_impedance_preset_hpp */

class Configuration {
    Configuration(const Configuration&) = delete;
    Configuration& operator=(const Configuration&) = delete;
public:
    
    static constexpr TunningPresets tuning_presets{};
    static constexpr HammerPresets hammer_presets{};
    static constexpr StringImpedancePresets string_impedance_presets{};

    constexpr Configuration() noexcept = default;
    
};



#endif /* configuration_hpp */

class StringModel {
    
public:
    
    struct SpatialPort {
        int size = 0;
        int max_index = 0;
        double position = 0.0;
        double accurate_index = 0.0;
        
        int index_a = 0;
        int index_b = 0;
        float weight_a = 1.0f;
        float weight_b = 0.0f;
        
        SpatialPort(int size, double position) : size(size), position(position) {
            if(size < 2)
                throw std::runtime_error("string_model: size is too small, size: " + std::to_string(size));
            if(position > 1.0 || position <= 0.0)
                throw std::runtime_error("string_model: position isn't good, position: " + std::to_string(position));
            max_index = size - 1;
            accurate_index = max_index * position;
            
            index_a = std::ceil(accurate_index);
            index_b = index_a - 1;
            
            weight_a = accurate_index - index_b;
            weight_b = 1 - weight_a;
        }
    };
    
public:
    // ======================== ========================
    // Configuration
    // 配置
    // ======================== ========================
    const Configuration *configuration_ = nullptr;
    
    // ======================== ========================
    // Basic member value
    // 基础成员变量
    // ======================== ========================
    int midi_n_ = 69;
    double f0 = 440.0;
    double sample_rate_ = 44100.0;
    double internal_sample_rate_ = 44100.0;
    
    // ======================== ========================
    // Filters
    // 滤波器 (为了计算 group delay 放在前面)
    // ======================== ========================
    LossFilter loss_filter;
    DispersionFilter dispersion_filter;
    double loss_phase_delay = 0.0;
    double dispersion_phase_delay = 0.0;
    
    // ======================== ========================
    // Delay data
    // 延迟数据
    // ======================== ========================
    double delay = 0.0;
    int delay_int = 0;
    double delay_frac = 0.0;
    int traveling_wave_max_index = 0;

    // ======================== ========================
    // Waveguide
    // 波导
    // ======================== ========================
    std::vector<float> left;
    std::vector<float> right;
    int left_head = 0;
    int right_head = 0;
    float* left_boundary_point = nullptr;
    float* right_boundary_point = nullptr;
    
    // ======================== ========================
    // Frac position
    // 分数格点
    // ======================== ========================
    double strike_point = 1.0 / 9.4;
    SpatialPort strike_port;
    
    // ======================== ========================
    // Fine-tuning coefficient
    // 微调系数
    // 弦特性阻抗
    // ======================== ========================
    double z_ = 0.0;
    
    // ======================== ========================
    // Damper
    // 制音器
    // ======================== ========================
    Damper damper;
    bool damper_active = false;
    
    // ======================== ========================
    // Filters
    // 滤波器
    // ======================== ========================
    FractionalFilter fractional_filter;
    
    // ======================== ========================
    // State
    // 状态
    // ======================== ========================
    bool is_active = false;
    
    // ======================== ========================
    // Pre-compute
    // 预计算
    // ======================== ========================
    double c_z = 0.0;

    
public:
    
    StringModel(double sample_rate,
                int midi_n,
                TunningPresets::Temperament temperament,
                TunningPresets::StringIndex string_index,
                const Configuration* configuration) :
        configuration_(configuration),
        midi_n_(midi_n),
        f0(configuration->tuning_presets
           .get_frequency(midi_n_,
                          temperament,
                          string_index)),
        sample_rate_(sample_rate),
        internal_sample_rate_(midi_n_ >= 96 ? 2.0 * sample_rate_ : sample_rate_),
        loss_filter(midi_n_),
        dispersion_filter(f0),
        // loss_filter.get_phase_delay(sample_rate, f0)
        loss_phase_delay(loss_filter.get_phase_delay(internal_sample_rate_, f0)),
//        dispersion_phase_delay(dispersion_filter.get_phase_delay(sample_rate_, f0)),
//        delay((internal_sample_rate_ / f0 - loss_phase_delay) / 2.0),
//        delay((sample_rate_ / f0 - loss_phase_delay - dispersion_phase_delay) / 2.0),
        delay(dispersion_filter.get_loop_delay_samples() / 2.0),
        // delay_int 是数组节点数；真实单程整数延迟为 delay_int - 1。
        delay_int(static_cast<int>(std::floor(delay)) + 1),
        // fractional filter 每圈一次，因此承担完整 round-trip residual。
        delay_frac(2.0 * (delay - std::floor(delay))),
        traveling_wave_max_index(delay_int - 1),
        strike_port(delay_int, strike_point),
        fractional_filter(delay_frac, 2.0 *
                          std::numbers::pi_v<double> * f0 / internal_sample_rate_)
    {
        
        if(delay_int < 4)
            throw std::runtime_error("string_model: delay_int is too small: " + std::to_string(delay_int));
        if(strike_port.index_a + 1 > traveling_wave_max_index ||
           strike_port.index_a - 1 < 0) {
            throw std::runtime_error("string_model: next_index doesn't exist: " + std::to_string(delay_int));
        }
        if (std::abs(dispersion_filter.design_sample_rate() - internal_sample_rate_) > 0.5) {
            throw std::runtime_error("string_model: dispersion preset sample rate mismatch");
        }
        
//        std::cout
//        << "midi_n: " << midi_n_
//        << ", delay: " << delay << '\n';

        left.resize(delay_int, 0.0f);
        right.resize(delay_int, 0.0f);
            
        left_head = 0;
        right_head = 0;
        
        left_boundary_point = &left[left_head];
        right_boundary_point = &right[get_i(traveling_wave_max_index, right_head)];
        
        z_ = configuration->string_impedance_presets.get_characteristic_impedance(midi_n_);
        c_z = 1.0 / (2.0 * z_);
    }
    
    inline void propagate() {
        // ======================== ========================
        // Reverse moving boundary index
        // 反向移动边界下标
        // ======================== ========================
        if(left_head == traveling_wave_max_index)
            left_head = 0;
        else
            left_head++;
        if(right_head == 0)
            right_head = traveling_wave_max_index;
        else
            right_head--;
        
        // ======================== ========================
        // Boundary reflection
        // 边界反射
        // ======================== ========================
        right[get_i(0, right_head)] = -left[get_i(0, left_head)];
        left[get_i(traveling_wave_max_index, left_head)] =
            -right[get_i(traveling_wave_max_index, right_head)];
        
        // ======================== ========================
        // Update boundary point
        // 更新边界指针
        // ======================== ========================
        left_boundary_point = &right[get_i(0, right_head)];
        right_boundary_point = &left[get_i(traveling_wave_max_index, left_head)];
    }
    
    inline int get_i(int real_index, int head) {
        return real_index + head <= traveling_wave_max_index ?
               real_index + head :
               real_index + head - delay_int;
    }

    inline void string_movement(double hammer_force) {

        const bool excited = hammer_force > 0.0;

        if (excited) {
            inject(hammer_force * c_z); // inject(hammer_force / (2 * z_));
        }

        propagate();
        filter();

        check_active();

        if (excited) {
            is_active = true;
        }
    }
    
    inline void inject(double inject_v) {
        left[get_i(strike_port.index_a, left_head)] += inject_v * strike_port.weight_a;
        left[get_i(strike_port.index_b, left_head)] += inject_v * strike_port.weight_b;
        right[get_i(strike_port.index_a, right_head)] += inject_v * strike_port.weight_a;
        right[get_i(strike_port.index_b, right_head)] += inject_v * strike_port.weight_b;
    }
    
    inline double get_bridge_force() {
        return -2.0 * z_ * static_cast<double>(*right_boundary_point);
    }
    
    inline double get_string_vs() {
        return strike_port.weight_a * left[get_i(strike_port.index_a, left_head)] + strike_port.weight_b * left[get_i(strike_port.index_b, left_head)] + strike_port.weight_a * right[get_i(strike_port.index_a, right_head)] + strike_port.weight_b * right[get_i(strike_port.index_b, right_head)];
    }

    inline void system_reset() {
        left_head = 0;
        right_head = 0;

        is_active = false;
        damper_active = false;

        inactive_probe_count_ = 0;
        activity_probe_counter_ = 0;

        std::fill(left.begin(), left.end(), 0.0f);
        std::fill(right.begin(), right.end(), 0.0f);

        left_boundary_point = &left[left_head];
        right_boundary_point = &right[get_i(traveling_wave_max_index, right_head)];

        loss_filter.system_reset();
        fractional_filter.system_reset();
        damper.system_reset();
        dispersion_filter.system_reset();
    }
    

private:
    inline void filter() {
        fractional_filter.process(*left_boundary_point);
        loss_filter.process(*left_boundary_point);
        dispersion_filter.process(*left_boundary_point);
        if(damper_active) {
            damper.process(*left_boundary_point);
        }
    }
    
    int inactive_probe_count_ = 0;
    int activity_probe_counter_ = 0;
    inline void check_active() {
        const int probe_interval = midi_n_ >= 96 ? 128 : 64;
        constexpr int k_inactive_probe_count = 8;

        if (++activity_probe_counter_ < probe_interval) {
            return;
        }

        activity_probe_counter_ = 0;

        constexpr float kVelocityThreshold = 1.0e-5f;
        constexpr float kEnergyThreshold = kVelocityThreshold * kVelocityThreshold;

        if (activity_probe() < kEnergyThreshold) {
            if (++inactive_probe_count_ >= k_inactive_probe_count) {
                is_active = false;
                system_reset();
            }
        } else {
            inactive_probe_count_ = 0;
            is_active = true;
        }
    }


    inline float activity_probe() {
        float rail_energy = 0.0f;
        for (int index = 0; index < delay_int; ++index) {
            rail_energy +=
                left[static_cast<std::size_t>(index)] *
                    left[static_cast<std::size_t>(index)]
                + right[static_cast<std::size_t>(index)] *
                    right[static_cast<std::size_t>(index)];
        }
        rail_energy /= static_cast<float>(2 * delay_int);

        float state_energy = std::max(
            loss_filter.state_energy(),
            fractional_filter.state_energy());
        state_energy = std::max(state_energy,
                                static_cast<float>(dispersion_filter.state_energy()));
        if (damper_active) {
            state_energy = std::max(state_energy, damper.state_energy());
        }

        return std::max(rail_energy, state_energy);
    }
    
};

#endif /* string_model_hpp */
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
// [ZERO AI-GENERATED CODE]
// Every line in this file is written and understood by its author. Every result
// is reproducible, every assumption is open to inspection, and every
// implementation stands open to criticism and challenge. AI may be used for
// non-core, replaceable, engineering work; this file, however, contains core
// logic that the author considers necessary to understand firsthand, explain
// line by line, and take full responsibility for, and is therefore implemented
// entirely by hand.
// ---------------------------------------------------------------------------
// [本文件承诺不含任何 AI 生成代码]
// 每一行代码均由作者亲自编写，并确知其意义。一切结果可以复现，一切假设可经受检验，一切实现经得起批评与质疑。
// AI 可用于非核心、可替代的工程工作；
// 本文件承载作者认为必须亲自理解、能够逐行解释并为之负责的核心逻辑，因此刻意保持完全人工实现。
//
// Ziyang Tan
// 2026-09-04
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

#ifndef hammer_model_hpp
#define hammer_model_hpp

#include <cmath>
#include <iostream>


class HammerModel {

public: // 暂时 public
  // ======================== ======================== ========================
  // 双层 Kelvin–Voigt
  // ======================== ======================== ========================

  // ======================== ========================
  // Basic parameters
  // 基本参数
  // ======================== ========================
  int midi_n_ = 69;

  // ======================== ========================
  // Degrees of freedom of parameters
  // 参数自由度
  // ======================== ========================
  const Parameters::Hammer::LegacyFit::HammerPreset *hammer_presets = nullptr;

  // ======================== ========================
  // Cross-function update volume
  // 跨函数更新量
  // ======================== ========================
  double ts_ = 0.0;

  double w_a_1_ = 0.0;
  double w_b_1_ = 0.0;

  double hammer_v_ = 0.0;
  double middle_v_ = 0.0;
  double release_distance = 0.0;

  bool is_contacting_ = false;

  // ======================== ========================
  // Pre-conpute
  // 预计算
  // ======================== ========================
  double inv_mass_ = 0.0;

public:
  HammerModel(double sample_rate, int midi_n,
              const Configuration *configuration)
      : ts_((1.0 / 2.0) / sample_rate), midi_n_(midi_n),
        hammer_presets(configuration->hammer_presets.find_preset(midi_n_)) {
    inv_mass_ = ts_ / hammer_presets->mass_kg;
  }

  inline double hammer_movement(double string_v) {
    if (!is_contacting_)
      return 0.0;

    if (w_a_1_ < 0.0 || w_b_1_ < 0.0) {
      system_reset();
      return 0.0;
    }

    // 算 f (解出 middle_v)
    double hammer_force = solve_f(string_v);

    // 无根、NaN、零力或负力：接触结束。
    if (!std::isfinite(hammer_force) || hammer_force <= 0.0) {
      system_reset();
      return 0.0;
    }

    // 击锤的空间移动
    if (hammer_v_ < 0 || release_distance < 0.0) {
      release_distance += hammer_v_ * ts_;
      if (release_distance > 0.0) {
        release_distance = 0.0;
      }
    }
    // 检查是否接触结束
    if (release_distance < hammer_presets->release_threshold_m) {
      system_reset();
      return 0.0;
    }

    // 更新压缩量
    w_a_1_ = w_a_1_ + (middle_v_ - string_v) * ts_;
    w_b_1_ = w_b_1_ + (hammer_v_ - middle_v_) * ts_;

    constexpr double compression_tolerance = 1.0e-12;

    // 任意一层将进入拉伸状态，说明单边接触已经结束。
    if (!std::isfinite(w_a_1_) || !std::isfinite(w_b_1_) ||
        w_a_1_ < -compression_tolerance || w_b_1_ < -compression_tolerance) {

      system_reset();
      return 0.0;
    }

    // 只消除舍入产生的极小负数。
    w_a_1_ = std::max(0.0, w_a_1_);
    w_b_1_ = std::max(0.0, w_b_1_);

    // 更新击锤的速度
    // 负力不更新！
    if (hammer_force > 0.0)
      // hammer_v_ -= (hammer_force / hammer_presets->mass_kg) * ts_;
      hammer_v_ -= hammer_force * inv_mass_;

    return hammer_force;
  }

  inline void trigger(double hammer_v) {
    if (is_contacting_)
      return;

    system_reset();
    hammer_v_ = hammer_v;
    is_contacting_ = true;
  }

  inline void system_reset() {
    w_a_1_ = 0.0;
    w_b_1_ = 0.0;
    hammer_v_ = 0.0;
    middle_v_ = 0.0;
    release_distance = 0.0;
    is_contacting_ = false;
  }

private:
  inline double signed_pow(double x, double p) {
    return x >= 0 ? std::pow(x, p) : -std::pow(-x, p);
  }
  inline double scope_pow(double w, double exponent) {
    return std::pow(w > 0.0 ? w : 0.0, exponent);
  }

  inline double solve_f(double string_v) {
    double lower_limit = string_v - w_a_1_ / ts_;
    double upper_limit = hammer_v_ + w_b_1_ / ts_;

    if (!(lower_limit <= upper_limit)) {
      return 0.0;
    }

    // 与 middle_v 无关的弹簧力提前计算
    const double spring_a =
        hammer_presets->k_a * scope_pow(w_a_1_, hammer_presets->p1);

    const double spring_b =
        hammer_presets->k_b * scope_pow(w_b_1_, hammer_presets->p3);

    const double c_a = hammer_presets->c_a;
    const double c_b = hammer_presets->c_b;

    const double p2 = hammer_presets->p2;
    const double p4 = hammer_presets->p4;

    // 只计算 residual。
    const auto residual = [&](double middle_v) {
      const double f_a = spring_a + c_a * signed_pow(middle_v - string_v, p2);

      const double f_b = spring_b + c_b * signed_pow(hammer_v_ - middle_v, p4);

      return f_a - f_b;
    };

    // 首先确认根仍然被包含在物理解区间内。
    const double residual_lower = residual(lower_limit);
    const double residual_upper = residual(upper_limit);

    if (!std::isfinite(residual_lower) || !std::isfinite(residual_upper) ||
        residual_lower > 0.0 || residual_upper < 0.0) {
      return 0.0;
    }

    // 上一时间步的 middle_v 通常非常接近当前时间步的根。
    // 如果超出当前物理解区间，则限制回区间内部。
    double middle_v_suppose = std::clamp(middle_v_, lower_limit, upper_limit);

    constexpr int max_iterations = 8;
    constexpr double residual_tolerance = 1.0e-10;
    constexpr double velocity_tolerance = 1.0e-10;
    constexpr double derivative_tolerance = 1.0e-14;

    double final_force = 0.0;

    for (int i = 0; i < max_iterations; ++i) {

      const double velocity_a = middle_v_suppose - string_v;

      const double velocity_b = hammer_v_ - middle_v_suppose;

      const double abs_velocity_a = std::abs(velocity_a);

      const double abs_velocity_b = std::abs(velocity_b);

      // 每层只做一次主要幂运算。
      const double power_a = std::pow(abs_velocity_a, p2);

      const double power_b = std::pow(abs_velocity_b, p4);

      const double damping_a = std::copysign(power_a, velocity_a);

      const double damping_b = std::copysign(power_b, velocity_b);

      const double f_a = spring_a + c_a * damping_a;

      const double f_b = spring_b + c_b * damping_b;

      const double r = f_a - f_b;

      if (!std::isfinite(r)) {
        return 0.0;
      }

      final_force = f_a;

      // 当前点同时用于缩小安全区间。
      if (r > 0.0) {
        upper_limit = middle_v_suppose;
      } else {
        lower_limit = middle_v_suppose;
      }

      // 力已经足够平衡。
      if (std::abs(r) <= residual_tolerance) {
        break;
      }

      // 区间已经足够小。
      if ((upper_limit - lower_limit) <= velocity_tolerance) {
        break;
      }

      // -------------------------------------------------
      // residual 的解析导数
      //
      // d/dv [sgn(v)|v|^p]
      //     = p |v|^(p - 1)
      //
      // 已经计算过 |v|^p，因此使用
      //
      // |v|^(p - 1) = |v|^p / |v|
      //
      // 避免额外的 pow。
      // -------------------------------------------------

      double derivative = 0.0;

      if (abs_velocity_a > 0.0) {
        derivative += c_a * p2 * (power_a / abs_velocity_a);
      }

      if (abs_velocity_b > 0.0) {
        derivative += c_b * p4 * (power_b / abs_velocity_b);
      }

      double next_middle_v;

      // 优先 Newton。
      if (std::isfinite(derivative) && derivative > derivative_tolerance) {

        const double newton = middle_v_suppose - r / derivative;

        // Newton 必须留在当前有根区间内。
        if (std::isfinite(newton) && newton > lower_limit &&
            newton < upper_limit) {

          next_middle_v = newton;

        } else {

          // Newton 不可信时退回二分。
          next_middle_v = (lower_limit + upper_limit) * 0.5;
        }

      } else {

        // 导数过小或异常时退回二分。
        next_middle_v = (lower_limit + upper_limit) * 0.5;
      }

      middle_v_suppose = next_middle_v;
    }

    middle_v_ = middle_v_suppose;

    return final_force;
  }

  //    inline double solve_f(double string_v) {
  //        double lower_limit = string_v - w_a_1_ / ts_;
  //        double upper_limit = hammer_v_ + w_b_1_ / ts_;
  //        double middle_v_suppose = (upper_limit + lower_limit) / 2.0;
  //
  //        // 与 middle_v 无关的弹簧提出来
  //        const double spring_a = hammer_presets->k_a * scope_pow(w_a_1_,
  //        hammer_presets->p1); const double spring_b = hammer_presets->k_b *
  //        scope_pow(w_b_1_, hammer_presets->p3);
  //
  //        const auto residual = [&](double middle_v) {
  //            const double f_a =
  //                spring_a + hammer_presets->c_a * signed_pow(middle_v -
  //                string_v, hammer_presets->p2);
  //
  //            const double f_b =
  //                spring_b + hammer_presets->c_b * signed_pow(hammer_v_ -
  //                middle_v, hammer_presets->p4);
  //
  //            return f_a - f_b;
  //        };
  //
  //        if (!(lower_limit <= upper_limit)) {
  //            return 0.0;
  //        }
  //
  //        const double residual_lower = residual(lower_limit);
  //        const double residual_upper = residual(upper_limit);
  //
  //        if (!std::isfinite(residual_lower) ||
  //            !std::isfinite(residual_upper) ||
  //            residual_lower > 0.0 ||
  //            residual_upper < 0.0) {
  //            return 0.0;
  //        }
  //
  //        for(int i = 0; i < 20; i++) {
  //            if(residual(middle_v_suppose) > 0) {
  //                upper_limit = middle_v_suppose;
  //            } else {
  //                lower_limit = middle_v_suppose;
  //            }
  //            middle_v_suppose = (upper_limit + lower_limit) / 2.0;
  //        }
  //
  //        middle_v_ = middle_v_suppose;
  //
  //        return spring_a + hammer_presets->c_a *  signed_pow(middle_v_suppose
  //        - string_v, hammer_presets->p2);
  //
  //
  //    }
};

#endif /* hammer_model_hpp */
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
// [ZERO AI-GENERATED CODE]
// Every line in this file is written and understood by its author. Every result is
// reproducible, every assumption is open to inspection, and every implementation
// stands open to criticism and challenge.
// AI may be used for non-core, replaceable, engineering work;
// this file, however, contains core logic that the author considers
// necessary to understand firsthand, explain line by line, and take full responsibility for,
// and is therefore implemented entirely by hand.
// ---------------------------------------------------------------------------
// [本文件承诺不含任何 AI 生成代码]
// 每一行代码均由作者亲自编写，并确知其意义。一切结果可以复现，一切假设可经受检验，一切实现经得起批评与质疑。
// AI 可用于非核心、可替代的工程工作；
// 本文件承载作者认为必须亲自理解、能够逐行解释并为之负责的核心逻辑，因此刻意保持完全人工实现。
//
// Ziyang Tan
// 2026-09-15
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

#ifndef bridge_model_hpp
#define bridge_model_hpp

#include <iostream>

class BridgeModel {
    
    
    
public:
    BridgeModel () {
        
        
    }
    
    void process() const {

//        const float z = boundary_point->z;
//        const float y = boundary_point->y;
//
//        boundary_point->z = 0.999848f * z - 0.017452f * y;
//
//        boundary_point->y = 0.017452f * z + 0.999848f * y;
    }
    
};

#endif /* bridge_model_hpp */
#ifndef soundboard_model_hpp
#define soundboard_model_hpp

#include <array>
#include <algorithm>
#include <cmath>
#include <cstddef>

class SoundboardModel {
    static constexpr bool soundboard_active_ = true;
    
    SoundboardModel(const SoundboardModel&) = delete;
    SoundboardModel& operator=(const SoundboardModel&) = delete;

    static constexpr std::size_t kKeyCount = 88;
    static constexpr std::size_t kFDNSize = 8;
    static constexpr std::size_t kMaxDelay = 721;

    // Bank:
    //
    // 37, 87, 181, 271, 359, 492, 687, 721 samples
    //
    // Short delays are essential for preserving the early energy
    // of the piano soundboard impulse response.
    static constexpr std::array<std::size_t, kFDNSize>
    kDelayLengths{
        37,
        87,
        181,
        271,
        359,
        492,
        687,
        721
    };


    // ================================================================
    // Delay line
    // ================================================================

    struct DelayLine {
        std::array<float, kMaxDelay> data{};

        std::size_t length = 1;
        std::size_t index = 0;

        inline void set_length(std::size_t new_length) noexcept {
            length = std::clamp<std::size_t>(
                new_length,
                1,
                kMaxDelay
            );

            index = 0;
            data.fill(0.0f);
        }

        inline float read() const noexcept {
            return data[index];
        }

        inline void write(float x) noexcept {
            data[index] = x;

            ++index;

            if (index >= length) {
                index = 0;
            }
        }

        inline void reset() noexcept {
            data.fill(0.0f);
            index = 0;
        }
    };


    // ================================================================
    // Frequency-dependent loss
    //
    // H(z) =
    //
    //           g (1 - a)
    //       -----------------
    //        1 - a z^-1
    //
    //
    // DC gain:
    //
    // H(1) = g
    //
    // Higher frequencies have slightly smaller gain.
    //
    // This follows Bank's structure:
    // one-pole loss filter in series with every delay line.
    //
    // Exact coefficients in Bank were obtained from measured
    // frequency-dependent decay times and are not published
    // numerically in the thesis.
    // ================================================================

    struct LossFilter {
        float gain = 1.0f;
        float pole = 0.0f;

        float state = 0.0f;

        inline float process(float x) noexcept {
            const float y =
                gain * (1.0f - pole) * x
                + pole * state;

            state = y;

            return y;
        }

        inline void reset() noexcept {
            state = 0.0f;
        }
    };


    // ================================================================
    // Bridge-region shaping
    //
    // Bank notes that different excitation positions on the bridge
    // produce different force-pressure responses.
    //
    // However, they excite essentially the same soundboard modes.
    // Therefore a shared FDN is used after position-dependent
    // shaping filters.
    //
    // This low/high decomposition is a provisional low-order
    // approximation until measured P/F FIR coefficients are available.
    // ================================================================

    struct RegionShaper {
        float alpha = 0.0f;

        float low_gain = 1.0f;
        float high_gain = 1.0f;

        float low_state = 0.0f;

        inline float process(float x) noexcept {
            low_state =
                alpha * low_state
                + (1.0f - alpha) * x;

            const float low = low_state;
            const float high = x - low;

            return
                low_gain * low
                + high_gain * high;
        }

        inline void reset() noexcept {
            low_state = 0.0f;
        }
    };


    std::array<DelayLine, kFDNSize> delay_lines_{};
    std::array<LossFilter, kFDNSize> loss_filters_{};

    RegionShaper bass_shaper_{};
    RegionShaper middle_shaper_{};
    RegionShaper treble_shaper_{};

    double sample_rate_ = 44100.0;


public:

    mutable std::array<float, kKeyCount> bridge_force{};


    explicit SoundboardModel(
        double sample_rate = 44100.0
    ) :
        sample_rate_(sample_rate)
    {
        initialize_delays();
        initialize_losses();
        initialize_region_shapers();
    }


    // ================================================================
    // Per-sample soundboard radiation
    //
    // bridge force
    //
    //      ↓
    //
    // spatial region shaping
    //
    //      ↓
    //
    // sum
    //
    //      ↓
    //
    // 8-line high-modal-density FDN
    //
    //      ↓
    //
    // radiation proxy
    // ================================================================

    inline float get_sample() noexcept {
        if(!soundboard_active_) {
            float sum = 0.0;
            for(int i = 0; i < bridge_force.size(); i++) {
                sum += bridge_force[i];
            }
            return sum;
        }

        // ------------------------------------------------------------
        // 1. Spatial bridge-force reduction
        //
        // Current split is provisional.
        //
        // MIDI 21...40  -> bass
        // MIDI 41...75  -> middle
        // MIDI 76...108 -> treble
        // ------------------------------------------------------------

        float bass_force = 0.0f;
        float middle_force = 0.0f;
        float treble_force = 0.0f;

        for (std::size_t i = 0; i < 20; ++i) {
            bass_force += bridge_force[i];
        }

        for (std::size_t i = 20; i < 55; ++i) {
            middle_force += bridge_force[i];
        }

        for (std::size_t i = 55; i < 88; ++i) {
            treble_force += bridge_force[i];
        }


        // ------------------------------------------------------------
        // 2. Position-dependent shaping
        //
        // The treble region intentionally retains more high-frequency
        // energy than the bass region.
        // ------------------------------------------------------------

        const float shaped_force =
            bass_shaper_.process(bass_force)
            +
            middle_shaper_.process(middle_force)
            +
            treble_shaper_.process(treble_force);


        // ------------------------------------------------------------
        // 3. Read delayed states and apply frequency-dependent losses
        //
        // In FDN notation:
        //
        // s(z) = D(z) [ A s(z) + b x(z) ]
        // ------------------------------------------------------------

        std::array<float, kFDNSize> delayed{};

        float delayed_sum = 0.0f;

        for (std::size_t i = 0; i < kFDNSize; ++i) {

            delayed[i] =
                loss_filters_[i].process(
                    delay_lines_[i].read()
                );

            delayed_sum += delayed[i];
        }


        // ------------------------------------------------------------
        // 4. FDN output vector c
        //
        // A sign-alternating normalized vector avoids all delay lines
        // contributing with identical phase/sign.
        // ------------------------------------------------------------

        constexpr float q =
            0.3535533905932738f; // 1 / sqrt(8)

        const float fdn_output =
            q *
            (
                delayed[0]
                - delayed[1]
                + delayed[2]
                - delayed[3]
                + delayed[4]
                - delayed[5]
                + delayed[6]
                - delayed[7]
            );


        // ------------------------------------------------------------
        // 5. Orthogonal feedback matrix
        //
        // Start with the Householder matrix:
        //
        //          A = I - (2/N) J
        //
        // N = 8:
        //
        //          A = I - 0.25 J
        //
        //
        // Bank shifts this circular matrix by one position.
        //
        // Therefore:
        //
        // feedback[i]
        //      = delayed[(i + 1) mod 8]
        //        - 0.25 * sum(delayed)
        //
        //
        // This is mathematically equivalent to the full 8×8
        // matrix multiplication but much cheaper.
        // ------------------------------------------------------------

        const float common =
            0.25f * delayed_sum;


        // ------------------------------------------------------------
        // Input vector b.
        //
        // Normalized equal excitation of all delay lines.
        // ------------------------------------------------------------

        const float input =
            q * shaped_force;


        for (std::size_t i = 0; i < kFDNSize; ++i) {

            const std::size_t shifted =
                (i + 1) & 7;

            const float feedback =
                delayed[shifted]
                - common;

            delay_lines_[i].write(
                feedback + input
            );
        }


        // ------------------------------------------------------------
        // 6. Direct component
        //
        // Eq. (5.4) allows:
        //
        // y = c^T s + d x
        //
        // A modest direct component preserves the immediate
        // bridge-to-air transient while the FDN represents the dense
        // decaying modal field.
        //
        // This value is provisional and should eventually be included
        // in measured force-pressure calibration.
        // ------------------------------------------------------------

        constexpr float direct_gain = 0.18f;

        return
            fdn_output
            + direct_gain * shaped_force;
    }


    inline void system_reset() noexcept {

        bridge_force.fill(0.0f);

        for (auto& delay : delay_lines_) {
            delay.reset();
        }

        for (auto& loss : loss_filters_) {
            loss.reset();
        }

        bass_shaper_.reset();
        middle_shaper_.reset();
        treble_shaper_.reset();
    }


private:

    inline void initialize_delays() noexcept {

        for (std::size_t i = 0; i < kFDNSize; ++i) {
            delay_lines_[i].set_length(
                kDelayLengths[i]
            );
        }
    }


    inline void initialize_losses() noexcept {

        // ============================================================
        // Bank calibrates these filters from measured soundboard
        // decay times in octave bands.
        //
        // The exact numerical coefficients are not published.
        //
        // For this provisional implementation we reproduce the same
        // structure and use a physically meaningful decay mapping:
        //
        // gain =
        //
        //       10 ^ [ -3 * delay_time / T60 ]
        //
        //
        // so that repeated circulation decays by 60 dB in T60.
        //
        // The one-pole term adds frequency-dependent damping.
        // ============================================================

        constexpr double target_t60 = 0.34;

        // Keep this fairly weak.
        //
        // Too high a value makes the piano sound artificially dark.
        constexpr float loss_pole = 0.075f;


        for (std::size_t i = 0; i < kFDNSize; ++i) {

            const double delay_seconds =
                static_cast<double>(kDelayLengths[i])
                / sample_rate_;

            const double gain =
                std::pow(
                    10.0,
                    -3.0 *
                    delay_seconds /
                    target_t60
                );

            loss_filters_[i].gain =
                static_cast<float>(gain);

            loss_filters_[i].pole =
                loss_pole;

            loss_filters_[i].state =
                0.0f;
        }
    }


    inline void initialize_region_shapers() noexcept {

        // One-pole crossover around 1.8 kHz.
        //
        // alpha = exp(-2πfc/Fs)

        constexpr double crossover_hz = 1800.0;

        const float alpha =
            static_cast<float>(
                std::exp(
                    -2.0 *
                    3.14159265358979323846 *
                    crossover_hz /
                    sample_rate_
                )
            );


        // ------------------------------------------------------------
        // These three gain pairs are NOT published Bank coefficients.
        //
        // They only approximate the experimentally observed tendency:
        //
        // low bridge excitation  -> softer radiation
        // high bridge excitation -> brighter radiation
        //
        // Bank explicitly reports this behavior in the measurements.
        // ------------------------------------------------------------

        bass_shaper_.alpha = alpha;
        bass_shaper_.low_gain = 1.00f;
        bass_shaper_.high_gain = 0.82f;


        middle_shaper_.alpha = alpha;
        middle_shaper_.low_gain = 1.00f;
        middle_shaper_.high_gain = 1.00f;


        treble_shaper_.alpha = alpha;
        treble_shaper_.low_gain = 0.95f;
        treble_shaper_.high_gain = 1.28f;
    }
};

#endif /* soundboard_model_hpp */





//
//class SoundboardModel {
//    SoundboardModel(const SoundboardModel&) = delete;
//    SoundboardModel& operator=(const SoundboardModel&) = delete;
//
//    // ================================================================
//    // Reduced soundboard radiation model
//    //
//    // Input:
//    //      bridge force F_b[n]
//    //
//    // Output:
//    //      radiated acoustic pressure proxy p[n]
//    //
//    // This model is intentionally outside the mechanical feedback loop.
//    //
//    // It does NOT model the bridge impedance seen by the strings.
//    // Instead, it approximates the spatially dependent transfer:
//    //
//    //                  P(ω)
//    //      H(ω) = ----------------
//    //                F_bridge(ω)
//    //
//    // using three bridge regions and four damped modes per region.
//    // ================================================================
//
//    static constexpr int kModeCount = 4;
//
//    struct RadiationMode {
//        // Transfer function:
//        //
//        //                 b0
//        // H(z) = -----------------------
//        //         1 + a1 z^-1 + a2 z^-2
//        //
//        // implemented in transposed DF-II.
//
//        float b0 = 0.0f;
//        float a1 = 0.0f;
//        float a2 = 0.0f;
//
//        float s1 = 0.0f;
//        float s2 = 0.0f;
//
//        inline float process(float x) noexcept {
//            const float y =
//                b0 * x + s1;
//
//            s1 =
//                -a1 * y + s2;
//
//            s2 =
//                -a2 * y;
//
//            return y;
//        }
//
//        inline void reset() noexcept {
//            s1 = 0.0f;
//            s2 = 0.0f;
//        }
//    };
//
//    struct RadiationRegion {
//        std::array<RadiationMode, kModeCount> modes{};
//
//        inline float process(float force) noexcept {
//            float pressure = 0.0f;
//
//            for (auto& mode : modes) {
//                pressure += mode.process(force);
//            }
//
//            return pressure;
//        }
//
//        inline void reset() noexcept {
//            for (auto& mode : modes) {
//                mode.reset();
//            }
//        }
//    };
//
//    RadiationRegion bass_;
//    RadiationRegion middle_;
//    RadiationRegion treble_;
//
//    double sample_rate_ = 44100.0;
//
//public:
//
//    mutable std::array<float, 88> bridge_force{};
//
//    explicit SoundboardModel(double sample_rate = 44100.0)
//        : sample_rate_(sample_rate)
//    {
//        // ------------------------------------------------------------
//        // Bass bridge region
//        //
//        // Lower-frequency modal emphasis.
//        // ------------------------------------------------------------
//
//        bass_.modes[0] =
//            make_mode(95.0,   0.10, 0.32);
//
//        bass_.modes[1] =
//            make_mode(210.0,  0.08, 0.25);
//
//        bass_.modes[2] =
//            make_mode(430.0,  0.07, 0.16);
//
//        bass_.modes[3] =
//            make_mode(850.0,  0.09, 0.08);
//
//
//        // ------------------------------------------------------------
//        // Middle bridge region
//        // ------------------------------------------------------------
//
//        middle_.modes[0] =
//            make_mode(140.0,  0.09, 0.20);
//
//        middle_.modes[1] =
//            make_mode(370.0,  0.07, 0.24);
//
//        middle_.modes[2] =
//            make_mode(820.0,  0.06, 0.18);
//
//        middle_.modes[3] =
//            make_mode(1650.0, 0.08, 0.10);
//
//
//        // ------------------------------------------------------------
//        // Treble bridge region
//        //
//        // More energy is retained in the upper modal range.
//        // ------------------------------------------------------------
//
//        treble_.modes[0] =
//            make_mode(230.0,  0.10, 0.10);
//
//        treble_.modes[1] =
//            make_mode(720.0,  0.08, 0.17);
//
//        treble_.modes[2] =
//            make_mode(1700.0, 0.07, 0.20);
//
//        treble_.modes[3] =
//            make_mode(3400.0, 0.10, 0.12);
//    }
//
//
//    inline float get_sample() noexcept {
//
//        // ============================================================
//        // Spatial reduction of the bridge-force field
//        //
//        // MIDI:
//        //
//        // 21 ... 40   bass
//        // 41 ... 75   middle
//        // 76 ... 108  treble
//        //
//        // bridge_force[0] corresponds to MIDI 21.
//        // ============================================================
//
//        float bass_force = 0.0f;
//        float middle_force = 0.0f;
//        float treble_force = 0.0f;
//
//        for (int i = 0; i < 20; ++i) {
//            bass_force += bridge_force[i];
//        }
//
//        for (int i = 20; i < 55; ++i) {
//            middle_force += bridge_force[i];
//        }
//
//        for (int i = 55; i < 88; ++i) {
//            treble_force += bridge_force[i];
//        }
//
//
//        // ============================================================
//        // Bridge force -> radiated pressure proxy
//        // ============================================================
//
//        const float bass_pressure =
//            bass_.process(bass_force);
//
//        const float middle_pressure =
//            middle_.process(middle_force);
//
//        const float treble_pressure =
//            treble_.process(treble_force);
//
//
//        return
//            bass_pressure +
//            middle_pressure +
//            treble_pressure;
//    }
//
//
//    inline void system_reset() noexcept {
//
//        bridge_force.fill(0.0f);
//
//        bass_.reset();
//        middle_.reset();
//        treble_.reset();
//    }
//
//
//private:
//
//    RadiationMode make_mode(
//        double frequency_hz,
//        double damping_ratio,
//        double gain)
//    {
//        // ============================================================
//        // Continuous damped mode:
//        //
//        // poles:
//        //
//        // s = -ζω₀ ± j ω₀ sqrt(1-ζ²)
//        //
//        // Mapping directly into discrete-time poles:
//        //
//        // r     = exp(-ζω₀Ts)
//        //
//        // theta = ω₀ sqrt(1-ζ²) Ts
//        //
//        // denominator:
//        //
//        // 1 - 2r cos(theta) z^-1 + r² z^-2
//        //
//        // Therefore:
//        //
//        // a1 = -2r cos(theta)
//        // a2 = r²
//        // ============================================================
//
//        const double omega =
//            2.0 *
//            std::numbers::pi *
//            frequency_hz;
//
//        const double ts =
//            1.0 / sample_rate_;
//
//        const double damping =
//            std::max(
//                0.0,
//                std::min(damping_ratio, 0.999999)
//            );
//
//        const double radius =
//            std::exp(
//                -damping *
//                omega *
//                ts
//            );
//
//        const double damped_omega =
//            omega *
//            std::sqrt(
//                1.0 -
//                damping * damping
//            );
//
//        const double theta =
//            damped_omega * ts;
//
//        RadiationMode mode;
//
//        mode.a1 =
//            static_cast<float>(
//                -2.0 *
//                radius *
//                std::cos(theta)
//            );
//
//        mode.a2 =
//            static_cast<float>(
//                radius * radius
//            );
//
//        // Modal excitation coefficient.
//        //
//        // This normalization keeps the gain parameter usable as a
//        // relative modal-strength parameter rather than allowing the
//        // pole radius alone to dominate the amplitude.
//        mode.b0 =
//            static_cast<float>(
//                gain *
//                (1.0 - radius)
//            );
//
//        return mode;
//    }
//};
//
//#endif /* soundboard_model_hpp */
//





//#ifndef soundboard_model_hpp
//#define soundboard_model_hpp
//
//#include <array>
//class SoundboardModel {
//    SoundboardModel(const SoundboardModel&) = delete;
//    SoundboardModel& operator=(const SoundboardModel&) = delete;
//
//    // Temporary bridge-force -> acoustic-radiation proxy.
//    //
//    // This filter is intentionally placed outside the mechanical
//    // string/bridge feedback loop. It does not represent the soundboard
//    // mobility itself; it only approximates the spectral coloration
//    // between total bridge excitation and radiated acoustic output.
//    //
//    // Fs = 44100 Hz
//    float radiation_s1_ = 0.0f;
//    float radiation_s2_ = 0.0f;
//
//public:
//    mutable std::array<float, 88> bridge_force{};
//
//    explicit SoundboardModel() = default;
//
//    inline float get_sample() {
//        float bridge_excitation = 0.0f;
//
//        for (std::size_t i = 0; i < bridge_force.size(); ++i) {
//            bridge_excitation += bridge_force[i];
//        }
//
//        /*
//            Temporary radiation transfer function:
//
//                         b0 + b1 z^-1 + b2 z^-2
//                H(z) = -------------------------
//                          1 + a1 z^-1 + a2 z^-2
//
//            This is NOT a physical soundboard model.
//
//            It is a low-order approximation of the transfer from
//            aggregated bridge excitation to radiated acoustic output,
//            introduced only to avoid treating raw bridge force as PCM.
//        */
//
//        constexpr float b0 = 0.08315987f;
//        constexpr float b1 = 0.16631974f;
//        constexpr float b2 = 0.08315987f;
//
//        constexpr float a1 = -1.03517121f;
//        constexpr float a2 = 0.36781069f;
//
//        const float radiation =
//            b0 * bridge_excitation + radiation_s1_;
//
//        radiation_s1_ =
//            b1 * bridge_excitation
//            - a1 * radiation
//            + radiation_s2_;
//
//        radiation_s2_ =
//            b2 * bridge_excitation
//            - a2 * radiation;
//
//        return radiation;
//    }
//
//    inline void system_reset() {
//        bridge_force.fill(0.0f);
//
//        radiation_s1_ = 0.0f;
//        radiation_s2_ = 0.0f;
//    }
//};
//
////class SoundboardModel {
////    SoundboardModel(const SoundboardModel&) = delete;
////    SoundboardModel& operator=(const SoundboardModel&) = delete;
////    
////    float radiation_z1_ = 0.0f;
////    float radiation_z2_ = 0.0f;
////    
////public:
////    mutable std::array<float, 88> bridge_force{};
////    
////    explicit SoundboardModel() {
////        
////    }
//////    inline float get_sample() {
//////        float bridge_sum = 0.0f;
//////
//////        for (int i = 0; i < bridge_force.size(); ++i) {
//////            bridge_sum += bridge_force[i];
//////        }
//////
//////        // ============================================================
//////        // Temporary soundboard radiation filter
//////        //
//////        // 这里只处理最终辐射输出，不参与 string / bridge feedback。
//////        // 目的：暂时抑制直接 bridge-force 输出中过强的高频与尖锐感。
//////        // ============================================================
//////
//////        // 二阶 Butterworth 风格低通。
//////        // 当前系数按 44.1 kHz 下约 5 kHz 截止频率使用。
//////        constexpr float b0 = 0.08315987f;
//////        constexpr float b1 = 0.16631974f;
//////        constexpr float b2 = 0.08315987f;
//////
//////        constexpr float a1 = -1.03517121f;
//////        constexpr float a2 = 0.36781069f;
//////
//////        const float filtered =
//////            b0 * bridge_sum + radiation_z1_;
//////
//////        radiation_z1_ =
//////            b1 * bridge_sum
//////            - a1 * filtered
//////            + radiation_z2_;
//////
//////        radiation_z2_ =
//////            b2 * bridge_sum
//////            - a2 * filtered;
//////
//////        // 保留少量未经滤波的 bridge signal，
//////        // 避免暂时的低通使 attack 完全失去存在感。
//////        constexpr float direct_mix = 0.15f;
//////        constexpr float filtered_mix = 0.85f;
//////
//////        return direct_mix * bridge_sum
//////             + filtered_mix * filtered;
//////    }
////    inline float get_sample() {
////        float sum = 0.0f;
////        for(int i = 0; i < bridge_force.size(); i++) {
////            sum += bridge_force[i];
////        }
////        return sum;
////    }
////    
////    inline void system_reset() {
////        bridge_force.fill(0.0f);
////
////        radiation_z1_ = 0.0f;
////        radiation_z2_ = 0.0f;
////    }
////};
//
//#endif /* soundboard_model_hpp */


class KeyModel {
    
    // 弦锤耦合与tunning
    
public:
    
    double samplerate_ = 0.0;
    
    int midi_n_;
    int string_count_ = 3;
    
    const BridgeModel *bridge_;
    const SoundboardModel *soundboard_;
    HammerModel hammer_;
    std::array<StringModel, 3> strings_;
    
    std::array<double, 3> string_vs_ = {0.0, 0.0, 0.0};
    
    bool key_down_ = false;
    bool key_active_ = false;
    
    bool sustainpedal_active_ = false;
    
    const Configuration *configuration_ = nullptr;
    
    // 构造后琴弦阻抗不再变化，因此提前计算
    double total_impedance_ = 0.0;
    double inverse_total_impedance_ = 0.0;
    double inverse_double_total_impedance_ = 0.0;
    std::array<double, 3> force_ratios_ = {0.0, 0.0, 0.0};
    
    // ======================== ========================
    // Compute
    // midi_n_ > 96
    // ======================== ========================
    std::array<float, 3> last_bridge_force_96{};
    
    KeyModel(int midi_n,
             double sample_rate,
             int string_count,
             TunningPresets::Temperament temperament,
             const SoundboardModel *soundboard,
             const BridgeModel *bridge,
             const Configuration *configuration) :
        midi_n_(midi_n),
        string_count_(string_count),
        samplerate_(sample_rate),
        hammer_(sample_rate, midi_n, configuration),
        strings_{
            StringModel(sample_rate,
                        midi_n,
                        temperament,
                        TunningPresets::StringIndex::left,
                        configuration),
            StringModel(sample_rate,
                        midi_n,
                        temperament,
                        TunningPresets::StringIndex::center,
                        configuration),
            StringModel(sample_rate,
                        midi_n,
                        temperament,
                        TunningPresets::StringIndex::right,
                        configuration),
        },
        soundboard_(soundboard),
        bridge_(bridge),
        configuration_(configuration)
    {
        for (int i = 0; i < string_count_; ++i) {
            total_impedance_ += strings_[i].z_;
        }
        
        if (total_impedance_ > 0.0) {
            inverse_total_impedance_ = 1.0 / total_impedance_;
            inverse_double_total_impedance_ =
                1.0 / (total_impedance_ * 2.0);
            
            for (int i = 0; i < string_count_; ++i) {
                force_ratios_[i] =
                    strings_[i].z_ * inverse_total_impedance_;
            }
        }
    }
    
    inline void key_movement() {
        
        sustainpedal_controller();
        
        double contact_string_velocity = 0.0;
        
        for (int i = 0; i < string_count_; ++i) {
            string_vs_[i] = strings_[i].get_string_vs();
            
            contact_string_velocity += force_ratios_[i] * string_vs_[i];
        }
        
        double hammer_force_1 =
            hammer_.hammer_movement(contact_string_velocity);
        
        double hammer_force_2 =
            hammer_.hammer_movement(
                contact_string_velocity +
                hammer_force_1 * inverse_double_total_impedance_
            );
        
        double hammer_force =
            (hammer_force_1 + hammer_force_2) / 2.0;
        
        for (int i = 0; i < string_count_; i++) {
            if(strings_[i].midi_n_ < 96) {
                strings_[i].string_movement(hammer_force * force_ratios_[i]);
            } else {
                strings_[i].string_movement(hammer_force_1 * force_ratios_[i]);
                last_bridge_force_96[i] = strings_[i].get_bridge_force();
                strings_[i].string_movement(hammer_force_2 * force_ratios_[i]);
            }
            
            // bridge_->process(strings_[i].right_boundary_point);
        }
        
        update_bridge_force();
        
        check_active();
        
        if (!key_active_) {
            soundboard_->bridge_force[midi_n_ - 21] = 0.0f;
            last_bridge_force_96.fill(0.0f);
        }
        
    }
    
    inline void trigger(double velocity_mps) {
        for (int i = 0; i < string_count_; i++) {
            strings_[i].is_active = true;
            string_vs_[i] = 0.0;
        }
        hammer_.trigger(velocity_mps);
    }
    
    inline void update_bridge_force() {
        float result = 0.0;
        for (int i = 0; i < string_count_; i++) {
            if(strings_[i].midi_n_ < 96) {
                result += strings_[i].get_bridge_force();
            } else {
                result += (strings_[i].get_bridge_force() + last_bridge_force_96[i]) / 2.0;
            }
            
        }
        soundboard_->bridge_force[midi_n_ - 21] = result;
    }
    
    inline void system_reset() {
        for (int i = 0; i < string_count_; i++) {
            strings_[i].system_reset();
        }
        hammer_.system_reset();
        key_down_ = false;
        key_active_ = false;
        string_vs_.fill(0.0);
        sustainpedal_active_ = false;
    }
    
private:
    
    inline void check_active() {
        key_active_ = hammer_.is_contacting_;
        
        for (int i = 0; i < string_count_; ++i) {
            key_active_ = key_active_ || strings_[i].is_active;
        }
    }
    
    inline void sustainpedal_controller() {
        // key 抬起且延音踏板未踩下则启动制音器
        const bool damper_should_touch =
            !key_down_ && !sustainpedal_active_;
        
        for (int index = 0; index < string_count_; ++index) {
            strings_[index].damper_active =
                damper_should_touch;
        }
    }
};

#endif /* key_model_hpp */



class PianoModel {
    PianoModel(const PianoModel&) = delete;
    PianoModel& operator=(const PianoModel&) = delete;
public:
    
    static constexpr Configuration configuration{};
    BridgeModel bridge_;
    SoundboardModel soundboard_;
    std::array<KeyModel*, 88> piano_keys;
    
    PianoModel(double sample_rate) {
        for (std::size_t index = 0; index < piano_keys.size(); ++index) {
            const int midi_n = static_cast<int>(index) + 21;

            int string_count = 1;
            if (midi_n >= 34) {
                string_count = 3;
            } else if (midi_n >= 29) {
                string_count = 2;
            }
            
            piano_keys[index] = new KeyModel(midi_n,
                                             sample_rate,
                                             string_count,
                                             TunningPresets::Temperament::equal,
                                             &soundboard_,
                                             &bridge_,
                                             &configuration);
        }
    }
    
    inline void piano_movement() {
        for(int i = 0; i < piano_keys.size(); i++) {
            if(piano_keys[i]->key_active_)
                piano_keys[i]->key_movement();
        }
        
//        piano_keys[69-21]->key_movement();
    }
    
    inline float get_sample() {
        return 0.01 * soundboard_.get_sample();
    }
    
    inline void sustainpedal_control(bool is_active) {
        if(is_active) {
            for(int i = 0; i < piano_keys.size(); i++) {
                piano_keys[i]->sustainpedal_active_ = true;
            }
        } else {
            for(int i = 0; i < piano_keys.size(); i++) {
                piano_keys[i]->sustainpedal_active_ = false;
            }
        }
    }
    
    inline void system_reset() {
        for(int i = 0; i < piano_keys.size(); i++) {
            piano_keys[i]->system_reset();
        }
        soundboard_.system_reset();
    }
    
    ~PianoModel() noexcept {
        for(int i = 0; i < piano_keys.size(); i++) {
            delete piano_keys[i];
        }
    }
};

#endif /* piano_model_hpp */

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
// 2026-09-10
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

#ifndef soundcard_helper_hpp
#define soundcard_helper_hpp

#include <AudioToolbox/AudioToolbox.h>
#include <stdexcept>


#ifndef BBPL_AUDIO_CALLBACK_GATE_HPP
#define BBPL_AUDIO_CALLBACK_GATE_HPP

#include <atomic>
#include <cstdint>

// Allocation-free admission barrier for a real-time callback. open() is called
// immediately before AudioOutputUnitStart. close_and_wait() is called before
// callback dependencies are released.
class AudioCallbackGate {
 public:
  class Guard {
   public:
    explicit Guard(AudioCallbackGate& gate) noexcept
        : gate_(&gate), entered_(gate.try_enter()) {}

    Guard(const Guard&) = delete;
    Guard& operator=(const Guard&) = delete;

    ~Guard() {
      if (entered_) {
        gate_->leave();
      }
    }

    explicit operator bool() const noexcept { return entered_; }

   private:
    AudioCallbackGate* gate_;
    bool entered_;
  };

  AudioCallbackGate() noexcept = default;
  AudioCallbackGate(const AudioCallbackGate&) = delete;
  AudioCallbackGate& operator=(const AudioCallbackGate&) = delete;

  void open() noexcept { state_.store(0, std::memory_order_release); }

  void close_and_wait() noexcept {
    std::uint32_t state =
        state_.fetch_or(k_stopping_bit, std::memory_order_acq_rel) |
        k_stopping_bit;
    while ((state & k_count_mask) != 0) {
      state_.wait(state, std::memory_order_acquire);
      state = state_.load(std::memory_order_acquire);
    }
  }

  bool is_open() const noexcept {
    return (state_.load(std::memory_order_acquire) & k_stopping_bit) == 0;
  }

 private:
  static constexpr std::uint32_t k_stopping_bit = 0x8000'0000U;
  static constexpr std::uint32_t k_count_mask = ~k_stopping_bit;

  bool try_enter() noexcept {
    std::uint32_t state = state_.load(std::memory_order_acquire);
    while ((state & k_stopping_bit) == 0) {
      if ((state & k_count_mask) == k_count_mask) {
        return false;
      }
      if (state_.compare_exchange_weak(state, state + 1,
                                       std::memory_order_acq_rel,
                                       std::memory_order_acquire)) {
        return true;
      }
    }
    return false;
  }

  void leave() noexcept {
    state_.fetch_sub(1, std::memory_order_release);
    state_.notify_all();
  }

  std::atomic<std::uint32_t> state_{k_stopping_bit};
};

#endif

class Soundcard {
    
    Soundcard(Soundcard&&) = delete;
    Soundcard& operator=(Soundcard&&) = delete;
    
    AudioUnit audio_unit_ = nullptr;
    AudioCallbackGate callback_gate_;
    bool running_ = false;
    
public:
    // 禁止资源拥有类复制
    Soundcard(const Soundcard&) = delete;
    Soundcard& operator=(const Soundcard&) = delete;
    
    explicit Soundcard(double sample_rate) {
        try {
            // ======================== ======================== ========================
            // Initialize audio_unit_
            // 创建 audio_unit_
            // ======================== ======================== ========================
            // ======================== ========================
            // Audio Component Description
            // ”我要描述我想找什么样的音频组件？“
            /// (这里所写的就是搜索音频组建的过滤条件)
            // ======================== ========================
            AudioComponentDescription description{};
            // 我要找一个“输出类型”的 Audio Unit:
            /// (kAudioUnitType_ 还有很多其他功能的 audio unit, 下同)
            description.componentType = kAudioUnitType_Output;
            // 输出设备为 macOS 当前的默认输出设备
            description.componentSubType = kAudioUnitSubType_DefaultOutput;
            // 我要 Apple 提供的这个 Audio Unit
            /// (这是因为 macOS 的 Audio Unit 系统理论上也允许第三方厂商注册自己的 Audio Unit)
            description.componentManufacturer = kAudioUnitManufacturer_Apple;
            
            // 以上三句话连在一起就是：
            /// “我要找 Apple 制造的 Output 类型的 Default Output 的 Audio Unit”
            
            // 以下两句话的意思就是：
            /// “我没有额外的过滤条件了”
            description.componentFlags = 0;
            description.componentFlagsMask = 0;
            
            // ======================== ========================
            // Audio Component FindNext
            // ”根据 description，帮我在系统里找到符合条件的 Audio Component“
            // ======================== ========================
            // nullptr 表示：从搜索结果列表里的第一个开始找
            /// 这说明支持从指定的某一项开始往下找，这样也会更便捷
            AudioComponent component = AudioComponentFindNext(nullptr, &description);
            if (component == nullptr) {
                throw std::runtime_error("Failed to find audio component.");
            }
            
            // ======================== ========================
            // Audio Component InstanceNew
            // “根据刚刚找到的 AudioComponent，创建一个真正能使用的 AudioUnit 实例”
            // ======================== ========================
            /// 这里的末尾的 _ 代表是类成员变量，这样就能和入参区分出来
            OSStatus status_creatAudioUnit = AudioComponentInstanceNew(component, &audio_unit_);
            if (status_creatAudioUnit != noErr) {
                throw std::runtime_error("Failed to create audio unit.");
            }
            
            // ======================== ======================== ========================
            // The descriptions of PCM
            // 对 PCM 格式的描述
            // ======================== ======================== ========================
            // ======================== ========================
            // Audio Stream Basic Description
            // ”我要描述我想要什么样格式的 PCM？“
            // ======================== ========================
            AudioStreamBasicDescription format{};
            
            // 采样率
            format.mSampleRate = sample_rate;
            // 数据格式
            /// 未经 MP3、AAC 之类压缩的原始采样值
            format.mFormatID = kAudioFormatLinearPCM;
            // 进一步说明 PCM 的具体类型
            /// Float32 float -1.0 ~ +1.0
            format.mFormatFlags = kAudioFormatFlagsNativeFloatPacked;
            // 一个 sample 占：32 bit = 4 byte
            /// 32比特等于4字节，一个字节8比特
            format.mBitsPerChannel = 32;
            // 双声道 stereo
            // mono 单声道
            format.mChannelsPerFrame = 1;
            // 计算一frame分配多少bytes(字节)
            // sizeof(Float32) == 4; mChannelsPerFrame == 2;
            // 因此 mBytesPerFrame == 8;
            format.mBytesPerFrame = sizeof(Float32) * format.mChannelsPerFrame;
            // 对于压缩形式来说一般多frames会被压进一个packet里面
            // 但纯pcm作为非压缩格式一般一帧为一个packet
            format.mFramesPerPacket = 1;
            // 计算一个packet分配多少bytes(字节)
            format.mBytesPerPacket = format.mBytesPerFrame * format.mFramesPerPacket;
            // Apple 开发时所做的预留字段
            format.mReserved = 0;
            
            // ======================== ========================
            // Give the PCM format to audio_unit_
            // ”把 PCM format 交给 audio_unit_“
            // ======================== ========================
            // "给 audio_unit_ 的某个属性设置一个值"
            OSStatus status_StreamFormat = AudioUnitSetProperty(
                audio_unit_,
                kAudioUnitProperty_StreamFormat,// 我要修改它的“音频流格式”属性。
                kAudioUnitScope_Input,// 我将要送进这个 Output Unit 的 PCM 是什么格式？
                0,// HAL Output 我设置哪一个 bus 0 (主输出 bus)
                &format,
                sizeof(format));
            // 上文的意思是：
            /// “对 audio_unit_，把它第 0 个输入端口的 Stream Format 设置成 format 描述的格式。”
            if (status_StreamFormat != noErr) {
                throw std::runtime_error("Failed to set stream format.");
            }
            
            // ======================== ======================== ========================
            // Register for callback
            // 注册 Render Callback
            // ======================== ======================== ========================
            AURenderCallbackStruct callback{};
            // Core Audio是 C 风格 API, 成员函数不隐含"this"指针
            // 故使用不绑定具体对象的静态函数更接近 C 风格的普通函数
            callback.inputProc = &Soundcard::render_callback;
            // 但是使用静态函数指针回传给 C API 导致 C 无法访问 this
            // 所以下面这个变量就多传回一个 this
            callback.inputProcRefCon = this;
            OSStatus status_SetRenderCallback = AudioUnitSetProperty(
                audio_unit_,
                kAudioUnitProperty_SetRenderCallback,
                kAudioUnitScope_Input,
                0,
                &callback,
                sizeof(callback));
            if (status_SetRenderCallback != noErr) {
                throw std::runtime_error("Failed to set render callback.");
            }
        } catch (...) {
            AudioComponentInstanceDispose(audio_unit_);
            audio_unit_ = nullptr;
            throw;
        }
    }
    
    // ======================== ======================== ========================
    // Core Audio Callback Bridge
    // Render Callback Bridge
    // Core Audio C API 与 C++ Soundcard 实例之间的回调桥接
    // 将 C 风格回调传入的上下文指针恢复为 Soundcard 实例，
    // 再把实际的音频渲染工作转发给成员函数 render()
    // 将 Core Audio 的 C 风格回调桥接到 Soundcard C++ 实例
    // ======================== ======================== ========================
    static OSStatus render_callback(
        // 给 C 风格 API 多传入的成员函数的this指针
        // 在前面的设置中，这里就是 Soundcard*
        void* input_proc_ref_con,
        // 这一次 render 的状态/行为标志
        /// 相当于 core audio 回传了更精细的信息让以后的dsp能够有更多操作空间(下同)
        AudioUnitRenderActionFlags* io_action_flags,
        // 这一块音频在 Core Audio 时间轴上的时间信息
        /// 以后能利用这个做精确同步等操作
        const AudioTimeStamp* in_time_stamp,
        // in_bus_number 之前已经设置过 bus 0 (主输出)
        UInt32 in_bus_number,
        // frames_count
        UInt32 in_number_frames,
        // Core Audio 给的 PCM 内存
        /// Core Audio 给的 音频缓冲区列表及其描述信息
        AudioBufferList* io_data) noexcept
    {
        // 恢复指针类型，从万用指针 void* 变成 Soundcard*
        auto* soundcard = static_cast<Soundcard*>(input_proc_ref_con);
        
        if(soundcard == nullptr || io_data == nullptr) {
            return kAudio_ParamError;
        }
        
        AudioCallbackGate::Guard callback_guard(soundcard->callback_gate_);
        
        if(!callback_guard) {
            clear_output(io_action_flags, io_data);
            return noErr;
        }
        
        // 恢复之后就能直接调用 Soundcard 类下面的 render 函数
        return soundcard->render(
            io_action_flags,
            in_number_frames,
            io_data);
    }
    
    // ======================== ======================== ========================
    // Fill the buffer
    // 用 PCM 填充 Core Audio 给的音频缓冲区
    // ======================== ======================== ========================
    OSStatus render(AudioUnitRenderActionFlags* io_action_flags,
                    UInt32 frames_count,
                    AudioBufferList* data) noexcept ;
    
    void start() {
        // 函数幂等化：
        /// 设计初始化的函数不会重复启动
        if(running_)
            return ;
        
        OSStatus status = AudioUnitInitialize(audio_unit_);
        if (status != noErr) {
            throw std::runtime_error("Failed to initialize audio unit.");
        }
        
        // 从这里开始允许 callback 访问 PianoModel。
        callback_gate_.open();
        
        status = AudioOutputUnitStart(audio_unit_);
        
        if (status != noErr) {
            callback_gate_.close_and_wait();
            AudioUnitUninitialize(audio_unit_);
            throw std::runtime_error("Failed to start audio unit.");
        }
        
        running_ = true;
    }
    
    // 析构函数不能随意 throw
    // 因为析构函数里面调用了 stop
    // 所以这里用 noexcept
    void stop() noexcept {
        if(!running_)
            return ;
        
        // 阻止新 callback 进入，并等待已经进入的 callback 退出。
        callback_gate_.close_and_wait();
        
        AudioOutputUnitStop(audio_unit_);
        AudioUnitUninitialize(audio_unit_);
        
        running_ = false;
    }
    
    static void clear_output(AudioUnitRenderActionFlags* io_action_flags,
                             AudioBufferList* data) noexcept;
    
    ~Soundcard() noexcept {
        stop();
        if (audio_unit_ != nullptr) {
            // 来自前文 AudioComponentInstanceNew 所对应的析构
            AudioComponentInstanceDispose(audio_unit_);
            audio_unit_ = nullptr;
        }
    }
    
};

#endif /* soundcard_helper_hpp */













#ifndef engine_rate_eval_hpp
#define engine_rate_eval_hpp

#include <cstdint>
#include <iomanip>
#include <iostream>

#include <mach/mach_time.h>

class EngineEval {
private:
    uint64_t start_ = 0;

    double sample_rate_ = 44100.0;
    double instant_rate_ = 0.0;

public:
    explicit EngineEval(double sample_rate)
        : sample_rate_(sample_rate) {}

    inline void start_timing() noexcept {
        start_ =
            clock_gettime_nsec_np(CLOCK_UPTIME_RAW);
    }

    inline void end_timing(int frame_count) noexcept {
        const uint64_t end =
            clock_gettime_nsec_np(CLOCK_UPTIME_RAW);

        const uint64_t actual_ns =
            end - start_;

        const double buffer_ns =
            1'000'000'000.0 *
            static_cast<double>(frame_count) /
            sample_rate_;

        const double current_rate =
            static_cast<double>(actual_ns) /
            buffer_ns;

        instant_rate_ =
            0.9 * instant_rate_ +
            0.1 * current_rate;
    }

    [[nodiscard]]
    inline double engine_rate() const noexcept {
        return instant_rate_;
    }

    [[nodiscard]]
    inline double engine_rate_percent() const noexcept {
        return instant_rate_ * 100.0;
    }

    [[nodiscard]]
    inline double realtime_multiple() const noexcept {
        if (instant_rate_ <= 0.0) {
            return 0.0;
        }

        return 1.0 / instant_rate_;
    }

    [[nodiscard]]
    inline bool realtime_capable() const noexcept {
        return instant_rate_ <= 1.0;
    }

    [[nodiscard]]
    double benchmark(
        std::unique_ptr<PianoModel>& piano,
        int warmup_frames = 512,
        int measure_frames = 8192,
        int repetitions = 5
    ) const {

        constexpr int kMaxRepetitions = 31;

        repetitions =
            std::clamp(repetitions, 1, kMaxRepetitions);

        std::array<double, kMaxRepetitions> results{};

        volatile float sample_sink = 0.0f;

        for (int repetition = 0;
             repetition < repetitions;
             ++repetition) {

            for (int frame = 0;
                 frame < warmup_frames;
                 ++frame) {

                piano->piano_movement();

                sample_sink =
                    sample_sink +
                    piano->get_sample();
            }

            const uint64_t start =
                clock_gettime_nsec_np(
                    CLOCK_UPTIME_RAW
                );

            for (int frame = 0;
                 frame < measure_frames;
                 ++frame) {

                piano->piano_movement();

                sample_sink =
                    sample_sink +
                    piano->get_sample();
            }

            const uint64_t end =
                clock_gettime_nsec_np(
                    CLOCK_UPTIME_RAW
                );

            const double actual_ns =
                static_cast<double>(end - start);

            const double rendered_ns =
                1'000'000'000.0 *
                static_cast<double>(measure_frames) /
                sample_rate_;

            results[repetition] =
                actual_ns / rendered_ns;
        }

        std::sort(
            results.begin(),
            results.begin() + repetitions
        );

        return results[
            static_cast<std::size_t>(
                repetitions / 2
            )
        ];
    }
    
    inline void reset() noexcept {
        instant_rate_ = 0.0;
    }
    void print_benchmark(std::unique_ptr<PianoModel>& piano, std::string range) const {
        const double rate =
            benchmark(piano);

        const double frame_budget_us =
            1'000'000.0 / sample_rate_;

        const double frame_cost_us =
            frame_budget_us * rate;

        std::cout
            << std::fixed
            << std::setprecision(2)

//            << "bbpl L1 DSP benchmark\n"
//            << "Sample rate: "
//            << sample_rate_
//            << " Hz\n"

            << "    Frame: "
            << frame_cost_us
            << " us / "
            << frame_budget_us
            << " us\n"

            << "    Occupancy: "
            << rate * 100.0
            << "%\n"

            << "    Speed: "
            << (1.0 / rate)
            << "x realtime\n"

            <<  "    " + range + " realtime: "
            << (rate <= 1.0 ? "PASS" : "FAIL")
            << "\n\n";
    }
    
};

#endif

#ifndef BBPL_PIANO_COMMAND_QUEUE_HPP
#define BBPL_PIANO_COMMAND_QUEUE_HPP

#include <array>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <utility>

enum class PianoCommandType : std::uint8_t {
  note_on,
  note_off,
  note_aftertouch,
  soft_pedal,
  harmonic_pedal,
  sostenuto_pedal,
  sustain_pedal,
  all_silence,
};

struct PianoCommand {
  PianoCommandType type = PianoCommandType::all_silence;
  int note = 0;
  double value = 0.0;
};

// Bounded allocation-free multi-producer/single-consumer queue. Producers are
// CoreMIDI, CLI, and service threads; the Core Audio render thread is the only
// consumer. Capacity must be a power of two.
template <std::size_t Capacity>
class PianoCommandQueue {
  static_assert(Capacity >= 2 && (Capacity & (Capacity - 1)) == 0);
  static_assert(std::is_trivially_copyable_v<PianoCommand>);

 public:
  PianoCommandQueue() noexcept {
    for (std::size_t index = 0; index < Capacity; ++index) {
      slots_[index].sequence.store(index, std::memory_order_relaxed);
    }
  }

  PianoCommandQueue(const PianoCommandQueue&) = delete;
  PianoCommandQueue& operator=(const PianoCommandQueue&) = delete;

  bool try_push(PianoCommand command) noexcept {
    std::size_t position = enqueue_position_.load(std::memory_order_relaxed);
    for (;;) {
      Slot& slot = slots_[position & (Capacity - 1)];
      const std::size_t sequence =
          slot.sequence.load(std::memory_order_acquire);
      const auto difference = static_cast<std::intptr_t>(sequence) -
                              static_cast<std::intptr_t>(position);
      if (difference == 0) {
        if (enqueue_position_.compare_exchange_weak(
                position, position + 1, std::memory_order_relaxed)) {
          slot.command = command;
          slot.sequence.store(position + 1, std::memory_order_release);
          return true;
        }
      } else if (difference < 0) {
        return false;
      } else {
        position = enqueue_position_.load(std::memory_order_relaxed);
      }
    }
  }

  bool try_pop(PianoCommand& command) noexcept {
    Slot& slot = slots_[dequeue_position_ & (Capacity - 1)];
    const std::size_t sequence = slot.sequence.load(std::memory_order_acquire);
    const auto difference = static_cast<std::intptr_t>(sequence) -
                            static_cast<std::intptr_t>(dequeue_position_ + 1);
    if (difference != 0) {
      return false;
    }

    command = slot.command;
    slot.sequence.store(dequeue_position_ + Capacity,
                        std::memory_order_release);
    ++dequeue_position_;
    return true;
  }

  void clear() noexcept {
    PianoCommand command;
    while (try_pop(command)) {
    }
  }

 private:
  struct Slot {
    std::atomic<std::size_t> sequence{0};
    PianoCommand command{};
  };

  alignas(64) std::array<Slot, Capacity> slots_{};
  alignas(64) std::atomic<std::size_t> enqueue_position_{0};
  alignas(64) std::size_t dequeue_position_ = 0;
};

class PianoCommandBus {
 public:
  bool post(PianoCommand command) noexcept {
    if (queue_.try_push(command)) {
      return true;
    }
    // Dropping note-off or pedal-up can leave a stuck state. Overflow therefore
    // requests a deterministic reset on the audio thread instead.
    reset_requested_.store(true, std::memory_order_release);
    return false;
  }

  template <class Consumer>
  void drain(Consumer&& consume) {
    PianoCommand command;
    if (reset_requested_.exchange(false, std::memory_order_acq_rel)) {
      while (queue_.try_pop(command)) {
      }
      consume(PianoCommand{PianoCommandType::all_silence, 0, 0.0});
    }
    while (queue_.try_pop(command)) {
      consume(command);
    }
  }

  // Call only from the single consumer after the AudioUnit callback gate has
  // closed. Producers must already belong to stopped service objects.
  void clear() noexcept {
    queue_.clear();
    reset_requested_.store(false, std::memory_order_release);
  }

 private:
  PianoCommandQueue<4096> queue_;
  std::atomic<bool> reset_requested_{false};
};

#endif

std::unique_ptr<PianoModel> bBpiano;
std::unique_ptr<Soundcard> soundcard;
std::unique_ptr<EngineEval> engine_eval;

// ======================== ======================== ========================
// Commands
// 基础命令流模块
// ======================== ======================== ========================
namespace {

constexpr int k_lowest_midi_note = 21;
constexpr int k_highest_midi_note = 108;

PianoCommandBus piano_commands;

std::atomic<bool> audio_render_failure{false};
std::atomic<std::uint32_t> engine_event_generation{0};

bool is_playable_note(int midi_note) noexcept {
    return midi_note >= k_lowest_midi_note &&
    midi_note <= k_highest_midi_note;
}

void notify_engine_event() noexcept {
    engine_event_generation.fetch_add(1, std::memory_order_release);
    engine_event_generation.notify_all();
}

void post_command(PianoCommandType type, int midi_note, double value) noexcept {
    static_cast<void>(piano_commands.post({type, midi_note, value}));
}

void apply_piano_command(const PianoCommand& command) {
    if (!bBpiano) {
        throw std::logic_error("Piano engine is not initialized.");
    }

    switch (command.type) {
        case PianoCommandType::note_on: {
            if (!is_playable_note(command.note))
                break;
            KeyModel* key = bBpiano->piano_keys[command.note - k_lowest_midi_note];
            key->key_down_ = true;
            for (int i = 0; i < key->string_count_; ++i) {
                key->strings_[i].damper_active = false;
            }
            key->key_active_ = true;
            // Yamaha Disklavier 使用的 mapping
            const double hammer_v0 = std::pow(2, (command.value - 52.0) / 25.0);
            key->trigger(hammer_v0);
            break;
        }

        case PianoCommandType::note_off: {
            if (is_playable_note(command.note)) {
                bBpiano->piano_keys[command.note - k_lowest_midi_note]->key_down_ = false;
            }
            KeyModel* key = bBpiano->piano_keys[command.note - k_lowest_midi_note];
            key->key_down_ = false;
            if (!key->sustainpedal_active_) {
                for (int i = 0; i < key->string_count_; ++i) {
                    key->strings_[i].damper_active = true;
                }
            }
            key->key_active_ = true;
            break;
        }

        case PianoCommandType::sustain_pedal:
            bBpiano->sustainpedal_control(command.value > 0.01);
            
            break;

        case PianoCommandType::all_silence:
            bBpiano->system_reset();
            break;

        case PianoCommandType::note_aftertouch:
            
        case PianoCommandType::soft_pedal:
        case PianoCommandType::harmonic_pedal:
        case PianoCommandType::sostenuto_pedal:
            // 当前 PianoModel 尚未实现这些动作。
            break;
    }
}

}  // namespace


// ======================== ======================== ========================
// Initialize
// 初始化
// ======================== ======================== ========================
void bbpiano_init(double sample_rate) {
    bBpiano = std::make_unique<PianoModel>(sample_rate);
}
void bbpiano_shutdown() noexcept {
    bBpiano.reset();
}
void eval_init(double sample_rate) {
    engine_eval = std::make_unique<EngineEval>(sample_rate);
}
void eval_shutdown() noexcept {
    engine_eval.reset();
}
void soundcard_init(double sample_rate) {
    soundcard = std::make_unique<Soundcard>(sample_rate);
    soundcard->start();
}
void soundcard_shutdown() noexcept {
    soundcard.reset();
}

// ======================== ======================== ========================
// Hardware callback
// 硬件回调
// ======================== ======================== ========================
void get_next_buffer(float* out, int frameCount, double amplitudeLimiter) {
    
    if (out == nullptr || frameCount < 0) {
        throw std::invalid_argument("Invalid audio output buffer.");
    }

    if (!bBpiano || !engine_eval) {
        throw std::logic_error("Piano engine is not initialized.");
    }
    
    // 每个音频 buffer 只检查一次队列，不在每个 sample 上做原子操作。
    piano_commands.drain([](const PianoCommand& command) {
        apply_piano_command(command);
    });

    engine_eval->start_timing();
    
    for (int i = 0; i < frameCount; ++i) {
        bBpiano->piano_movement();
      
        float x = bBpiano->get_sample();
        
        if (!std::isfinite(x)) {
            std::cerr
                << "OUTPUT ABNORMAL\n"
                << "sample = " << x << '\n'
                << "frame = " << i << '\n';
            
            for(int i = 0; i < bBpiano->piano_keys.size(); i++) {
                if(bBpiano->piano_keys[i]->key_active_) {
                    std::cout << "hammer_.w_a_1_" <<
                    bBpiano->piano_keys[i]->hammer_.w_a_1_ << "\n";
                    std::cout << "hammer_.w_b_1_" <<
                    bBpiano->piano_keys[i]->hammer_.w_b_1_ << "\n";
                    std::cout << "hammer_.middle_v_" <<
                    bBpiano->piano_keys[i]->hammer_.middle_v_ << "\n";
                }
            }
            
            std::cout<<"\n";
                x = 0.0f;
            }
        
        out[i] = x * amplitudeLimiter;
    }
    
    engine_eval->end_timing(frameCount);
    
}


// ======================== ======================== ========================
// Piano gesture
// 钢琴手势
// ======================== ======================== ========================
void note_on(int midi_n, double velocity) noexcept {
    if(is_playable_note(midi_n)) {
        post_command(PianoCommandType::note_on, midi_n, velocity);
    }
}
void note_off(int midi_n, double velocity) noexcept {
    if(is_playable_note(midi_n)) {
        post_command(PianoCommandType::note_off, midi_n, velocity);
    }
}
void note_aftertouch(int midi_n, double pressure) noexcept {
    if(is_playable_note(midi_n)) {
        post_command(PianoCommandType::note_aftertouch, midi_n, pressure);
    }
}

// ======================== ======================== ========================
// Piano pedal
// 钢琴踏板
// ======================== ======================== ========================
void softpedal_control(double depth) noexcept {
    post_command(PianoCommandType::soft_pedal, 0, depth);
}
void harmonicpedal_control(double depth) noexcept {
    post_command(PianoCommandType::harmonic_pedal, 0, depth);
}
void sostenutopedal_control(double depth) noexcept {
    post_command(PianoCommandType::sostenuto_pedal, 0, depth);
}
void sustainpedal_control(double depth) noexcept {
    post_command(PianoCommandType::sustain_pedal, 0, depth);
}

void all_silence() noexcept {
    post_command(PianoCommandType::all_silence, 0, 0.0);
}




// ======================== ======================== ========================
// Deal with Error
// 报错与线程
// ======================== ======================== ========================

void clear_piano_commands() noexcept {
    piano_commands.clear();

    if (bBpiano) {
        bBpiano->system_reset();
    }
}

void report_audio_render_failure() noexcept {
    audio_render_failure.store(
        true, std::memory_order_release);

    notify_engine_event();
}

bool audio_render_failed() noexcept {
    return audio_render_failure.load(
        std::memory_order_acquire);
}

void clear_audio_render_failure() noexcept {
    audio_render_failure.store(
        false, std::memory_order_release);
}

bool wait_for_stop_or_audio_failure(
    std::stop_token stop_token) noexcept {

    std::stop_callback wake_on_stop(
        stop_token,
        [] {
            notify_engine_event();
        });

    while (!stop_token.stop_requested() &&
           !audio_render_failed()) {

        const std::uint32_t generation =
            engine_event_generation.load(
                std::memory_order_acquire);

        if (stop_token.stop_requested() ||
            audio_render_failed()) {
            break;
        }

        engine_event_generation.wait(
            generation,
            std::memory_order_acquire);
    }

    return audio_render_failed();
}


// ======================== ======================== ========================
// Test
// 测试
// ======================== ======================== ========================
void print_engine_rate() {
    if (!bBpiano || !engine_eval) {
        throw std::logic_error(
            "Piano engine is not initialized."
        );
    }
    
    // 保证测试从完全确定的状态开始。
    piano_commands.clear();
    bBpiano->system_reset();
    
    // 激活 High-range。
    for (int midi = 79;
         midi <= k_highest_midi_note;
         ++midi) {

        apply_piano_command({
            PianoCommandType::note_on,
            midi,
            110.0
        });
    }
    
    engine_eval->print_benchmark(bBpiano, "High-range");
    

    // 保证测试从完全确定的状态开始。
    piano_commands.clear();
    bBpiano->system_reset();

    // 激活 Mid-range。
    for (int midi = 48;
         midi <= 78;
         ++midi) {

        apply_piano_command({
            PianoCommandType::note_on,
            midi,
            110.0
        });
    }

    engine_eval->print_benchmark(bBpiano, "Mid-range");
    
    // 保证测试从完全确定的状态开始。
    piano_commands.clear();
    bBpiano->system_reset();

    // 激活 Bass-range。
    for (int midi = k_lowest_midi_note;
         midi <= 47;
         ++midi) {

        apply_piano_command({
            PianoCommandType::note_on,
            midi,
            110.0
        });
    }

    engine_eval->print_benchmark(bBpiano, "Bass-range");
    
    // 保证测试从完全确定的状态开始。
    piano_commands.clear();
    bBpiano->system_reset();

    // 激活全部 88 个键。
    for (int midi = k_lowest_midi_note;
         midi <= k_highest_midi_note;
         ++midi) {

        apply_piano_command({
            PianoCommandType::note_on,
            midi,
            110.0
        });
    }

    engine_eval->print_benchmark(bBpiano, "88-keys");
    

    bBpiano->system_reset();
}
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
// 2026-04-06
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
// [ZERO AI-GENERATED CODE]
// Every line in this file is written and understood by its author. Every result is
// reproducible, every assumption is open to inspection, and every implementation
// stands open to criticism and challenge.
// AI may be used for non-core, replaceable, engineering work;
// this file, however, contains core logic that the author considers
// necessary to understand firsthand, explain line by line, and take full responsibility for,
// and is therefore implemented entirely by hand.
// ---------------------------------------------------------------------------
// [本文件承诺不含任何 AI 生成代码]
// 每一行代码均由作者亲自编写，并确知其意义。一切结果可以复现，一切假设可经受检验，一切实现经得起批评与质疑。
// AI 可用于非核心、可替代的工程工作；
// 本文件承载作者认为必须亲自理解、能够逐行解释并为之负责的核心逻辑，因此刻意保持完全人工实现。
//
// Ziyang Tan
// 2026-09-04
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
// [ZERO AI-GENERATED CODE]
// Every line in this file is written and understood by its author. Every result is
// reproducible, every assumption is open to inspection, and every implementation
// stands open to criticism and challenge.
// AI may be used for non-core, replaceable, engineering work;
// this file, however, contains core logic that the author considers
// necessary to understand firsthand, explain line by line, and take full responsibility for,
// and is therefore implemented entirely by hand.
// ---------------------------------------------------------------------------
// [本文件承诺不含任何 AI 生成代码]
// 每一行代码均由作者亲自编写，并确知其意义。一切结果可以复现，一切假设可经受检验，一切实现经得起批评与质疑。
// AI 可用于非核心、可替代的工程工作；
// 本文件承载作者认为必须亲自理解、能够逐行解释并为之负责的核心逻辑，因此刻意保持完全人工实现。
//
// Ziyang Tan
// 2026-04-06
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
// [ZERO AI-GENERATED CODE]
// Every line in this file is written and understood by its author. Every result is
// reproducible, every assumption is open to inspection, and every implementation
// stands open to criticism and challenge.
// AI may be used for non-core, replaceable, engineering work;
// this file, however, contains core logic that the author considers
// necessary to understand firsthand, explain line by line, and take full responsibility for,
// and is therefore implemented entirely by hand.
// ---------------------------------------------------------------------------
// [本文件承诺不含任何 AI 生成代码]
// 每一行代码均由作者亲自编写，并确知其意义。一切结果可以复现，一切假设可经受检验，一切实现经得起批评与质疑。
// AI 可用于非核心、可替代的工程工作；
// 本文件承载作者认为必须亲自理解、能够逐行解释并为之负责的核心逻辑，因此刻意保持完全人工实现。
//
// Ziyang Tan
// 2026-09-15
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
// [ZERO AI-GENERATED CODE]
// Every line in this file is written and understood by its author. Every result is
// reproducible, every assumption is open to inspection, and every implementation
// stands open to criticism and challenge.
// AI may be used for non-core, replaceable, engineering work;
// this file, however, contains core logic that the author considers
// necessary to understand firsthand, explain line by line, and take full responsibility for,
// and is therefore implemented entirely by hand.
// ---------------------------------------------------------------------------
// [本文件承诺不含任何 AI 生成代码]
// 每一行代码均由作者亲自编写，并确知其意义。一切结果可以复现，一切假设可经受检验，一切实现经得起批评与质疑。
// AI 可用于非核心、可替代的工程工作；
// 本文件承载作者认为必须亲自理解、能够逐行解释并为之负责的核心逻辑，因此刻意保持完全人工实现。
//
// Ziyang Tan
// 2026-09-04
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>














// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
// [ZERO AI-GENERATED CODE]
// Every line in this file is written and understood by its author. Every result is
// reproducible, every assumption is open to inspection, and every implementation
// stands open to criticism and challenge.
// AI may be used for non-core, replaceable, engineering work;
// this file, however, contains core logic that the author considers
// necessary to understand firsthand, explain line by line, and take full responsibility for,
// and is therefore implemented entirely by hand.
// ---------------------------------------------------------------------------
// [本文件承诺不含任何 AI 生成代码]
// 每一行代码均由作者亲自编写，并确知其意义。一切结果可以复现，一切假设可经受检验，一切实现经得起批评与质疑。
// AI 可用于非核心、可替代的工程工作；
// 本文件承载作者认为必须亲自理解、能够逐行解释并为之负责的核心逻辑，因此刻意保持完全人工实现。
//
// Ziyang Tan
// 2026-09-04
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
// [ZERO AI-GENERATED CODE]
// Every line in this file is written and understood by its author. Every result is
// reproducible, every assumption is open to inspection, and every implementation
// stands open to criticism and challenge.
// AI may be used for non-core, replaceable, engineering work;
// this file, however, contains core logic that the author considers
// necessary to understand firsthand, explain line by line, and take full responsibility for,
// and is therefore implemented entirely by hand.
// ---------------------------------------------------------------------------
// [本文件承诺不含任何 AI 生成代码]
// 每一行代码均由作者亲自编写，并确知其意义。一切结果可以复现，一切假设可经受检验，一切实现经得起批评与质疑。
// AI 可用于非核心、可替代的工程工作；
// 本文件承载作者认为必须亲自理解、能够逐行解释并为之负责的核心逻辑，因此刻意保持完全人工实现。
//
// Ziyang Tan
// 2026-09-04
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
// [ZERO AI-GENERATED CODE]
// Every line in this file is written and understood by its author. Every result is
// reproducible, every assumption is open to inspection, and every implementation
// stands open to criticism and challenge.
// AI may be used for non-core, replaceable, engineering work;
// this file, however, contains core logic that the author considers
// necessary to understand firsthand, explain line by line, and take full responsibility for,
// and is therefore implemented entirely by hand.
// ---------------------------------------------------------------------------
// [本文件承诺不含任何 AI 生成代码]
// 每一行代码均由作者亲自编写，并确知其意义。一切结果可以复现，一切假设可经受检验，一切实现经得起批评与质疑。
// AI 可用于非核心、可替代的工程工作；
// 本文件承载作者认为必须亲自理解、能够逐行解释并为之负责的核心逻辑，因此刻意保持完全人工实现。
//
// Ziyang Tan
// 2026-04-06
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

//
//  soundboard_model.cpp
//  bbpl
//
//  Created by opus arc on 2026/9/13.
//

//
//  dispersion_filter.cpp
//  bbpl
//
//  Created by opus arc on 2026/9/16.
//

//
//  string_impedance_preset.cpp
//  bbpl
//
//  Created by opus arc on 2026/9/13.
//

//
//  tunning_preset.cpp
//  bbpl
//
//  Created by opus arc on 2026/9/12.
//

//
//  hammer_preset.cpp
//  bbpl
//
//  Created by opus arc on 2026/9/12.
//

// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
// [ZERO AI-GENERATED CODE]
// Every line in this file is written and understood by its author. Every result is
// reproducible, every assumption is open to inspection, and every implementation
// stands open to criticism and challenge.
// AI may be used for non-core, replaceable, engineering work;
// this file, however, contains core logic that the author considers
// necessary to understand firsthand, explain line by line, and take full responsibility for,
// and is therefore implemented entirely by hand.
// ---------------------------------------------------------------------------
// [本文件承诺不含任何 AI 生成代码]
// 每一行代码均由作者亲自编写，并确知其意义。一切结果可以复现，一切假设可经受检验，一切实现经得起批评与质疑。
// AI 可用于非核心、可替代的工程工作；
// 本文件承载作者认为必须亲自理解、能够逐行解释并为之负责的核心逻辑，因此刻意保持完全人工实现。
//
// Ziyang Tan
// 2026-09-04
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

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
// 2026-09-10
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

#include <cstring>
#include <limits>


OSStatus Soundcard::render(AudioUnitRenderActionFlags* io_action_flags,
                           UInt32 frames_count,
                           AudioBufferList* data) noexcept {
    if (data == nullptr ||
        data->mNumberBuffers != 1 ||
        data->mBuffers[0].mData == nullptr ||
        data->mBuffers[0].mNumberChannels != 1 ||
        data->mBuffers[0].mDataByteSize < frames_count * sizeof(Float32) ||
        frames_count > static_cast<UInt32>(std::numeric_limits<int>::max()))
    {

        report_audio_render_failure();
        clear_output(io_action_flags, data);
        return noErr;
    }
    
    // 当前使用 mono PCM，因此只有一个输出 buffer
    AudioBuffer& buffer = data->mBuffers[0];
    
    // mData 是 void*，
    // 但我们已经声明 PCM 格式为 Float32，
    // 因此恢复成 Float32*
    auto* pcm = static_cast<Float32*>(buffer.mData);

    // 让 AudioEngine 直接把下一段 PCM 写进
    // Core Audio 提供的内存
    try {
        get_next_buffer(
            pcm,
            static_cast<int>(frames_count),
            0.90);
        
        return noErr;
    } catch (...) {
        // 不允许异常越过 Core Audio 的 C callback。
        report_audio_render_failure();
        clear_output(io_action_flags, data);
        return noErr;
    }
}


void Soundcard::clear_output(
    AudioUnitRenderActionFlags* io_action_flags,
    AudioBufferList* data) noexcept {

    if(data != nullptr) {
        for (UInt32 i = 0; i < data->mNumberBuffers; ++i) {
            AudioBuffer& buffer = data->mBuffers[i];
            if (buffer.mData != nullptr) {
                std::memset(buffer.mData, 0, buffer.mDataByteSize);
            }
        }
    }

    if (io_action_flags != nullptr) {
        *io_action_flags |= kAudioUnitRenderAction_OutputIsSilence;
    }
}
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
// [ZERO AI-GENERATED CODE]
// Every line in this file is written and understood by its author. Every result
// is reproducible, every assumption is open to inspection, and every
// implementation stands open to criticism and challenge. AI may be used for
// non-core, replaceable, engineering work; this file, however, contains core
// logic that the author considers necessary to understand firsthand, explain
// line by line, and take full responsibility for, and is therefore implemented
// entirely by hand.
// ---------------------------------------------------------------------------
// [本文件承诺不含任何 AI 生成代码]
// 每一行代码均由作者亲自编写，并确知其意义。一切结果可以复现，一切假设可经受检验，一切实现经得起批评与质疑。
// AI 可用于非核心、可替代的工程工作；
// 本文件承载作者认为必须亲自理解、能够逐行解释并为之负责的核心逻辑，因此刻意保持完全人工实现。
//
// Ziyang Tan
// 2026-09-03
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

#include <csignal>
#include <cstdlib>
#include <exception>
#include <getopt.h>
#include <iostream>
#include <optional>
#include <pthread.h>
#include <signal.h>
#include <stop_token>
#include <string>
#include <thread>
#include <unistd.h>



// ======================== ======================== ========================
// Internal declarations
// 内部声明
// ======================== ======================== ========================
namespace {

// ------------------------ ------------------------
// CLI functions
// cli 函数
// ------------------------ ------------------------
void print_logo_and_version(const char *version, const char *logo);
void help();

// ------------------------ ------------------------
// Engine functions
// engine 函数
// ------------------------ ------------------------
void init_engine(bool start_soundcard);
void shutdown_engine();

// ------------------------ ------------------------
// Lifecycle Guards
// 生命周期守卫
// ------------------------ ------------------------
class EngineLifetime {
public:
  explicit EngineLifetime(bool start_soundcard) {
    init_engine(start_soundcard);
  }

  EngineLifetime(const EngineLifetime &) = delete;
  EngineLifetime &operator=(const EngineLifetime &) = delete;

  ~EngineLifetime() { shutdown_engine(); }
};

} // namespace

// ======================== ======================== ========================
// Public entry
// 公开入口
// ======================== ======================== ========================
int cli_helper(int argc, char *argv[], const char *version, const char *logo,
               std::stop_token external_stop) {

  // ------------------------ ------------------------
  // Register a signal handler
  // 注册信号处理函数
  // ------------------------ ------------------------
  sigset_t signals;
  sigemptyset(&signals);

  sigaddset(&signals, SIGINT);
  sigaddset(&signals, SIGTERM);
  sigaddset(&signals, SIGUSR1);

  const int mask_status = pthread_sigmask(SIG_BLOCK, &signals, nullptr);

  if (mask_status != 0) {
    std::cerr << "Failed to block process signals.\n";
    return EXIT_FAILURE;
  }

  // ------------------------ ------------------------
  // Parse
  // 解析
  // ------------------------ ------------------------
  static const option long_options[] = {
      // ------------------------
      // CLI commands
      // Cli命令
      // ------------------------
      {"help", no_argument, nullptr, 'h'},
      {"version", no_argument, nullptr, 'v'},
      // ------------------------
      // Service commands
      // Service命令
      // ------------------------
      {"midi", required_argument, nullptr, 'm'}, // midi_path
      {"piano", no_argument, nullptr, 'p'},
      {"keyboard", no_argument, nullptr, 'k'},
      {"export", required_argument, nullptr, 'e'}, // midi_path
      {"record", no_argument, nullptr, 'r'},
      {"test", no_argument, nullptr, 't'},
      {"internal-test", no_argument, nullptr, 'i'},
      // ------------------------
      // Basic option ending
      // 基本option结尾
      // ------------------------
      {nullptr, 0, nullptr, 0}};
  int opt;
  // ------------------------
  // CLI commands structs
  // cli 命令结构体
  // ------------------------
  struct CliOptions {
    // ------------------------
    // CLI commands
    // Cli命令
    // ------------------------
    bool help = false;
    bool version = false;
    // ------------------------
    // Service commands
    // Service命令
    // ------------------------
    std::optional<std::string> midi = std::nullopt;
    bool piano = false;
    bool keyboard = false;
    std::optional<std::string> export_path = std::nullopt;
    bool record = false;
    bool test = false;
    bool internal_test = false;
  };
  CliOptions options;

  while ((opt = getopt_long(argc, argv, "hvm:pke:rti", long_options,
                            nullptr)) != -1) {
    switch (opt) {
    // ------------------------
    // CLI commands
    // Cli命令
    // ------------------------
    case 'h':
      options.help = true;
      break;
    case 'v':
      options.version = true;
      break;
    // ------------------------
    // Service commands
    // Service命令
    // ------------------------
    case 'm':
      options.midi = optarg;
      break;
    case 'p':
      options.piano = true;
      break;
    case 'k':
      options.keyboard = true;
      break;
    case 'e':
      options.export_path = optarg;
      break;
    case 'r':
      options.record = true;
      break;
    case 't':
      options.test = true;
      break;
    case 'i':
      options.internal_test = true;
      break;
    default:
      return EXIT_FAILURE;
    }
  }

  // ------------------------ ------------------------
  // Router
  // 路由
  // ------------------------ ------------------------
  // ------------------------
  // Basic threshold
  // 基本门槛
  // ------------------------
  if (!(options.help || options.version || options.midi || options.piano ||
        options.keyboard || options.export_path || options.record ||
        options.test || options.internal_test)) {
    print_logo_and_version(version, logo);
    return EXIT_FAILURE;
  }
  // ------------------------
  // CLI commands
  // Cli命令
  // ------------------------
  if (options.help) {
    help();
    return EXIT_SUCCESS;
  }
  if (options.version) {
    print_logo_and_version(version, logo);
    return EXIT_SUCCESS;
  }
  // ------------------------
  // Service commands
  // Service命令
  // ------------------------
  std::stop_source service_stop;
  const std::stop_token stop_token = service_stop.get_token();
  std::stop_callback external_stop_callback(
      external_stop, [&service_stop] { service_stop.request_stop(); });

  // 在导出以及测试模式之外都启动声卡
  const bool start_soundcard =
      !options.export_path.has_value() && !options.test;
  EngineLifetime engine(start_soundcard);

  // 信号等待线程：
  std::jthread signal_waiter(
      [signals, &service_stop](std::stop_token thread_stop) {
        const pthread_t own_thread = pthread_self();

        // jthread 自己销毁时，用 SIGUSR1 唤醒 sigwait。
        std::stop_callback wake_on_thread_stop(thread_stop, [own_thread] {
          static_cast<void>(pthread_kill(own_thread, SIGUSR1));
        });

        int received_signal = 0;

        if (sigwait(&signals, &received_signal) == 0 &&
            received_signal != SIGUSR1) {
          service_stop.request_stop();
        }
      });

  // 音频故障等待线程
  std::jthread engine_watcher([&service_stop] {
    const bool failed =
        wait_for_stop_or_audio_failure(service_stop.get_token());

    if (failed && !service_stop.stop_requested()) {
      service_stop.request_stop();
    }
  });

  std::exception_ptr service_error;

  try {
    if (options.midi) {
      print_logo_and_version(version, logo);
      midi_service(*options.midi, stop_token);
    } else if (options.piano) {
      print_logo_and_version(version, logo);
      piano_service(stop_token);
    } else if (options.keyboard) {
      print_logo_and_version(version, logo);
      keyboard_service(stop_token);
    } else if (options.export_path) {
      print_logo_and_version(version, logo);
      export_service(*options.export_path, stop_token);
    } else if (options.record) {
      print_logo_and_version(version, logo);
      record_service(stop_token);
    } else if (options.test) {
      print_logo_and_version(version, logo);
      test_service(stop_token);
    } else if (options.internal_test) {
      print_logo_and_version(version, logo);
      internal_test_service(stop_token);
    }
  } catch (...) {
    service_error = std::current_exception();
  }

  // 无论正常结束还是异常，都先通知所有等待者。
  service_stop.request_stop();
  signal_waiter.request_stop();

  signal_waiter.join();
  engine_watcher.join();

  // 关闭 gate 并等待 callback 退出。
  soundcard_shutdown();

  const bool render_failed = audio_render_failed();

  if (service_error) {
    try {
      std::rethrow_exception(service_error);
    } catch (const std::exception &error) {
      std::cerr << "bbpl: " << error.what() << '\n';
    } catch (...) {
      std::cerr << "bbpl: unknown service failure.\n";
    }

    return EXIT_FAILURE;
  }

  if (render_failed) {
    std::cerr << "The audio render callback failed.\n";
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

// ======================== ======================== ========================
// Internal implementation
// 内部实现
// ======================== ======================== ========================
namespace {

// ------------------------ ------------------------
// CLI functions
// cli 函数
// ------------------------ ------------------------
void print_logo_and_version(const char *version, const char *logo) {
  std::cout << logo;
  //    std::cout<<version<<"\n\n";
}

void help() {
  std::cout <<
      R"(
    bbpl - bBpiano physical-modeling piano engine

    Usage:
      bbpl <command>

    Playback:
      -k, --keyboard          Play using the computer keyboard
      -p, --piano             Play using a connected MIDI keyboard
      -m, --midi <file.mid>   Play a Standard MIDI File

    Recording and export:
      -r, --record            Play from a MIDI keyboard and record to MIDI
      -e, --export <file.mid> Render a MIDI file to WAV

    Diagnostics:
      -t, --test              Run the service-lifecycle smoke test
      -i, --internal-test     Play the built-in A4 engine test

    General:
      -h, --help              Show this help and exit
      -v, --version           Show version information and exit

    Examples:
      bbpl --keyboard
      bbpl --piano
      bbpl --record
      bbpl --midi "performance.mid"
      bbpl --export "performance.mid"

    Notes:
      Specify exactly one command per invocation.
      Press Ctrl-C to stop a running service cleanly.
      --record creates a timestamped MIDI file in the current directory.
      --export writes <file.wav> beside the input file without opening
      the audio output device. Existing output files are not overwritten.

    )";
}

// ------------------------ ------------------------
// Engine functions
// Engine 函数
// ------------------------ ------------------------
void init_engine(bool start_soundcard) {

  clear_audio_render_failure();

  try {
    // ------------------------
    // Initialize bbpl
    // 初始化 bbpl
    // ------------------------
    bbpiano_init(sample_rate);

    // ------------------------
    // Initialize eval
    // 初始化 eval
    // ------------------------
    eval_init(sample_rate);

    // ------------------------
    // Initialize soundcard
    // 初始化声卡驱动
    // ------------------------
    if (start_soundcard) {
      soundcard_init(sample_rate);
    }
  } catch (...) {
    shutdown_engine();
    throw;
  }
}
void shutdown_engine() {
  // 先关闭并排空 callback。
  soundcard_shutdown();

  // 此时 service producer 和 audio consumer 都必须已停止。
  clear_piano_commands();

  // 最后释放 callback 依赖。
  eval_shutdown();
  bbpiano_shutdown();
}

} // namespace
#ifndef bbpl_c_api_h
#define bbpl_c_api_h

#ifdef __cplusplus
extern "C" {
#endif

// ========================
// MIDI Service
// ========================

void midi_service_start(const char* midi_path);
void midi_service_stop(void);


// ========================
// Piano Service
// ========================

void piano_service_start(void);
void piano_service_stop(void);


// ========================
// Keyboard Service
// ========================

void keyboard_service_start(void);
void keyboard_service_stop(void);


// ========================
// Export Service
// ========================

void export_service_start(const char* midi_path);
void export_service_stop(void);


// ========================
// Record Service
// ========================

void record_service_start(void);
void record_service_stop(void);


// ========================
// Test Service
// ========================

void test_service_start(void);
void test_service_stop(void);


// ========================
// Internal Test Service
// ========================

void internal_test_service_start(void);
void internal_test_service_stop(void);

// ========================
// Shotdown All Service
// ========================
void close_all_service(void);

#ifdef __cplusplus
}
#endif

#endif /* bbpl_c_api_h */


#include <stop_token>
#include <thread>

namespace {

static constexpr const char *version_c_api = "L1-Clavier";

static constexpr const char *logo_c_api =
    R"(
            ┌────────────────────────────┐──╭────╮
            │                            │==│╲╱╲╱│
            │  bBpiano 1                 │==│╱╲╱╲│
            │  L1-Clavier/260903         │==│╲╱╲╱│
            │                            │==│╱╲╱╲│
            │  Physical Modeling Piano   │==│╲╱╲╱│
            │                            │==│╱╲╱╲│
            │  Developed by              │==│╲╱╲╱│
            │    Ziyang Tan              │==│╱╲╱╲│
            │                            │==│╲╱╲╱│
            │  bBSonicLab                │==│╱╲╱╲│
            └────────────────────────────┘──╰────╯

            With Special Thanks to
            Zhuoran Chen
            for the conversations and inspiration

            bBSonicLab Technology
            Copyright (c) 2026 Ziyang Tan.
            All rights reserved.

        )";

// Each service owns one independent jthread.
//
// request_stop() only affects the corresponding invocation.
// cli_entry() receives the jthread stop_token and bridges it into
// its internal service_stop source.
std::jthread midi_thread;
std::jthread piano_thread;
std::jthread keyboard_thread;
std::jthread export_thread;
std::jthread record_thread;
std::jthread test_thread;
std::jthread internal_test_thread;

// ========================
// Thread helper
// ========================

void stop_thread(std::jthread &thread) {
  if (!thread.joinable()) {
    return;
  }

  thread.request_stop();
  thread.join();
}

} // namespace

// ========================
// MIDI Service
// ========================

void midi_service_start(const char *midi_path) {
  if (midi_path == nullptr) {
    return;
  }

  stop_thread(midi_thread);

  std::string path(midi_path);

  midi_thread = std::jthread(
      [path = std::move(path)](std::stop_token stop_token) mutable {
        char program[] = "bbpl";
        char option[] = "-m";

        char *argv[] = {program, option, path.data()};

        cli_entry(3, argv, version_c_api, logo_c_api, stop_token);
      });
}

void midi_service_stop(void) { stop_thread(midi_thread); }

// ========================
// Piano Service
// ========================

void piano_service_start(void) {
  stop_thread(piano_thread);

  piano_thread = std::jthread([](std::stop_token stop_token) {
    char program[] = "bbpl";
    char option[] = "-p";

    char *argv[] = {program, option};

    cli_entry(2, argv, version_c_api, logo_c_api, stop_token);
  });
}

void piano_service_stop(void) { stop_thread(piano_thread); }

// ========================
// Keyboard Service
// ========================

void keyboard_service_start(void) {
  stop_thread(keyboard_thread);

  keyboard_thread = std::jthread([](std::stop_token stop_token) {
    char program[] = "bbpl";
    char option[] = "-k";

    char *argv[] = {program, option};

    cli_entry(2, argv, version_c_api, logo_c_api, stop_token);
  });
}

void keyboard_service_stop(void) { stop_thread(keyboard_thread); }

// ========================
// Export Service
// ========================

void export_service_start(const char *midi_path) {
  if (midi_path == nullptr) {
    return;
  }

  stop_thread(export_thread);

  std::string path(midi_path);

  export_thread = std::jthread(
      [path = std::move(path)](std::stop_token stop_token) mutable {
        char program[] = "bbpl";
        char option[] = "-e";

        char *argv[] = {program, option, path.data()};

        cli_entry(3, argv, version_c_api, logo_c_api, stop_token);
      });
}

void export_service_stop(void) { stop_thread(export_thread); }

// ========================
// Record Service
// ========================

void record_service_start(void) {
  stop_thread(record_thread);

  record_thread = std::jthread([](std::stop_token stop_token) {
    char program[] = "bbpl";
    char option[] = "-r";

    char *argv[] = {program, option};

    cli_entry(2, argv, version_c_api, logo_c_api, stop_token);
  });
}

void record_service_stop(void) { stop_thread(record_thread); }

// ========================
// Test Service
// ========================

void test_service_start(void) {
  stop_thread(test_thread);

  test_thread = std::jthread([](std::stop_token stop_token) {
    char program[] = "bbpl";
    char option[] = "-t";

    char *argv[] = {program, option};

    cli_entry(2, argv, version_c_api, logo_c_api, stop_token);
  });
}

void test_service_stop(void) { stop_thread(test_thread); }

// ========================
// Internal Test Service
// ========================

void internal_test_service_start(void) {
  stop_thread(internal_test_thread);

  internal_test_thread = std::jthread([](std::stop_token stop_token) {
    char program[] = "bbpl";
    char option[] = "-i";

    char *argv[] = {program, option};

    cli_entry(2, argv, version_c_api, logo_c_api, stop_token);
  });
}

void internal_test_service_stop(void) { stop_thread(internal_test_thread); }

// ========================
// Shutdown All Services
// ========================

void close_all_service(void) {
  midi_service_stop();
  piano_service_stop();
  keyboard_service_stop();
  export_service_stop();
  record_service_stop();
  test_service_stop();
  internal_test_service_stop();
}
