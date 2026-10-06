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

#ifndef string_model_hpp
#define string_model_hpp

#include <cmath>
#include <numbers>
#include <stdexcept>
#include <string>
#include <vector>

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
#ifndef dispersion_filter_hpp
#define dispersion_filter_hpp

#include <array>
#include <cmath>
#include <cstddef>

// External PT8 direct-partial adaptive fit; float32/exact-root verified.
class DispersionFilter {
  inline static constexpr std::size_t kMaxSections = 6;
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
  std::array<float, kMaxSections> x1_{}, x2_{}, y1_{}, y2_{};

public:
  explicit DispersionFilter(double f) noexcept : preset_(lookup(f)) {}
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
    x1_.fill(0);
    x2_.fill(0);
    y1_.fill(0);
    y2_.fill(0);
  }
  [[nodiscard]] inline double state_energy() const noexcept {
    double e = 0;
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
  [[nodiscard]] inline static const Preset &lookup(double f) noexcept {
    std::size_t best = 0;
    double d = std::abs(kPresets[0].referenceF1 - f);
    for (std::size_t i = 1; i < kPresets.size(); ++i) {
      double q = std::abs(kPresets[i].referenceF1 - f);
      if (q < d) {
        d = q;
        best = i;
      }
    }
    return kPresets[best];
  }
  inline static constexpr std::array<Preset, 237> kPresets = {{
      // string 0, MIDI 21; direct partial RMS 0.762307 cent
      Preset{21,
             27.3782,
             5.4850291759431087e-05,
             44100,
             1256.1791970538138,
             5,
             {{SOS{-1.97561741f, 0.975828409f}, SOS{-1.97552645f, 0.976193845f},
               SOS{-1.97369206f, 0.975448072f}, SOS{-1.97081971f, 0.974156499f},
               SOS{-1.97242332f, 0.978192449f}, SOS{0.0f, 0.0f}}}},
      // string 1, MIDI 22; direct partial RMS 1.99171 cent
      Preset{22,
             29.010100000000001,
             5.3600800116909449e-05,
             44100,
             1175.7458552382921,
             5,
             {{SOS{-1.96372128f, 0.964050353f}, SOS{-1.9717077f, 0.972444832f},
               SOS{-1.96961272f, 0.971636713f}, SOS{-1.96402633f, 0.968087614f},
               SOS{-1.96275449f, 0.969608963f}, SOS{0.0f, 0.0f}}}},
      // string 2, MIDI 23; direct partial RMS 0.932665 cent
      Preset{
          23,
          30.7393,
          5.2581980993204519e-05,
          44100,
          1150.0474452299304,
          6,
          {{SOS{-1.91192961f, 0.913868785f}, SOS{-1.94920659f, 0.950945914f},
            SOS{-1.91715014f, 0.918866158f}, SOS{-1.61721599f, 0.659784973f},
            SOS{-1.95656514f, 0.960707247f}, SOS{-1.95035338f, 0.959751129f}}}},
      // string 3, MIDI 24; direct partial RMS 0.786542 cent
      Preset{24,
             32.571599999999997,
             5.17816694448717e-05,
             44100,
             1200.8506824441356,
             3,
             {{SOS{-1.92620552f, 0.928909123f}, SOS{-1.90365779f, 0.905978262f},
               SOS{-1.91570508f, 0.923792899f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 4, MIDI 25; direct partial RMS 0.558929 cent
      Preset{25,
             34.513100000000001,
             5.1190427994123997e-05,
             44100,
             1144.0504372404769,
             3,
             {{SOS{-1.87815917f, 0.882206082f}, SOS{-1.87360358f, 0.878423035f},
               SOS{-1.89597535f, 0.903852522f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 5, MIDI 26; direct partial RMS 0.453162 cent
      Preset{26,
             36.570300000000003,
             5.0801325194743499e-05,
             44100,
             1048.4193343558957,
             4,
             {{SOS{-1.94385779f, 0.944940805f}, SOS{-1.93963969f, 0.943285704f},
               SOS{-1.92240548f, 0.932544053f}, SOS{-1.87991309f, 0.900426209f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 6, MIDI 27; direct partial RMS 0.904841 cent
      Preset{
          27,
          38.7502,
          5.0609819378936862e-05,
          44100,
          917.227027224621,
          6,
          {{SOS{-1.93205214f, 0.955200076f}, SOS{-1.93604088f, 0.951059163f},
            SOS{-1.94753456f, 0.956511199f}, SOS{-1.95472836f, 0.959277093f},
            SOS{-1.95814013f, 0.960000515f}, SOS{-1.96135461f, 0.961906552f}}}},
      // string 7, MIDI 28; direct partial RMS 1.10258 cent
      Preset{
          28,
          41.060000000000002,
          5.0613704795535706e-05,
          44100,
          865.67404586467183,
          6,
          {{SOS{-1.93850732f, 0.955186605f}, SOS{-1.94436634f, 0.953947306f},
            SOS{-1.92963254f, 0.956615448f}, SOS{-1.95022285f, 0.955191553f},
            SOS{-1.95876062f, 0.95939666f}, SOS{-1.95195532f, 0.95412153f}}}},
      // string 8, MIDI 29; direct partial RMS 1.03674 cent
      Preset{29,
             43.5075,
             5.081302372431709e-05,
             44100,
             875.56928074648386,
             4,
             {{SOS{-1.90810955f, 0.910220563f}, SOS{-1.92635751f, 0.930483162f},
               SOS{-1.91664648f, 0.928137422f}, SOS{-1.89224124f, 0.917906106f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 9, MIDI 30; direct partial RMS 0.8348 cent
      Preset{
          30,
          46.1008,
          5.1210071831366423e-05,
          44100,
          770.30917108001245,
          6,
          {{SOS{-1.94940615f, 0.952197254f}, SOS{-1.93999875f, 0.94663614f},
            SOS{-1.9564203f, 0.957157433f}, SOS{-1.93214333f, 0.951343656f},
            SOS{-1.93162382f, 0.943187416f}, SOS{-1.92634833f, 0.956761956f}}}},
      // string 10, MIDI 31; direct partial RMS 0.450342 cent
      Preset{31,
             48.824800000000003,
             5.180354900715938e-05,
             44100,
             780.05071239915003,
             5,
             {{SOS{-1.84576738f, 0.867045403f}, SOS{-1.83324111f, 0.856321812f},
               SOS{-1.89549732f, 0.903491855f}, SOS{-1.91561091f, 0.9180215f},
               SOS{-1.80722249f, 0.85793072f}, SOS{0.0f, 0.0f}}}},
      // string 11, MIDI 31; direct partial RMS 0.450098 cent
      Preset{31,
             48.848799999999997,
             5.1809506247468992e-05,
             44100,
             780.05009007276419,
             5,
             {{SOS{-1.80101502f, 0.85103637f}, SOS{-1.80756569f, 0.834348261f},
               SOS{-1.85942256f, 0.879543066f}, SOS{-1.91567767f, 0.918087423f},
               SOS{-1.8964113f, 0.904385269f}, SOS{0.0f, 0.0f}}}},
      // string 12, MIDI 32; direct partial RMS 0.524592 cent
      Preset{32,
             51.735100000000003,
             5.2610545216462892e-05,
             44100,
             770.05006467739531,
             3,
             {{SOS{-1.83070004f, 0.837868094f}, SOS{-1.83421659f, 0.860558987f},
               SOS{-1.85556376f, 0.865851521f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 13, MIDI 32; direct partial RMS 0.523896 cent
      Preset{32,
             51.7605,
             5.2618306901262375e-05,
             44100,
             769.6446342298284,
             3,
             {{SOS{-1.8307873f, 0.83794558f}, SOS{-1.85589695f, 0.866165459f},
               SOS{-1.83452427f, 0.860902846f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 14, MIDI 33; direct partial RMS 0.881645 cent
      Preset{33,
             54.818899999999999,
             5.3636409104809483e-05,
             44100,
             748.95618998032705,
             2,
             {{SOS{-1.81941128f, 0.837222934f}, SOS{-1.79409838f, 0.804697275f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}}}},
      // string 15, MIDI 33; direct partial RMS 0.881668 cent
      Preset{33,
             54.845799999999997,
             5.3646069605784835e-05,
             44100,
             748.58648791269366,
             2,
             {{SOS{-1.81932974f, 0.837156832f}, SOS{-1.79400969f, 0.804617643f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}}}},
      // string 16, MIDI 34; direct partial RMS 0.572237 cent
      Preset{34,
             58.084000000000003,
             5.4892370405986803e-05,
             44100,
             646.05007144786737,
             5,
             {{SOS{-1.89789987f, 0.901324987f}, SOS{-1.85185313f, 0.861660242f},
               SOS{-1.87971151f, 0.893190444f}, SOS{-1.87204635f, 0.899526238f},
               SOS{-1.86635423f, 0.912846982f}, SOS{0.0f, 0.0f}}}},
      // string 17, MIDI 34; direct partial RMS 0.633117 cent
      Preset{34,
             58.1126,
             5.4904088538290463e-05,
             44100,
             685.7487941932003,
             3,
             {{SOS{-1.84933233f, 0.885742486f}, SOS{-1.87260377f, 0.886826158f},
               SOS{-1.88976836f, 0.89396739f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}}}},
      // string 18, MIDI 35; direct partial RMS 0.418491 cent
      Preset{35,
             61.543700000000001,
             5.6394372616627355e-05,
             44100,
             662.67040836747901,
             2,
             {{SOS{-1.82757664f, 0.850194752f}, SOS{-1.85171771f, 0.858706355f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}}}},
      // string 19, MIDI 35; direct partial RMS 0.418496 cent
      Preset{35,
             61.573900000000002,
             5.6408210716939515e-05,
             44100,
             662.34615361778719,
             2,
             {{SOS{-1.82748187f, 0.850121617f}, SOS{-1.85164189f, 0.858637571f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}}}},
      // string 20, MIDI 36; direct partial RMS 0.546915 cent
      Preset{36,
             65.209400000000002,
             5.8160810311740388e-05,
             44100,
             596.04981384202756,
             4,
             {{SOS{-1.79989874f, 0.871051073f}, SOS{-1.84324586f, 0.873099267f},
               SOS{-1.85206687f, 0.865746737f}, SOS{-1.87699485f, 0.881926f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 21, MIDI 36; direct partial RMS 0.473112 cent
      Preset{36,
             65.241399999999999,
             5.8176980625087649e-05,
             44100,
             586.26597416569825,
             4,
             {{SOS{-1.84031534f, 0.890000165f}, SOS{-1.86913574f, 0.894349515f},
               SOS{-1.89406621f, 0.897736728f}, SOS{-1.88252389f, 0.892955363f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 22, MIDI 37; direct partial RMS 0.706057 cent
      Preset{37,
             69.093400000000003,
             6.0213806022023323e-05,
             44100,
             590.64592913210822,
             2,
             {{SOS{-1.8506279f, 0.858371317f}, SOS{-1.81322587f, 0.842635274f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}}}},
      // string 23, MIDI 37; direct partial RMS 0.706075 cent
      Preset{37,
             69.127300000000005,
             6.0232509095508895e-05,
             44100,
             590.35650285055772,
             2,
             {{SOS{-1.85055399f, 0.858304918f}, SOS{-1.81312394f, 0.842561543f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}}}},
      // string 24, MIDI 38; direct partial RMS 0.875903 cent
      Preset{38,
             73.208799999999997,
             6.2579636542420321e-05,
             44100,
             540.96645325977465,
             3,
             {{SOS{-1.77542222f, 0.83026284f}, SOS{-1.83614564f, 0.857392848f},
               SOS{-1.86275458f, 0.8690781f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}}}},
      // string 25, MIDI 38; direct partial RMS 0.875939 cent
      Preset{38,
             73.244699999999995,
             6.2601105219795922e-05,
             44100,
             540.70283446011558,
             3,
             {{SOS{-1.86268044f, 0.869010687f}, SOS{-1.77522743f, 0.830127835f},
               SOS{-1.83604038f, 0.857310891f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 26, MIDI 38; direct partial RMS 0.954322 cent
      Preset{38,
             73.280699999999996,
             6.2622647886681856e-05,
             44100,
             543.94986947803545,
             3,
             {{SOS{-1.79897559f, 0.826723218f}, SOS{-1.84655499f, 0.854426205f},
               SOS{-1.69159698f, 0.753333986f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 27, MIDI 39; direct partial RMS 0.853122 cent
      Preset{39,
             77.569199999999995,
             6.5289084468286726e-05,
             44100,
             464.55815692925933,
             5,
             {{SOS{-1.86808908f, 0.872439206f}, SOS{-1.89379632f, 0.900982618f},
               SOS{-1.88958466f, 0.907149732f}, SOS{-1.87972772f, 0.915879428f},
               SOS{-1.85997808f, 0.923252761f}, SOS{0.0f, 0.0f}}}},
      // string 28, MIDI 39; direct partial RMS 1.07696 cent
      Preset{39,
             77.607399999999998,
             6.5313716505639545e-05,
             44100,
             480.05028445125447,
             5,
             {{SOS{-1.8432864f, 0.849597394f}, SOS{-1.81370378f, 0.848389804f},
               SOS{-1.86131322f, 0.873798668f}, SOS{-1.74893141f, 0.803318262f},
               SOS{-1.74191105f, 0.825929582f}, SOS{0.0f, 0.0f}}}},
      // string 29, MIDI 39; direct partial RMS 1.03207 cent
      Preset{39,
             77.645499999999998,
             6.5338299358686647e-05,
             44100,
             478.05031913935181,
             5,
             {{SOS{-1.83910131f, 0.845573485f}, SOS{-1.66116881f, 0.732892215f},
               SOS{-1.86410344f, 0.875801086f}, SOS{-1.79301178f, 0.856918335f},
               SOS{-1.83166659f, 0.862691164f}, SOS{0.0f, 0.0f}}}},
      // string 30, MIDI 40; direct partial RMS 0.456843 cent
      Preset{
          40,
          82.189499999999995,
          6.8378564983695274e-05,
          44100,
          449.94992592826696,
          6,
          {{SOS{-1.83181882f, 0.839642048f}, SOS{-1.82127488f, 0.838442802f},
            SOS{-1.74467003f, 0.778825104f}, SOS{-1.77838814f, 0.831142604f},
            SOS{-1.68879676f, 0.768835366f}, SOS{-1.71406794f, 0.879858971f}}}},
      // string 31, MIDI 40; direct partial RMS 0.425433 cent
      Preset{40,
             82.229900000000001,
             6.8406550346842109e-05,
             44100,
             477.94996872428879,
             4,
             {{SOS{-1.76762152f, 0.781171978f}, SOS{-1.8027842f, 0.822329521f},
               SOS{-1.77790082f, 0.830524087f}, SOS{0.212062538f, 0.161472589f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 32, MIDI 40; direct partial RMS 0.368584 cent
      Preset{
          40,
          82.270200000000003,
          6.8434483060773974e-05,
          44100,
          440.04997722859707,
          6,
          {{SOS{-1.82780993f, 0.835303366f}, SOS{-1.85174644f, 0.862174392f},
            SOS{-1.8434484f, 0.866375983f}, SOS{-1.72853351f, 0.810030282f},
            SOS{-1.81772447f, 0.865983486f}, SOS{-1.70036685f, 0.808718562f}}}},
      // string 33, MIDI 41; direct partial RMS 0.964482 cent
      Preset{41,
             87.084900000000005,
             7.1890266102344605e-05,
             44100,
             454.05018390197006,
             3,
             {{SOS{-1.76347291f, 0.777459264f}, SOS{-1.66741228f, 0.742573142f},
               SOS{-1.77968466f, 0.807876945f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 34, MIDI 41; direct partial RMS 1.04945 cent
      Preset{
          41,
          87.127700000000004,
          7.1922034626807979e-05,
          44100,
          460.04990662643326,
          5,
          {{SOS{-0.953317821f, 0.432814002f}, SOS{-0.733706534f, 0.522687376f},
            SOS{-1.0167892f, 0.568364084f}, SOS{-1.76161194f, 0.79612869f},
            SOS{-1.74281156f, 0.759355545f}, SOS{0.0f, 0.0f}}}},
      // string 35, MIDI 41; direct partial RMS 0.899354 cent
      Preset{41,
             87.170500000000004,
             7.1953821532202286e-05,
             44100,
             451.02797456738773,
             3,
             {{SOS{-1.77740252f, 0.789789855f}, SOS{-1.8012805f, 0.825034499f},
               SOS{-1.73827446f, 0.80238843f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}}}},
      // string 36, MIDI 42; direct partial RMS 0.184543 cent
      Preset{42,
             92.271900000000002,
             7.5873719965610862e-05,
             44100,
             402.04987305491164,
             5,
             {{SOS{-1.8123194f, 0.82188946f}, SOS{-1.80629373f, 0.825213552f},
               SOS{-1.76036572f, 0.795512676f}, SOS{-1.76910222f, 0.830251336f},
               SOS{-1.70921075f, 0.827708483f}, SOS{0.0f, 0.0f}}}},
      // string 37, MIDI 42; direct partial RMS 0.287128 cent
      Preset{42,
             92.3172,
             7.5909690237425587e-05,
             44100,
             432.05002328747639,
             3,
             {{SOS{-1.63367343f, 0.667850912f}, SOS{-1.68208885f, 0.727181792f},
               SOS{-1.50401282f, 0.565943062f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 38, MIDI 42; direct partial RMS 0.183587 cent
      Preset{42,
             92.362499999999997,
             7.594568091914677e-05,
             44100,
             425.95006172308916,
             3,
             {{SOS{-1.7792064f, 0.803349495f}, SOS{-1.74647772f, 0.762872577f},
               SOS{-1.72785735f, 0.794150054f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 39, MIDI 43; direct partial RMS 0.272308 cent
      Preset{43,
             97.767799999999994,
             8.0386576743260001e-05,
             44100,
             414.0498857291692,
             2,
             {{SOS{-1.76342881f, 0.780166566f}, SOS{-1.71989155f, 0.764460504f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}}}},
      // string 40, MIDI 43; direct partial RMS 0.265258 cent
      Preset{43,
             97.815799999999996,
             8.0427313370761084e-05,
             44100,
             406.04996444490632,
             3,
             {{SOS{-1.76548958f, 0.781960964f}, SOS{-1.71794689f, 0.763104439f},
               SOS{-1.37856054f, 0.507372677f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 41, MIDI 43; direct partial RMS 0.268029 cent
      Preset{43,
             97.863900000000001,
             8.0468157850107554e-05,
             44100,
             413.33409135038357,
             2,
             {{SOS{-1.7280072f, 0.772561789f}, SOS{-1.77124584f, 0.787077367f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}}}},
      // string 42, MIDI 44; direct partial RMS 0.493808 cent
      Preset{
          44,
          103.59099999999999,
          8.5496148564465589e-05,
          44100,
          382.05001468336422,
          4,
          {{SOS{-1.70036209f, 0.744959593f}, SOS{-1.69032812f, 0.715087414f},
            SOS{-1.11455858f, 0.315571189f}, SOS{-0.195824429f, 0.0179404821f},
            SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 43, MIDI 44; direct partial RMS 0.434244 cent
      Preset{44,
             103.642,
             8.5542394832813175e-05,
             44100,
             370.05000543534283,
             4,
             {{SOS{-1.71443594f, 0.735205114f}, SOS{-1.72169161f, 0.752072036f},
               SOS{-1.67906368f, 0.741820395f}, SOS{-1.53495824f, 0.660775542f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 44, MIDI 44; direct partial RMS 0.401005 cent
      Preset{44,
             103.693,
             8.5588667164501184e-05,
             44100,
             371.94996486118373,
             4,
             {{SOS{-1.76678813f, 0.781279266f}, SOS{-1.77562404f, 0.80175823f},
               SOS{-1.73201358f, 0.798626423f}, SOS{-1.5806005f, 0.793981969f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 45, MIDI 45; direct partial RMS 0.344187 cent
      Preset{
          45,
          109.761,
          9.1281010195021513e-05,
          44100,
          313.95071330612313,
          6,
          {{SOS{-1.85649168f, 0.883714736f}, SOS{-1.87194347f, 0.88369137f},
            SOS{-1.87455583f, 0.879267097f}, SOS{-1.83531916f, 0.887555599f},
            SOS{-1.79912043f, 0.887820125f}, SOS{-1.76900101f, 0.908342361f}}}},
      // string 46, MIDI 45; direct partial RMS 0.416645 cent
      Preset{
          45,
          109.815,
          9.133333879731857e-05,
          44100,
          327.94996993667502,
          6,
          {{SOS{-1.81718612f, 0.826586545f}, SOS{-1.81402588f, 0.833453596f},
            SOS{-1.77758944f, 0.825789869f}, SOS{-1.59512293f, 0.675846279f},
            SOS{-1.72433889f, 0.820507169f}, SOS{-1.78278208f, 0.9509148f}}}},
      // string 47, MIDI 45; direct partial RMS 0.450753 cent
      Preset{45,
             109.869,
             9.1385697045794626e-05,
             44100,
             367.31881904436028,
             3,
             {{SOS{1.99399447f, 0.994009018f}, SOS{-1.70000136f, 0.752823293f},
               SOS{-1.74489331f, 0.764012933f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 48, MIDI 46; direct partial RMS 0.264729 cent
      Preset{46,
             116.295,
             9.7829378225853023e-05,
             44100,
             346.05010452595053,
             2,
             {{SOS{-1.7307018f, 0.751758754f}, SOS{-1.68997049f, 0.744265735f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}}}},
      // string 49, MIDI 46; direct partial RMS 0.272496 cent
      Preset{46,
             116.352,
             9.7888438760023548e-05,
             44100,
             346.04999119460979,
             2,
             {{SOS{-1.68581378f, 0.739552438f}, SOS{-1.72177672f, 0.743926644f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}}}},
      // string 50, MIDI 46; direct partial RMS 0.270107 cent
      Preset{46,
             116.41,
             9.7948570054725719e-05,
             44100,
             338.04995763831658,
             3,
             {{SOS{-1.60123682f, 0.64996171f}, SOS{-1.6085484f, 0.678918898f},
               SOS{-1.58199167f, 0.625674367f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 51, MIDI 47; direct partial RMS 0.235293 cent
      Preset{47,
             123.218,
             0.00010525133288627102,
             44100,
             326.04999048149699,
             2,
             {{SOS{-1.71862078f, 0.741641462f}, SOS{-1.66871953f, 0.726938426f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}}}},
      // string 52, MIDI 47; direct partial RMS 0.232963 cent
      Preset{47,
             123.279,
             0.0001053189775995625,
             44100,
             326.04999597635322,
             2,
             {{SOS{-1.66418004f, 0.722141862f}, SOS{-1.71114182f, 0.735150039f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}}}},
      // string 53, MIDI 47; direct partial RMS 0.236879 cent
      Preset{47,
             123.339,
             0.00010538555204720589,
             44100,
             318.04998063235035,
             3,
             {{SOS{-1.56920969f, 0.622857988f}, SOS{-1.5869875f, 0.664435506f},
               SOS{-1.60620368f, 0.648194611f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 54, MIDI 48; direct partial RMS 0.193597 cent
      Preset{
          48,
          130.553,
          0.00011367230448717747,
          44100,
          304.04997675092665,
          5,
          {{SOS{-0.132796273f, 0.0135793602f}, SOS{1.01230764f, 0.827987194f},
            SOS{0.484422207f, 0.957755923f}, SOS{-1.66607058f, 0.726877987f},
            SOS{-1.71855068f, 0.741774797f}, SOS{0.0f, 0.0f}}}},
      // string 55, MIDI 48; direct partial RMS 0.193606 cent
      Preset{48,
             130.61699999999999,
             0.00011374835315072723,
             44100,
             306.27143131786733,
             5,
             {{SOS{1.9417578f, 0.986379266f}, SOS{1.93587184f, 0.984759152f},
               SOS{1.94426513f, 0.987549067f}, SOS{-1.71844828f, 0.741689205f},
               SOS{-1.66593945f, 0.726791263f}, SOS{0.0f, 0.0f}}}},
      // string 56, MIDI 48; direct partial RMS 0.190821 cent
      Preset{
          48,
          130.68199999999999,
          0.00011382563612761529,
          44100,
          268.05004310068853,
          6,
          {{SOS{-1.69787848f, 0.722729921f}, SOS{-1.69267297f, 0.721511245f},
            SOS{-1.66998756f, 0.723823667f}, SOS{-1.6159972f, 0.690210044f},
            SOS{-1.59889317f, 0.721311569f}, SOS{-1.53093338f, 0.675382137f}}}},
      // string 57, MIDI 49; direct partial RMS 0.161078 cent
      Preset{49,
             138.32499999999999,
             0.00012324025444501751,
             44100,
             288.33200247673579,
             2,
             {{SOS{-1.71159673f, 0.73588872f}, SOS{-1.65555716f, 0.720007896f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}}}},
      // string 58, MIDI 49; direct partial RMS 0.161063 cent
      Preset{49,
             138.393,
             0.00012332696434276942,
             44100,
             288.19258974155349,
             2,
             {{SOS{-1.65534329f, 0.719861567f}, SOS{-1.7114346f, 0.735753238f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}}}},
      // string 59, MIDI 49; direct partial RMS 0.161047 cent
      Preset{49,
             138.46100000000001,
             0.00012341372685106358,
             44100,
             288.05380805269107,
             2,
             {{SOS{-1.71126699f, 0.735613167f}, SOS{-1.65512133f, 0.719709218f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}}}},
      // string 60, MIDI 50; direct partial RMS 0.16617 cent
      Preset{50,
             146.559,
             0.00013412721454709035,
             44100,
             255.95001268026917,
             4,
             {{SOS{-1.75094259f, 0.768893898f}, SOS{-1.71582925f, 0.756156802f},
               SOS{-1.66730738f, 0.757412493f}, SOS{-1.53113687f, 0.781898737f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 61, MIDI 50; direct partial RMS 0.188028 cent
      Preset{50,
             146.631,
             0.00013422590355159281,
             44100,
             264.0499473076913,
             3,
             {{SOS{-1.71829176f, 0.741890252f}, SOS{-1.63644683f, 0.705939114f},
               SOS{-1.37588906f, 0.524810493f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 62, MIDI 50; direct partial RMS 0.193088 cent
      Preset{50,
             146.703,
             0.00013432465389000837,
             44100,
             270.98122284513789,
             3,
             {{SOS{1.99399996f, 0.994009018f}, SOS{-1.64444637f, 0.71491915f},
               SOS{-1.71606958f, 0.740185022f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 63, MIDI 51; direct partial RMS 0.363384 cent
      Preset{51,
             155.28399999999999,
             0.00014653930235788255,
             44100,
             255.83484194440592,
             2,
             {{SOS{-1.69787824f, 0.724877119f}, SOS{-1.6341691f, 0.709071636f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}}}},
      // string 64, MIDI 51; direct partial RMS 0.363285 cent
      Preset{51,
             155.36000000000001,
             0.00014665148916490561,
             44100,
             255.69328190622721,
             2,
             {{SOS{-1.69782519f, 0.724830747f}, SOS{-1.63405752f, 0.708950102f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}}}},
      // string 65, MIDI 51; direct partial RMS 0.363183 cent
      Preset{51,
             155.43600000000001,
             0.00014676374729231839,
             44100,
             255.55484765906522,
             2,
             {{SOS{-1.69774973f, 0.724766076f}, SOS{-1.63392627f, 0.708822489f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}}}},
      // string 66, MIDI 52; direct partial RMS 0.208164 cent
      Preset{52,
             164.52799999999999,
             0.00016071595204200263,
             44100,
             242.05001160005759,
             3,
             {{SOS{1.31991482f, 0.769295216f}, SOS{-1.56788659f, 0.640675426f},
               SOS{-1.57678771f, 0.62387836f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}}}},
      // string 67, MIDI 52; direct partial RMS 0.193609 cent
      Preset{52,
             164.60900000000001,
             0.00016084498160109391,
             44100,
             236.04998975497838,
             3,
             {{SOS{-1.46472979f, 0.537552297f}, SOS{-1.52328551f, 0.605941415f},
               SOS{-1.31221569f, 0.43049857f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}}}},
      // string 68, MIDI 52; direct partial RMS 0.199142 cent
      Preset{52,
             164.68899999999999,
             0.00016097250147062826,
             44100,
             236.04999828674812,
             3,
             {{SOS{-1.45680451f, 0.53248477f}, SOS{-1.51357925f, 0.597271264f},
               SOS{-1.31951165f, 0.435287148f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 69, MIDI 53; direct partial RMS 0.110604 cent
      Preset{53,
             174.322,
             0.00017694240546449472,
             44100,
             218.34660497916312,
             3,
             {{SOS{-1.69234657f, 0.719554365f}, SOS{-1.62177324f, 0.692202866f},
               SOS{-1.45303488f, 0.603346109f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 70, MIDI 53; direct partial RMS 0.11059 cent
      Preset{53,
             174.40799999999999,
             0.00017709056309902615,
             44100,
             218.23961366866678,
             3,
             {{SOS{-1.69221604f, 0.719446659f}, SOS{-1.6216172f, 0.692102253f},
               SOS{-1.45282817f, 0.603286564f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 71, MIDI 53; direct partial RMS 0.113416 cent
      Preset{53,
             174.49299999999999,
             0.0001772370966770329,
             44100,
             225.95000991137317,
             2,
             {{SOS{-1.67363012f, 0.704431653f}, SOS{-1.60609734f, 0.688234985f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}}}},
      // string 72, MIDI 54; direct partial RMS 0.284507 cent
      Preset{54,
             184.69900000000001,
             0.0001951909802984074,
             44100,
             211.94996959014557,
             2,
             {{SOS{-1.6678158f, 0.699521005f}, SOS{-1.60801435f, 0.69054091f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}}}},
      // string 73, MIDI 54; direct partial RMS 0.45763 cent
      Preset{54,
             184.78999999999999,
             0.00019535414519352071,
             44100,
             214.05001384122934,
             2,
             {{SOS{-1.54874587f, 0.620200098f}, SOS{-1.48762035f, 0.553256631f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}}}},
      // string 74, MIDI 54; direct partial RMS 0.277354 cent
      Preset{54,
             184.881,
             0.00019551736416016424,
             44100,
             198.0499959747963,
             5,
             {{SOS{-1.45533264f, 0.531271398f}, SOS{-1.35787296f, 0.465750307f},
               SOS{-1.33543551f, 0.469496191f}, SOS{-1.44615507f, 0.560262322f},
               SOS{-0.336192518f, 0.118023083f}, SOS{0.0f, 0.0f}}}},
      // string 75, MIDI 55; direct partial RMS 0.240025 cent
      Preset{55,
             195.69399999999999,
             0.00021529375640180309,
             44100,
             190.05000832377476,
             3,
             {{SOS{-1.69136131f, 0.717938304f}, SOS{-1.66310835f, 0.72079885f},
               SOS{-1.55533886f, 0.69666791f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}}}},
      // string 76, MIDI 55; direct partial RMS 0.21993 cent
      Preset{55,
             195.78999999999999,
             0.00021547270321706786,
             44100,
             180.04997545983952,
             4,
             {{SOS{-1.73005056f, 0.750487506f}, SOS{-1.70790994f, 0.752524137f},
               SOS{-1.60336542f, 0.716419399f}, SOS{-1.51751494f, 0.664682388f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 77, MIDI 55; direct partial RMS 0.240835 cent
      Preset{55,
             195.886,
             0.00021565170887217493,
             44100,
             190.04998011160353,
             3,
             {{SOS{-1.68863273f, 0.715645313f}, SOS{-1.65999293f, 0.71844393f},
               SOS{-1.54738224f, 0.691302061f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 78, MIDI 56; direct partial RMS 0.270009 cent
      Preset{56,
             207.34399999999999,
             0.00023743618266172512,
             44100,
             187.47626947145369,
             2,
             {{SOS{-1.65138721f, 0.686474442f}, SOS{-1.57780647f, 0.668185413f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}}}},
      // string 79, MIDI 56; direct partial RMS 0.269984 cent
      Preset{56,
             207.44499999999999,
             0.00023763187926342927,
             44100,
             187.37568941541932,
             2,
             {{SOS{-1.57767451f, 0.66806972f}, SOS{-1.65129006f, 0.686394453f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}}}},
      // string 80, MIDI 56; direct partial RMS 0.269969 cent
      Preset{56,
             207.547,
             0.00023782957807628698,
             44100,
             187.27588565138433,
             2,
             {{SOS{-1.65117526f, 0.686300457f}, SOS{-1.57752264f, 0.667944729f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}}}},
      // string 81, MIDI 57; direct partial RMS 0.239697 cent
      Preset{57,
             219.68700000000001,
             0.00026181988958400476,
             44100,
             150.0500243558038,
             5,
             {{SOS{-1.70141172f, 0.725952089f}, SOS{-1.66816759f, 0.710098088f},
               SOS{-1.6581291f, 0.731994748f}, SOS{-1.58233917f, 0.738512754f},
               SOS{-1.41224015f, 0.617388666f}, SOS{0.0f, 0.0f}}}},
      // string 82, MIDI 57; direct partial RMS 0.268009 cent
      Preset{57,
             219.79400000000001,
             0.00026203536410030967,
             44100,
             158.05000870997708,
             5,
             {{SOS{-1.37575364f, 0.475228071f}, SOS{-1.47461915f, 0.551837325f},
               SOS{-1.40214801f, 0.506910861f}, SOS{-1.28594911f, 0.464166224f},
               SOS{-1.35327566f, 0.54272449f}, SOS{0.0f, 0.0f}}}},
      // string 83, MIDI 57; direct partial RMS 0.121587 cent
      Preset{57,
             219.90199999999999,
             0.00026225292323324228,
             44100,
             135.74440974360624,
             5,
             {{SOS{-1.82257116f, 0.831843495f}, SOS{-1.81065083f, 0.832508028f},
               SOS{-1.77790439f, 0.825349808f}, SOS{-1.76070583f, 0.840787888f},
               SOS{-1.75516939f, 0.891357183f}, SOS{0.0f, 0.0f}}}},
      // string 84, MIDI 58; direct partial RMS 0.277697 cent
      Preset{58,
             232.768,
             0.00028867597150438904,
             44100,
             133.94998287161803,
             5,
             {{SOS{-1.69730031f, 0.721301079f}, SOS{-1.70088959f, 0.735505879f},
               SOS{-1.60505247f, 0.7181831f}, SOS{-1.65851092f, 0.728623331f},
               SOS{-1.58519483f, 0.717450261f}, SOS{0.0f, 0.0f}}}},
      // string 85, MIDI 58; direct partial RMS 0.181207 cent
      Preset{58,
             232.88200000000001,
             0.00028891453989226564,
             44100,
             166.49552427208943,
             2,
             {{SOS{-1.51485682f, 0.573697805f}, SOS{-1.55158436f, 0.638001084f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}}}},
      // string 86, MIDI 58; direct partial RMS 0.00630023 cent
      Preset{58,
             232.99700000000001,
             0.00028915527950406771,
             44100,
             135.19968749557609,
             4,
             {{SOS{-1.7550329f, 0.770863712f}, SOS{-1.77666295f, 0.801575243f},
               SOS{-1.76776373f, 0.82268852f}, SOS{-1.75065327f, 0.855284274f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 87, MIDI 59; direct partial RMS 0.125428 cent
      Preset{59,
             246.62799999999999,
             0.00031824458244894441,
             44100,
             158.04999568606524,
             2,
             {{SOS{-1.48859429f, 0.558963716f}, SOS{-1.41516173f, 0.529622376f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}}}},
      // string 88, MIDI 59; direct partial RMS 0.174521 cent
      Preset{
          59,
          246.749,
          0.00031850768825534171,
          44100,
          165.76113005205127,
          1,
          {{SOS{-1.53636789f, 0.598389506f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
            SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 89, MIDI 59; direct partial RMS 0.175306 cent
      Preset{59,
             246.87100000000001,
             0.00031877305486527742,
             44100,
             165.66570789544255,
             1,
             {{SOS{-1.53665197f, 0.59860605f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 90, MIDI 60; direct partial RMS 0.394938 cent
      Preset{
          60,
          261.31400000000002,
          0.00035079667538140852,
          44100,
          156.74746405626604,
          1,
          {{SOS{-1.47983623f, 0.553979218f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
            SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 91, MIDI 60; direct partial RMS 0.394948 cent
      Preset{
          60,
          261.44200000000001,
          0.0003510858324722655,
          44100,
          156.67011966663554,
          1,
          {{SOS{-1.47962892f, 0.553829432f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
            SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 92, MIDI 60; direct partial RMS 0.394964 cent
      Preset{
          60,
          261.57100000000003,
          0.00035137734297986819,
          44100,
          156.59270234038718,
          1,
          {{SOS{-1.47939885f, 0.553662777f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
            SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 93, MIDI 61; direct partial RMS 0.302871 cent
      Preset{61,
             276.87400000000002,
             0.00038662562228112331,
             44100,
             115.94998641182315,
             4,
             {{SOS{-1.69431853f, 0.719602704f}, SOS{-1.68768716f, 0.7322855f},
               SOS{-1.63288796f, 0.728120208f}, SOS{-1.59083438f, 0.741442859f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 94, MIDI 61; direct partial RMS 0.284584 cent
      Preset{61,
             277.00999999999999,
             0.00038694476329759826,
             44100,
             116.05005513457725,
             4,
             {{SOS{-1.66865015f, 0.697217286f}, SOS{-1.68972647f, 0.73042208f},
               SOS{-1.66343975f, 0.748819828f}, SOS{-1.61079967f, 0.772055924f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 95, MIDI 61; direct partial RMS 0.376433 cent
      Preset{61,
             277.14600000000002,
             0.00038726400718821016,
             44100,
             137.9499962089794,
             3,
             {{SOS{-1.40533721f, 0.50103724f}, SOS{-1.39200425f, 0.501331091f},
               SOS{-0.576268554f, 0.364073306f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 96, MIDI 62; direct partial RMS 0.23588 cent
      Preset{62,
             293.36099999999999,
             0.00042605830918566789,
             44100,
             125.95000409686831,
             3,
             {{SOS{-1.43983138f, 0.518752396f}, SOS{-1.45462942f, 0.563627541f},
               SOS{-1.06789839f, 0.369027436f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 97, MIDI 62; direct partial RMS 0.231858 cent
      Preset{62,
             293.505,
             0.00042640927717448165,
             44100,
             125.95000093629491,
             3,
             {{SOS{-1.3932029f, 0.485260218f}, SOS{-1.05016685f, 0.29702577f},
               SOS{-1.41153252f, 0.536202371f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 98, MIDI 62; direct partial RMS 0.231865 cent
      Preset{62,
             293.649,
             0.0004267603578166185,
             44100,
             125.95000444198391,
             3,
             {{SOS{-1.39345741f, 0.485467881f}, SOS{-1.03243852f, 0.286306649f},
               SOS{-1.41258848f, 0.53706193f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}}}},
      // string 99, MIDI 63; direct partial RMS 0.33058 cent
      Preset{63,
             310.82900000000001,
             0.00046944827536626285,
             44100,
             91.950080171151683,
             4,
             {{SOS{-1.81460905f, 0.825380206f}, SOS{-1.80463636f, 0.83428359f},
               SOS{-1.77349973f, 0.846067429f}, SOS{-1.73728573f, 0.875533581f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 100, MIDI 63; direct partial RMS 0.249999 cent
      Preset{63,
             310.98200000000003,
             0.00046983553170952373,
             44100,
             119.90740810030783,
             2,
             {{SOS{-1.63480234f, 0.674715459f}, SOS{-1.55352521f, 0.671634018f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}}}},
      // string 101, MIDI 63; direct partial RMS 0.0562954 cent
      Preset{63,
             311.13400000000001,
             0.00047022037995252336,
             44100,
             98.050005780673672,
             4,
             {{SOS{-1.75590611f, 0.773005605f}, SOS{-1.75519907f, 0.790238082f},
               SOS{-1.74280059f, 0.824039221f}, SOS{-1.77896094f, 0.938246846f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 102, MIDI 64; direct partial RMS 0.37799 cent
      Preset{
          64,
          329.33699999999999,
          0.00051718777559689097,
          44100,
          122.55913384704931,
          1,
          {{SOS{-1.47278535f, 0.551728725f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
            SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 103, MIDI 64; direct partial RMS 0.377992 cent
      Preset{
          64,
          329.49900000000002,
          0.00051761353584727128,
          44100,
          122.49892949011617,
          1,
          {{SOS{-1.47255266f, 0.551564276f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
            SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 104, MIDI 64; direct partial RMS 0.000826978 cent
      Preset{64,
             329.661,
             0.00051803943206598496,
             44100,
             85.949921029226502,
             4,
             {{SOS{-1.77746117f, 0.792335093f}, SOS{-1.74501705f, 0.782088578f},
               SOS{-1.72204816f, 0.794623911f}, SOS{-1.7111454f, 0.843837678f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 105, MIDI 65; direct partial RMS 0.0534425 cent
      Preset{65,
             348.94799999999998,
             0.00056970865048497213,
             44100,
             106.04998122524377,
             2,
             {{SOS{-1.56499076f, 0.617891788f}, SOS{-1.5072993f, 0.632070899f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}}}},
      // string 106, MIDI 65; direct partial RMS 0.0567158 cent
      Preset{65,
             349.11900000000003,
             0.00057017523572402582,
             44100,
             106.04999165819454,
             2,
             {{SOS{-1.56249952f, 0.615925074f}, SOS{-1.50446403f, 0.629847646f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}}}},
      // string 107, MIDI 65; direct partial RMS 0.059189 cent
      Preset{65,
             349.291,
             0.00057064469874168445,
             44100,
             106.04999053526601,
             2,
             {{SOS{-1.56028557f, 0.614190698f}, SOS{-1.50156343f, 0.627655268f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}}}},
      // string 108, MIDI 66; direct partial RMS 0.206987 cent
      Preset{
          66,
          369.726,
          0.00062747715154224781,
          44100,
          108.34777269365895,
          1,
          {{SOS{-1.46289909f, 0.545959592f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
            SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 109, MIDI 66; direct partial RMS 0.207016 cent
      Preset{
          66,
          369.90800000000002,
          0.00062799265582099805,
          44100,
          108.29532729005696,
          1,
          {{SOS{-1.46261728f, 0.545761585f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
            SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 110, MIDI 66; direct partial RMS 0.207046 cent
      Preset{
          66,
          370.08999999999997,
          0.00062850832366120868,
          44100,
          108.24308999356471,
          1,
          {{SOS{-1.46232843f, 0.545558691f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
            SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 111, MIDI 67; direct partial RMS 0.18522 cent
      Preset{
          67,
          391.74200000000002,
          0.00069101348515588257,
          44100,
          102.23536023794553,
          1,
          {{SOS{-1.42246914f, 0.515941143f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
            SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 112, MIDI 67; direct partial RMS 0.0825962 cent
      Preset{67,
             391.93400000000003,
             0.00069157794011855437,
             44100,
             96.049988599892657,
             2,
             {{SOS{-1.51087976f, 0.579526603f}, SOS{-1.28563511f, 0.515001059f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}}}},
      // string 113, MIDI 67; direct partial RMS 0.185403 cent
      Preset{67,
             392.12700000000001,
             0.0006921455140092512,
             44100,
             102.1373268153034,
             1,
             {{SOS{-1.4218297f, 0.515500247f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 114, MIDI 68; direct partial RMS 0.228749 cent
      Preset{68,
             415.06799999999998,
             0.00076087878767108698,
             44100,
             90.049996831854457,
             2,
             {{SOS{-1.40132785f, 0.498443961f}, SOS{-1.23531437f, 0.42922911f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}}}},
      // string 115, MIDI 68; direct partial RMS 0.216463 cent
      Preset{68,
             415.27199999999999,
             0.00076150118321163189,
             44100,
             90.050003554931877,
             2,
             {{SOS{-1.40425193f, 0.501107633f}, SOS{-1.22184455f, 0.419836104f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}}}},
      // string 116, MIDI 68; direct partial RMS 0.00176634 cent
      Preset{68,
             415.476,
             0.00076212377448834864,
             44100,
             52.049881959969596,
             5,
             {{SOS{-1.85293555f, 0.860983431f}, SOS{-1.82006705f, 0.850245237f},
               SOS{-1.77923524f, 0.847789526f}, SOS{-1.73648858f, 0.866294324f},
               SOS{-1.64221585f, 0.85742408f}, SOS{0.0f, 0.0f}}}},
      // string 117, MIDI 69; direct partial RMS 0.000596963 cent
      Preset{69,
             439.78399999999999,
             0.00083769948617548108,
             44100,
             64.597930280719012,
             4,
             {{SOS{-1.719594f, 0.743334532f}, SOS{-1.67567897f, 0.738277435f},
               SOS{-1.60195076f, 0.73939234f}, SOS{-1.513659f, 0.776049137f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 118, MIDI 69; direct partial RMS 0.225643 cent
      Preset{69,
             440,
             0.00083838330327871036,
             44100,
             75.291577639267373,
             3,
             {{SOS{-1.61215842f, 0.655785203f}, SOS{-1.54449737f, 0.652785778f},
               SOS{-1.37891185f, 0.646372795f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 119, MIDI 69; direct partial RMS 0.338978 cent
      Preset{69,
             440.21600000000001,
             0.00083906733450199965,
             44100,
             73.949998325793956,
             4,
             {{SOS{-1.33123612f, 0.448966861f}, SOS{-1.26290882f, 0.411909491f},
               SOS{-1.27300584f, 0.458449602f}, SOS{-1.10220194f, 0.456748247f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 120, MIDI 70; direct partial RMS 0.000850427 cent
      Preset{70,
             465.99400000000003,
             0.00092222624045597843,
             44100,
             47.950046389724406,
             4,
             {{SOS{-1.85189188f, 0.860898197f}, SOS{-1.80926919f, 0.848365605f},
               SOS{-1.74589622f, 0.83617872f}, SOS{-1.70455754f, 0.86327368f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 121, MIDI 70; direct partial RMS 0.00227128 cent
      Preset{70,
             466.22300000000001,
             0.0009229784278811439,
             44100,
             47.949863835211467,
             4,
             {{SOS{-1.85002208f, 0.859169662f}, SOS{-1.80738449f, 0.846780717f},
               SOS{-1.74071598f, 0.831943929f}, SOS{-1.70029402f, 0.857420325f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 122, MIDI 70; direct partial RMS 0.0762854 cent
      Preset{
          70,
          466.452,
          0.00092373085009990236,
          44100,
          84.815021398302832,
          1,
          {{SOS{-1.38820076f, 0.493289798f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
            SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 123, MIDI 71; direct partial RMS 0.0269889 cent
      Preset{71,
             493.767,
             0.0010151496048357825,
             44100,
             62.050004707954301,
             4,
             {{SOS{-1.43114603f, 0.513247907f}, SOS{-1.48725891f, 0.575444043f},
               SOS{-1.44056785f, 0.617269754f}, SOS{-1.24975526f, 0.684027076f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 124, MIDI 71; direct partial RMS 0.0693765 cent
      Preset{71,
             494.00999999999999,
             0.0010159776288841188,
             44100,
             69.94999169200679,
             3,
             {{SOS{-1.50191987f, 0.571695924f}, SOS{-1.40049541f, 0.562435985f},
               SOS{-0.951542079f, 0.527846336f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 125, MIDI 71; direct partial RMS 0.0268638 cent
      Preset{71,
             494.25200000000001,
             0.0010168025017225687,
             44100,
             62.049994045836215,
             4,
             {{SOS{-1.41946125f, 0.504389226f}, SOS{-1.48749614f, 0.575066149f},
               SOS{-1.4392271f, 0.615870655f}, SOS{-1.24579155f, 0.681757569f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 126, MIDI 72; direct partial RMS 0.251561 cent
      Preset{
          72,
          523.19500000000005,
          0.0011172856755547671,
          44100,
          75.165670375930716,
          1,
          {{SOS{-1.36051321f, 0.475399137f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
            SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 127, MIDI 72; direct partial RMS 0.251501 cent
      Preset{
          72,
          523.452,
          0.0011181940327211753,
          44100,
          75.123726949064789,
          1,
          {{SOS{-1.36060619f, 0.475469589f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
            SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 128, MIDI 72; direct partial RMS 0.000219995 cent
      Preset{72,
             523.70899999999995,
             0.0011191026712183991,
             44100,
             47.949987968326525,
             4,
             {{SOS{-1.7237488f, 0.747173369f}, SOS{-1.67088199f, 0.735702515f},
               SOS{-1.57634509f, 0.712215304f}, SOS{-1.55141759f, 0.771344721f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 129, MIDI 73; direct partial RMS 0.171647 cent
      Preset{73,
             554.37599999999998,
             0.0012295302954572266,
             44100,
             64.049998116280591,
             2,
             {{SOS{-1.36679804f, 0.471606493f}, SOS{-1.31398308f, 0.498729795f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}}}},
      // string 130, MIDI 73; direct partial RMS 0.0934427 cent
      Preset{73,
             554.649,
             0.0012305310187171968,
             44100,
             62.050009773483652,
             2,
             {{SOS{-1.50772536f, 0.575759292f}, SOS{-1.44359136f, 0.60292995f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}}}},
      // string 131, MIDI 73; direct partial RMS 0.000133363 cent
      Preset{73,
             554.92100000000005,
             0.0012315283841869743,
             44100,
             61.305060527901894,
             2,
             {{SOS{-1.53759718f, 0.598483503f}, SOS{-1.4849304f, 0.632818639f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}}}},
      // string 132, MIDI 74; direct partial RMS 0.214253 cent
      Preset{74,
             587.41600000000005,
             0.0013528730659360278,
             44100,
             60.050000407357416,
             2,
             {{SOS{-1.38644004f, 0.487791806f}, SOS{-1.28536797f, 0.491276681f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}}}},
      // string 133, MIDI 74; direct partial RMS 0.214927 cent
      Preset{74,
             587.70500000000004,
             0.0013539716118675233,
             44100,
             60.050003071529595,
             2,
             {{SOS{-1.38402998f, 0.486105055f}, SOS{-1.28231752f, 0.489054441f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}}}},
      // string 134, MIDI 74; direct partial RMS 0.248215 cent
      Preset{74,
             587.99400000000003,
             0.0013550704960375276,
             44100,
             64.050002696336762,
             2,
             {{SOS{-1.2861768f, 0.424825698f},
               SOS{-0.343989342f, 0.0521134362f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 135, MIDI 75; direct partial RMS 0.261629 cent
      Preset{75,
             622.42600000000004,
             0.0014883936924585063,
             44100,
             56.049997189850302,
             2,
             {{SOS{-1.35457587f, 0.464643419f}, SOS{-1.27596891f, 0.479821116f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}}}},
      // string 136, MIDI 75; direct partial RMS 0.279361 cent
      Preset{75,
             622.73199999999997,
             0.0014895997064160145,
             44100,
             56.049996637251709,
             2,
             {{SOS{-1.3577528f, 0.467131346f}, SOS{-1.2708739f, 0.477653623f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}}}},
      // string 137, MIDI 75; direct partial RMS 0.288118 cent
      Preset{75,
             623.03700000000003,
             0.0014908021470083413,
             44100,
             56.049996216979928,
             2,
             {{SOS{-1.35758114f, 0.467141688f}, SOS{-1.26698744f, 0.475420266f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}}}},
      // string 138, MIDI 76; direct partial RMS 0.00386265 cent
      Preset{76,
             659.52099999999996,
             0.0016372643994772614,
             44100,
             24.146808107406216,
             3,
             {{SOS{-1.91826022f, 0.924577594f}, SOS{-1.93160176f, 0.968326449f},
               SOS{-1.82575846f, 0.958159924f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 139, MIDI 76; direct partial RMS 0.00223131 cent
      Preset{76,
             659.84500000000003,
             0.0016385882132663129,
             44100,
             32.049921268916975,
             3,
             {{SOS{-1.93342769f, 0.942627132f}, SOS{-1.83664703f, 0.909056425f},
               SOS{-1.76286805f, 0.925528824f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 140, MIDI 76; direct partial RMS 0.00491201 cent
      Preset{76,
             660.16899999999998,
             0.0016399124310108822,
             44100,
             24.116464758516159,
             3,
             {{SOS{-1.9182862f, 0.924626648f}, SOS{-1.93120396f, 0.968017042f},
               SOS{-1.82519495f, 0.957709849f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 141, MIDI 77; direct partial RMS 8.34524e-05 cent
      Preset{77,
             698.82799999999997,
             0.0018007898642832853,
             44100,
             48.249849873447289,
             2,
             {{SOS{-1.44530022f, 0.532936752f}, SOS{-1.30938339f, 0.532243192f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}}}},
      // string 142, MIDI 77; direct partial RMS 0.000102791 cent
      Preset{77,
             699.17100000000005,
             0.0018022425396217285,
             44100,
             48.22674104578897,
             2,
             {{SOS{-1.44504714f, 0.532761931f}, SOS{-1.30903041f, 0.532088757f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}}}},
      // string 143, MIDI 77; direct partial RMS 0.000145416 cent
      Preset{77,
             699.51499999999999,
             0.0018036998933031727,
             44100,
             38.04999180214341,
             4,
             {{SOS{-1.46186352f, 0.54294306f}, SOS{-1.2284348f, 0.417786479f},
               SOS{-1.17453015f, 0.474301785f}, SOS{-1.21694064f, 0.475296915f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 144, MIDI 78; direct partial RMS 0.161588 cent
      Preset{78,
             740.47699999999998,
             0.0019803801748292742,
             44100,
             47.523774393243798,
             2,
             {{SOS{-1.09188616f, 0.298132569f}, SOS{-1.0870961f, 0.338604301f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}}}},
      // string 145, MIDI 78; direct partial RMS 0.161525 cent
      Preset{78,
             740.84100000000001,
             0.0019819779269423863,
             44100,
             47.483135028623437,
             2,
             {{SOS{-1.09257054f, 0.298502475f}, SOS{-1.08799922f, 0.339093655f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}}}},
      // string 146, MIDI 78; direct partial RMS 0.161467 cent
      Preset{78,
             741.20500000000004,
             0.0019835761632297278,
             44100,
             47.443837174678947,
             2,
             {{SOS{-1.09317505f, 0.298829257f}, SOS{-1.08881748f, 0.339541167f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}}}},
      // string 147, MIDI 79; direct partial RMS 0.0791027 cent
      Preset{
          79,
          784.60799999999995,
          0.0021775887830562946,
          44100,
          48.547499828826787,
          1,
          {{SOS{-1.27058542f, 0.421279997f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
            SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 148, MIDI 79; direct partial RMS 0.0790934 cent
      Preset{
          79,
          784.99400000000003,
          0.0021793445621545167,
          44100,
          48.523575329641297,
          1,
          {{SOS{-1.27028108f, 0.421096712f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
            SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 149, MIDI 79; direct partial RMS 0.0790752 cent
      Preset{79,
             785.37900000000002,
             0.0021810963210037405,
             44100,
             48.499830313935945,
             1,
             {{SOS{-1.2699697f, 0.420909226f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 150, MIDI 80; direct partial RMS 4.78699e-05 cent
      Preset{80,
             831.37,
             0.0023941198237539172,
             44100,
             37.950001974274308,
             2,
             {{SOS{-1.47181594f, 0.552935421f}, SOS{-1.38261032f, 0.586579382f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}}}},
      // string 151, MIDI 80; direct partial RMS 6.07332e-05 cent
      Preset{80,
             831.77800000000002,
             0.0023960427315172735,
             44100,
             39.949997098975885,
             2,
             {{SOS{-1.35135889f, 0.468876839f}, SOS{-1.20124602f, 0.465730965f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}}}},
      // string 152, MIDI 80; direct partial RMS 0.0155488 cent
      Preset{
          80,
          832.18700000000001,
          0.0023979709317304625,
          44100,
          45.863072277814581,
          1,
          {{SOS{-1.22391295f, 0.393599957f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
            SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 153, MIDI 81; direct partial RMS 1.10432e-05 cent
      Preset{81,
             880.91800000000001,
             0.002631825353009612,
             44100,
             37.9499992637883,
             2,
             {{SOS{-1.23915434f, 0.394399941f}, SOS{-1.11627996f, 0.395525128f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}}}},
      // string 154, MIDI 81; direct partial RMS 8.93934e-05 cent
      Preset{81,
             881.351,
             0.0026339395379290798,
             44100,
             37.94999726257965,
             2,
             {{SOS{-1.23644924f, 0.392714918f}, SOS{-1.11370862f, 0.393554121f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}}}},
      // string 155, MIDI 81; direct partial RMS 0.000592458 cent
      Preset{81,
             881.78399999999999,
             0.0026360543561619763,
             44100,
             28.855336845982407,
             3,
             {{SOS{-1.67851853f, 0.715789676f}, SOS{-1.56876552f, 0.716429114f},
               SOS{-1.34250402f, 0.737495959f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 156, MIDI 82; direct partial RMS 0.000593932 cent
      Preset{82,
             933.56399999999996,
             0.0028934788772695488,
             44100,
             25.950016862324823,
             2,
             {{SOS{-1.81193781f, 0.835402727f}, SOS{-1.68513715f, 0.834208548f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}}}},
      // string 157, MIDI 82; direct partial RMS 0.000189923 cent
      Preset{82,
             934.02300000000002,
             0.0028958005795701332,
             44100,
             25.052877472737215,
             3,
             {{SOS{-1.69068253f, 0.726088643f}, SOS{-1.5674473f, 0.709040523f},
               SOS{-1.3994149f, 0.731583238f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}}}},
      // string 158, MIDI 82; direct partial RMS 0.000537694 cent
      Preset{82,
             934.48099999999999,
             0.0028981179144138163,
             44100,
             23.950012091818461,
             2,
             {{SOS{-1.84408128f, 0.864561021f}, SOS{-1.70568359f, 0.834620774f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}}}},
      // string 159, MIDI 83; direct partial RMS 0.000768784 cent
      Preset{83,
             989.35599999999999,
             0.0031807169710454416,
             44100,
             32.049998814936608,
             2,
             {{SOS{-1.41628051f, 0.51657784f}, SOS{-1.22288167f, 0.541632235f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}}}},
      // string 160, MIDI 83; direct partial RMS 0.000157482 cent
      Preset{83,
             989.84199999999998,
             0.0031832632707381259,
             44100,
             25.950000246200204,
             2,
             {{SOS{-1.60586166f, 0.654705346f}, SOS{-1.56754363f, 0.705698073f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}}}},
      // string 161, MIDI 83; direct partial RMS 0.000707413 cent
      Preset{83,
             990.32799999999997,
             0.0031858103268383163,
             44100,
             34.050002313610527,
             2,
             {{SOS{-1.20882189f, 0.381606311f}, SOS{-0.76430279f, 0.225313246f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}}}},
      // string 162, MIDI 84; direct partial RMS 0.281402 cent
      Preset{84,
             1048.48,
             0.003495987417455864,
             44100,
             31.950001267965064,
             2,
             {{SOS{-0.999297798f, 0.253464848f},
               SOS{-0.966494262f, 0.296039879f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 163, MIDI 84; direct partial RMS 0.265019 cent
      Preset{84,
             1049,
             0.0034988090267594118,
             44100,
             31.94999983245367,
             2,
             {{SOS{-0.988245428f, 0.24735406f},
               SOS{-0.969338775f, 0.295706987f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 164, MIDI 84; direct partial RMS 0.251339 cent
      Preset{84,
             1049.51,
             0.0035015771931539356,
             44100,
             31.95000036343454,
             2,
             {{SOS{-0.975654721f, 0.2403014f}, SOS{-0.973600626f, 0.296193272f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}}}},
      // string 165, MIDI 85; direct partial RMS 0.0212277 cent
      Preset{85,
             1111.1400000000001,
             0.0038419949048638536,
             44100,
             29.232737399599809,
             2,
             {{SOS{-1.09106624f, 0.301896185f},
               SOS{-0.977892458f, 0.324562669f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 166, MIDI 85; direct partial RMS 0.0204671 cent
      Preset{85,
             1111.6900000000001,
             0.0038450794309375725,
             44100,
             29.199636666280007,
             2,
             {{SOS{-1.09417939f, 0.303803355f}, SOS{-0.978356361f, 0.32524085f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}}}},
      // string 167, MIDI 85; direct partial RMS 0.019751 cent
      Preset{85,
             1112.23,
             0.0038481075901828548,
             44100,
             29.168747692132186,
             2,
             {{SOS{-1.09693968f, 0.305498838f}, SOS{-0.978756547f, 0.32585451f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}}}},
      // string 168, MIDI 86; direct partial RMS 3.02317 cent
      Preset{86,
             1177.55,
             0.0042119619561208719,
             44100,
             29.949999386057133,
             2,
             {{SOS{0.384083152f, 0.0433288142f},
               SOS{-1.14902902f, 0.357350677f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 169, MIDI 86; direct partial RMS 2.76383 cent
      Preset{86,
             1178.1199999999999,
             0.0042151127347369224,
             44100,
             29.949999810849739,
             2,
             {{SOS{0.429935843f, 0.0623645224f},
               SOS{-1.16155779f, 0.365283489f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 170, MIDI 86; direct partial RMS 2.74872 cent
      Preset{86,
             1178.7,
             0.0042183183075681406,
             44100,
             29.94999917052413,
             2,
             {{SOS{0.448067248f, 0.0643512383f},
               SOS{-1.16206348f, 0.365652114f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 171, MIDI 87; direct partial RMS 1.38002 cent
      Preset{
          87,
          1247.9200000000001,
          0.0045970393473528538,
          44100,
          29.093744880463838,
          1,
          {{SOS{-1.14893627f, 0.355150968f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
            SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 172, MIDI 87; direct partial RMS 1.38854 cent
      Preset{
          87,
          1248.53,
          0.0046003399631090302,
          44100,
          29.073644006212714,
          1,
          {{SOS{-1.14948452f, 0.355499178f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
            SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 173, MIDI 87; direct partial RMS 0.000486251 cent
      Preset{87,
             1249.1500000000001,
             0.004603693977777587,
             44100,
             15.950010446318959,
             3,
             {{SOS{-1.81487226f, 0.847692072f}, SOS{-1.56732917f, 0.77372241f},
               SOS{-1.32511139f, 0.831984341f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 174, MIDI 88; direct partial RMS 2.17319e-05 cent
      Preset{88,
             1322.5,
             0.0049951145405383321,
             44100,
             23.063326449313003,
             2,
             {{SOS{-1.26668882f, 0.420045257f},
               SOS{-0.943880856f, 0.391119719f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 175, MIDI 88; direct partial RMS 7.58147e-05 cent
      Preset{88,
             1323.1500000000001,
             0.0049985326872045224,
             44100,
             23.046738439728571,
             2,
             {{SOS{-1.266698f, 0.420055687f}, SOS{-0.94378227f, 0.391024172f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}}}},
      // string 176, MIDI 88; direct partial RMS 0.000111141 cent
      Preset{88,
             1323.8,
             0.0050019499015450716,
             44100,
             23.030372023025556,
             2,
             {{SOS{-1.26669502f, 0.420058727f},
               SOS{-0.943671107f, 0.390927941f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 177, MIDI 89; direct partial RMS 0.159526 cent
      Preset{89,
             1401.53,
             0.0054035704786987411,
             44100,
             22.04999991439503,
             2,
             {{SOS{-1.1086365f, 0.320119411f}, SOS{-0.814210057f, 0.284173995f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}}}},
      // string 178, MIDI 89; direct partial RMS 0.165596 cent
      Preset{89,
             1402.22,
             0.0054070707146882764,
             44100,
             22.049999373269927,
             2,
             {{SOS{-1.10473061f, 0.317801952f},
               SOS{-0.812058091f, 0.281852752f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 179, MIDI 89; direct partial RMS 0.194178 cent
      Preset{89,
             1402.9100000000001,
             0.0054105697677488025,
             44100,
             22.05000016180983,
             2,
             {{SOS{-1.10295749f, 0.316933215f},
               SOS{-0.808148801f, 0.279221237f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 180, MIDI 90; direct partial RMS 9.60997e-05 cent
      Preset{90,
             1485.29,
             0.0058195612069360638,
             44100,
             20.24240418164247,
             2,
             {{SOS{-1.16862035f, 0.360077918f},
               SOS{-0.817855597f, 0.310178459f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 181, MIDI 90; direct partial RMS 2.34529e-05 cent
      Preset{90,
             1486.02,
             0.0058231055893683322,
             44100,
             20.23369138965937,
             2,
             {{SOS{-1.16814804f, 0.35981071f}, SOS{-0.817106307f, 0.309898198f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}}}},
      // string 182, MIDI 90; direct partial RMS 4.26414e-05 cent
      Preset{90,
             1486.75,
             0.0058266485350518971,
             44100,
             20.225020654207832,
             2,
             {{SOS{-1.16767251f, 0.359541744f},
               SOS{-0.816351652f, 0.309615582f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 183, MIDI 91; direct partial RMS 0.590579 cent
      Preset{
          91,
          1574.0599999999999,
          0.0062398203557380589,
          44100,
          22.534597652165711,
          1,
          {{SOS{-1.03914273f, 0.294344574f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
            SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 184, MIDI 91; direct partial RMS 0.000125241 cent
      Preset{91,
             1574.8299999999999,
             0.006243369216496159,
             44100,
             18.050002277743431,
             2,
             {{SOS{-1.28692412f, 0.43690753f}, SOS{-0.944672942f, 0.422444552f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}}}},
      // string 185, MIDI 91; direct partial RMS 0.591605 cent
      Preset{
          91,
          1575.5999999999999,
          0.0062469163912102438,
          44100,
          22.512142462096577,
          1,
          {{SOS{-1.03848088f, 0.294029504f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
            SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 186, MIDI 92; direct partial RMS 0.000164021 cent
      Preset{92,
             1668.1199999999999,
             0.0066607089561924035,
             88200,
             36.128143257534461,
             2,
             {{SOS{-1.31384766f, 0.499646187f}, SOS{-1.46118248f, 0.541389525f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}}}},
      // string 187, MIDI 92; direct partial RMS 0.0029973 cent
      Preset{92,
             1668.9400000000001,
             0.0066642651179016771,
             88200,
             18.050073618784772,
             4,
             {{SOS{-1.92558289f, 0.936454177f}, SOS{-1.90293932f, 0.962630987f},
               SOS{-1.73225188f, 0.966341853f}, SOS{-1.49239087f, 0.916346431f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 188, MIDI 92; direct partial RMS 0.00014255 cent
      Preset{92,
             1669.76,
             0.0066678193086544425,
             88200,
             11.949999564697556,
             4,
             {{SOS{-1.92404354f, 0.931724072f}, SOS{-1.90811646f, 0.963221252f},
               SOS{-1.69624138f, 0.951371133f}, SOS{-1.76242805f, 0.901863456f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 189, MIDI 93; direct partial RMS 2.21207e-05 cent
      Preset{93,
             1767.8099999999999,
             0.0070785229884286109,
             88200,
             34.049999824083464,
             2,
             {{SOS{-1.39061058f, 0.489491194f}, SOS{-1.25199795f, 0.447369486f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}}}},
      // string 190, MIDI 93; direct partial RMS 8.6831e-06 cent
      Preset{93,
             1768.6800000000001,
             0.0070820399260056689,
             88200,
             34.050000419490786,
             2,
             {{SOS{-1.38737321f, 0.487151057f}, SOS{-1.25071621f, 0.445867151f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}}}},
      // string 191, MIDI 93; direct partial RMS 0.000103814 cent
      Preset{93,
             1769.55,
             0.0070855546206974353,
             88200,
             34.04999548008832,
             2,
             {{SOS{-1.38404977f, 0.484751672f}, SOS{-1.24949622f, 0.444392771f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}}}},
      // string 192, MIDI 94; direct partial RMS 4.75019e-05 cent
      Preset{94,
             1874.5999999999999,
             0.0074934700986100814,
             88200,
             32.050001959972057,
             2,
             {{SOS{-1.41132057f, 0.508179128f}, SOS{-1.13332355f, 0.38280952f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}}}},
      // string 193, MIDI 94; direct partial RMS 0.000147017 cent
      Preset{94,
             1875.52,
             0.0074968984776709983,
             88200,
             32.050004530495357,
             2,
             {{SOS{-1.40961003f, 0.506987572f}, SOS{-1.13035929f, 0.380449742f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}}}},
      // string 194, MIDI 94; direct partial RMS 7.91919e-06 cent
      Preset{94,
             1876.45,
             0.007500361588523464,
             88200,
             32.050000684932343,
             2,
             {{SOS{-1.40787315f, 0.505779803f}, SOS{-1.12734687f, 0.378048122f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}}}},
      // string 195, MIDI 95; direct partial RMS 0.000189152 cent
      Preset{95,
             1987.8399999999999,
             0.0078968439886875907,
             88200,
             34.050000773820308,
             2,
             {{SOS{-1.40740156f, 0.507375896f}, SOS{-1.05532122f, 0.78514415f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}}}},
      // string 196, MIDI 95; direct partial RMS 9.55196e-05 cent
      Preset{95,
             1988.8199999999999,
             0.0079001722277890086,
             88200,
             25.949996712467019,
             2,
             {{SOS{-1.68465018f, 0.723492444f}, SOS{-1.51265633f, 0.703982711f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}}}},
      // string 197, MIDI 95; direct partial RMS 0.000568732 cent
      Preset{95,
             1989.8,
             0.0079034976998747491,
             88200,
             25.94998708296416,
             2,
             {{SOS{-1.68620276f, 0.724880397f}, SOS{-1.51368713f, 0.706086874f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}}}},
      // string 198, MIDI 96; direct partial RMS 0.000310183 cent
      Preset{96,
             2107.9200000000001,
             0.0082842751064562091,
             88200,
             26.049992003862975,
             2,
             {{SOS{-1.52102256f, 0.590794623f}, SOS{-1.27824211f, 0.518641233f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}}}},
      // string 199, MIDI 96; direct partial RMS 0.00108302 cent
      Preset{96,
             2108.96,
             0.0082874532511910421,
             88200,
             16.475560521303869,
             3,
             {{SOS{-1.79486322f, 0.816107869f}, SOS{-1.72609103f, 0.831490874f},
               SOS{-1.48112917f, 0.846359909f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 200, MIDI 96; direct partial RMS 0.000597685 cent
      Preset{96,
             2109.9899999999998,
             0.0082905978744504689,
             88200,
             18.050012260345365,
             3,
             {{SOS{-1.70915866f, 0.740216672f}, SOS{-1.61312962f, 0.73246479f},
               SOS{-1.38371623f, 0.743409336f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 201, MIDI 97; direct partial RMS 3.73986e-05 cent
      Preset{97,
             2235.25,
             0.0086513811057734348,
             88200,
             20.049999422835274,
             3,
             {{SOS{-1.33764315f, 0.452865243f}, SOS{-1.23265481f, 0.409275472f},
               SOS{-1.18375885f, 0.488856226f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 202, MIDI 97; direct partial RMS 0.000591658 cent
      Preset{97,
             2236.3499999999999,
             0.0086543623851430551,
             88200,
             16.75332621829341,
             3,
             {{SOS{-1.70733809f, 0.73970741f}, SOS{-1.60602009f, 0.736931682f},
               SOS{-1.34576941f, 0.740814209f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 203, MIDI 97; direct partial RMS 0.00158313 cent
      Preset{97,
             2237.4499999999998,
             0.0086573404646191551,
             88200,
             16.049968539097616,
             3,
             {{SOS{-1.85847199f, 0.882319093f}, SOS{-1.67582357f, 0.81665957f},
               SOS{-1.45060456f, 0.888110697f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 204, MIDI 98; direct partial RMS 0.000204055 cent
      Preset{98,
             2370.2800000000002,
             0.0089938977410334853,
             88200,
             24.04999510940327,
             2,
             {{SOS{-1.44367349f, 0.536668599f}, SOS{-1.05527306f, 0.418754786f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}}}},
      // string 205, MIDI 98; direct partial RMS 1.51982e-05 cent
      Preset{98,
             2371.4400000000001,
             0.0089966395612102908,
             88200,
             24.050000944036569,
             2,
             {{SOS{-1.44260156f, 0.535899282f}, SOS{-1.05257785f, 0.417181641f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}}}},
      // string 206, MIDI 98; direct partial RMS 8.98018e-05 cent
      Preset{98,
             2372.6100000000001,
             0.0089994016309385607,
             88200,
             24.049998097681179,
             2,
             {{SOS{-1.44152284f, 0.535125732f}, SOS{-1.04984868f, 0.415592074f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}}}},
      // string 207, MIDI 99; direct partial RMS 0.000468924 cent
      Preset{
          99,
          2513.46,
          0.0093076518307531313,
          88200,
          27.303931738795988,
          1,
          {{SOS{-1.27929449f, 0.424628615f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
            SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 208, MIDI 99; direct partial RMS 0.000451854 cent
      Preset{
          99,
          2514.6999999999998,
          0.0093101571409708352,
          88200,
          27.286592230910919,
          1,
          {{SOS{-1.27938032f, 0.424696863f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
            SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 209, MIDI 99; direct partial RMS 0.00045464 cent
      Preset{99,
             2515.9299999999998,
             0.0093126387443926931,
             88200,
             27.269534540694522,
             1,
             {{SOS{-1.2794528f, 0.424756318f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 210, MIDI 100; direct partial RMS 0.000333352 cent
      Preset{100,
             2665.29,
             0.009588770619505492,
             88200,
             16.049993650337324,
             3,
             {{SOS{-1.41189635f, 0.504743397f}, SOS{-1.32147706f, 0.527017891f},
               SOS{-0.918336689f, 0.459195286f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 211, MIDI 100; direct partial RMS 0.000480014 cent
      Preset{100,
             2666.5999999999999,
             0.0095909775822555152,
             88200,
             14.05000921304956,
             3,
             {{SOS{-1.67238939f, 0.714045942f}, SOS{-1.54770339f, 0.723699868f},
               SOS{-1.16496634f, 0.742832482f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 212, MIDI 100; direct partial RMS 0.000254781 cent
      Preset{100,
             2667.9099999999999,
             0.0095931809174093739,
             88200,
             14.050004699752449,
             3,
             {{SOS{-1.67080188f, 0.712590873f}, SOS{-1.54771423f, 0.723582923f},
               SOS{-1.16424525f, 0.742861569f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 213, MIDI 101; direct partial RMS 0.000163252 cent
      Preset{101,
             2826.29,
             0.009833681755363196,
             88200,
             18.061293279039187,
             2,
             {{SOS{-1.56582272f, 0.637099981f}, SOS{-1.23706043f, 0.600258529f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}}}},
      // string 214, MIDI 101; direct partial RMS 0.000193106 cent
      Preset{101,
             2827.6799999999998,
             0.0098355725817048413,
             88200,
             15.95000356431677,
             3,
             {{SOS{-1.73297954f, 0.780714512f}, SOS{-1.39156926f, 0.73246944f},
               SOS{-0.732175231f, 0.744814754f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 215, MIDI 101; direct partial RMS 0.000165009 cent
      Preset{101,
             2829.0700000000002,
             0.0098374597092516624,
             88200,
             18.062247537041763,
             2,
             {{SOS{-1.56300688f, 0.634850442f}, SOS{-1.2336477f, 0.597753167f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}}}},
      // string 216, MIDI 102; direct partial RMS 0.00185303 cent
      Preset{102,
             2997.02,
             0.010039223629270608,
             88200,
             8.0500294257860858,
             3,
             {{SOS{-1.92696917f, 0.968685925f}, SOS{-1.66416311f, 0.856092572f},
               SOS{-1.33945107f, 0.817759395f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 217, MIDI 102; direct partial RMS 0.000118479 cent
      Preset{102,
             2998.4899999999998,
             0.010040768085133125,
             88200,
             10.427679969706709,
             3,
             {{SOS{-1.67182195f, 0.716325402f}, SOS{-1.5028441f, 0.689757407f},
               SOS{-1.23464954f, 0.721291363f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 218, MIDI 102; direct partial RMS 0.000256398 cent
      Preset{102,
             2999.9699999999998,
             0.010042319309200817,
             88200,
             14.064003043708219,
             3,
             {{SOS{-1.65408003f, 0.711521387f}, SOS{-1.32032216f, 0.624098301f},
               SOS{-0.867125869f, 0.724614978f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 219, MIDI 103; direct partial RMS 0.0024513 cent
      Preset{103,
             3178.0599999999999,
             0.010202684190796315,
             88200,
             16.049997564486684,
             2,
             {{SOS{-1.45802009f, 0.556949019f}, SOS{-1.10012186f, 0.507797778f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}}}},
      // string 220, MIDI 103; direct partial RMS 0.00340945 cent
      Preset{103,
             3179.6199999999999,
             0.010203867825481026,
             88200,
             16.049994461635109,
             2,
             {{SOS{-1.45615852f, 0.55553478f}, SOS{-1.09845221f, 0.506341875f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}}}},
      // string 221, MIDI 103; direct partial RMS 0.000600977 cent
      Preset{103,
             3181.1799999999998,
             0.010205047779361105,
             88200,
             7.9500108120458037,
             3,
             {{SOS{-1.85704398f, 0.889898777f}, SOS{-1.75982821f, 0.966056645f},
               SOS{-1.28446043f, 0.854913056f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 222, MIDI 104; direct partial RMS 6.92097e-05 cent
      Preset{104,
             3370.04,
             0.010321893182107819,
             88200,
             15.949998830535687,
             2,
             {{SOS{-1.30273044f, 0.447959423f},
               SOS{-0.935120285f, 0.399429828f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 223, MIDI 104; direct partial RMS 6.68987e-05 cent
      Preset{104,
             3371.6900000000001,
             0.010322696893368198,
             88200,
             15.950000932357083,
             2,
             {{SOS{-1.30059123f, 0.44649905f}, SOS{-0.933342814f, 0.397908658f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}}}},
      // string 224, MIDI 104; direct partial RMS 0.000775247 cent
      Preset{104,
             3373.3499999999999,
             0.010323501853965826,
             88200,
             8.6223366353627995,
             3,
             {{SOS{-1.76920259f, 0.808407784f}, SOS{-1.62415028f, 0.857822061f},
               SOS{-1.04090142f, 0.779321969f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 225, MIDI 105; direct partial RMS 0.000318697 cent
      Preset{105,
             3573.6100000000001,
             0.010395243004170114,
             88200,
             6.04999566006272,
             4,
             {{SOS{-1.39712834f, 0.506596565f}, SOS{-1.32416666f, 0.489707291f},
               SOS{-1.13913548f, 0.55498606f}, SOS{-0.795843482f, 0.608181298f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 226, MIDI 105; direct partial RMS 1.87928e-05 cent
      Preset{105,
             3575.3699999999999,
             0.010395661433177543,
             88200,
             14.050000038758405,
             2,
             {{SOS{-1.46755624f, 0.572719991f},
               SOS{-0.991771877f, 0.517681658f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 227, MIDI 105; direct partial RMS 0.000121185 cent
      Preset{105,
             3577.1199999999999,
             0.010396074005476357,
             88200,
             10.049998643098087,
             3,
             {{SOS{-1.43175745f, 0.528265715f}, SOS{-1.24529123f, 0.518923283f},
               SOS{-0.864156842f, 0.556155503f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 228, MIDI 106; direct partial RMS 8.87539e-05 cent
      Preset{106,
             3789.48,
             0.01042174655198037,
             88200,
             13.9500005768713,
             2,
             {{SOS{-1.21271563f, 0.38847962f}, SOS{-0.901799798f, 0.399888486f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}}}},
      // string 229, MIDI 106; direct partial RMS 0.000138488 cent
      Preset{106,
             3791.3400000000001,
             0.010421769215013135,
             88200,
             14.050001243937039,
             2,
             {{SOS{-1.29614079f, 0.449194372f}, SOS{-0.784460485f, 0.40342617f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}}}},
      // string 230, MIDI 106; direct partial RMS 0.000114526 cent
      Preset{106,
             3793.21,
             0.010421788656279837,
             88200,
             13.950001107162485,
             2,
             {{SOS{-1.21087635f, 0.387411445f},
               SOS{-0.897981465f, 0.398319453f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 231, MIDI 107; direct partial RMS 4.79444e-05 cent
      Preset{107,
             4018.3899999999999,
             0.010401042854577084,
             88200,
             8.0500002911816608,
             3,
             {{SOS{-1.50740194f, 0.589755476f}, SOS{-1.27144229f, 0.590140879f},
               SOS{-0.78878653f, 0.614752471f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 232, MIDI 107; direct partial RMS 0.000164398 cent
      Preset{107,
             4020.3699999999999,
             0.010400669406307886,
             88200,
             8.0500018924282237,
             3,
             {{SOS{-1.50677586f, 0.589403689f}, SOS{-1.26945913f, 0.588653088f},
               SOS{-0.788016021f, 0.614056766f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 233, MIDI 107; direct partial RMS 6.97607e-05 cent
      Preset{107,
             4022.3400000000001,
             0.010400294740517305,
             88200,
             6.0500005377427026,
             4,
             {{SOS{-1.36691868f, 0.507670105f}, SOS{-1.39220786f, 0.537437439f},
               SOS{-1.04930043f, 0.686420321f}, SOS{-0.37702921f, 0.730419397f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 234, MIDI 108; direct partial RMS 0.000131338 cent
      Preset{108,
             4261.1300000000001,
             0.010333412023005079,
             88200,
             12.050000698935637,
             2,
             {{SOS{-1.27907968f, 0.443780214f},
               SOS{-0.732009947f, 0.412854165f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 235, MIDI 108; direct partial RMS 0.000112235 cent
      Preset{108,
             4263.2299999999996,
             0.010332647299167762,
             88200,
             12.050000820600349,
             2,
             {{SOS{-1.27830303f, 0.443303198f},
               SOS{-0.730007112f, 0.412438154f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}}}},
      // string 236, MIDI 108; direct partial RMS 6.71633e-05 cent
      Preset{108,
             4265.3199999999997,
             0.010331883366577739,
             88200,
             12.049999616174771,
             2,
             {{SOS{-1.28984487f, 0.4519822f}, SOS{-0.727853715f, 0.422457099f},
               SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f}, SOS{0.0f, 0.0f},
               SOS{0.0f, 0.0f}}}},
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
// discussion, review, implementation guidance, or non-critical engineering
// suggestions.
//
// However, every line included in this file has been reviewed, understood,
// and accepted by its author. Every implementation is expected to be
// explainable, reproducible, open to inspection, and subject to criticism and
// revision.
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
  inline static constexpr std::size_t kSectionCount = 6;
  struct LossShelf {
    float b0;
    float b1;
    float a1;
  };
  struct LossPreset {
    int midi;
    double referenceF1;
    double phaseDelaySamples;
    double groupDelaySamples;
    std::array<LossShelf, kSectionCount> sections;
  };

public:
  LossPreset lossPreset;

private:
  std::array<float, kSectionCount> state_{};

public:
  explicit LossFilter(int midi) noexcept
      : lossPreset(kPresets[static_cast<std::size_t>(
            std::clamp(midi, kMidiMin, kMidiMax) - kMidiMin)]) {}
  [[nodiscard]] inline double get_group_delay() const noexcept {
    return lossPreset.groupDelaySamples;
  }
  [[nodiscard]] inline double get_phase_delay(double sampleRate,
                                              double frequency) const {
    const double omega =
        2.0 * std::numbers::pi_v<double> * frequency / sampleRate;
    const std::complex<double> z1 = std::polar(1.0, -omega);
    std::complex<double> response{1.0, 0.0};
    for (const auto &c : lossPreset.sections)
      response *=
          (double(c.b0) + double(c.b1) * z1) / (1.0 + double(c.a1) * z1);
    return -std::arg(response) / omega;
  }
  inline void process(float &x) noexcept {
    for (std::size_t i = 0; i < kSectionCount; ++i) {
      const auto &c = lossPreset.sections[i];
      const float y = c.b0 * x + state_[i];
      state_[i] = c.b1 * x - c.a1 * y;
      x = y;
    }
  }
  inline void system_reset() noexcept { state_.fill(0.0f); }
  [[nodiscard]] inline float state_energy() const noexcept {
    float energy = 0.0f;
    for (float value : state_)
      energy += value * value;
    return energy / static_cast<float>(kSectionCount);
  }

private:
  inline static constexpr std::array<LossPreset, 88> kPresets = {{
      // MIDI 21; PT8 adaptive effective loss; 4/6 active; holdout p95 27.409%
      LossPreset{21,
                 27.5,
                 0.85742032977504412,
                 0.85741661439239292,
                 {{
                     LossShelf{0.599726796f, 0.555388093f, 0.156409457f},
                     LossShelf{0.741163671f, 0.295103461f, 0.037428502f},
                     LossShelf{0.969172835f, 0.0165651198f, -0.0131573398f},
                     LossShelf{0.830871403f, -0.109907508f, -0.278228104f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                 }}},
      // MIDI 22; PT8 adaptive effective loss; 4/6 active; holdout p95 20.629%
      LossPreset{22,
                 29.13523509488062,
                 0.75066105224459401,
                 0.7506619571893699,
                 {{
                     LossShelf{0.580761135f, 0.516048372f, 0.0978947207f},
                     LossShelf{1.09314692f, -0.00136377837f, 0.0928633288f},
                     LossShelf{1.02259135f, 0.00562875113f, 0.029237438f},
                     LossShelf{0.51087898f, 0.509291947f, 0.0211803075f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                 }}},
      // MIDI 23; PT8 adaptive effective loss; 4/6 active; holdout p95 136.476%
      LossPreset{23,
                 30.867706328507751,
                 1.4800343380408656,
                 1.4800355018819626,
                 {{
                     LossShelf{0.594773233f, 0.514169991f, 0.109796979f},
                     LossShelf{0.570261121f, 0.531858742f, 0.102968365f},
                     LossShelf{0.610026777f, 0.478643805f, 0.0895087495f},
                     LossShelf{0.600029767f, 0.483035594f, 0.0838992447f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                 }}},
      // MIDI 24; PT8 adaptive effective loss; 2/6 active; holdout p95 27.319%
      LossPreset{24,
                 32.703195662574828,
                 0.67703153472507449,
                 0.67703038097734469,
                 {{
                     LossShelf{0.815292895f, 0.220123217f, 0.0368896984f},
                     LossShelf{0.526160359f, 0.418647438f, -0.0538475476f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                 }}},
      // MIDI 25; PT8 adaptive effective loss; 6/6 active; holdout p95 17.863%
      LossPreset{25,
                 34.64782887210901,
                 0.69666080209240655,
                 0.69665830332052181,
                 {{
                     LossShelf{1.02035236f, 0.0121773425f, 0.0329578482f},
                     LossShelf{0.994840384f, 0.0159894768f, 0.0112490673f},
                     LossShelf{0.997597337f, 0.000294383994f, -0.00169446308f},
                     LossShelf{1.0067879f, -0.0155846532f, -0.00838571787f},
                     LossShelf{0.959713995f, 0.00777110737f, -0.0321136862f},
                     LossShelf{0.425314099f, 0.424020171f, -0.150313497f},
                 }}},
      // MIDI 26; PT8 adaptive effective loss; 6/6 active; holdout p95 20.068%
      LossPreset{26,
                 36.70809598967594,
                 1.3911093481205912,
                 1.3911121453998017,
                 {{
                     LossShelf{0.630939364f, 0.607252359f, 0.238663197f},
                     LossShelf{0.644183218f, 0.523969352f, 0.16859743f},
                     LossShelf{0.558826864f, 0.512817502f, 0.0720524266f},
                     LossShelf{0.531618953f, 0.512325168f, 0.0443416163f},
                     LossShelf{1.05408549f, -0.0428682007f, 0.0116023384f},
                     LossShelf{1.01614916f, -0.0253095068f, -0.0087830117f},
                 }}},
      // MIDI 27; PT8 adaptive effective loss; 1/6 active; holdout p95 20.574%
      LossPreset{27,
                 38.890872965260115,
                 0.47121152885188244,
                 0.47120641378627143,
                 {{
                     LossShelf{0.614674032f, 0.198462442f, -0.185099587f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                 }}},
      // MIDI 28; PT8 adaptive effective loss; 3/6 active; holdout p95 17.353%
      LossPreset{28,
                 41.203444614108747,
                 0.79416646836438987,
                 0.79416265961839339,
                 {{
                     LossShelf{0.827214897f, 0.655734599f, 0.483892411f},
                     LossShelf{0.614918172f, 0.613052309f, 0.228751227f},
                     LossShelf{0.700333357f, 0.118912816f, -0.180232897f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                 }}},
      // MIDI 29; PT8 adaptive effective loss; 3/6 active; holdout p95 46.672%
      LossPreset{29,
                 43.653528929125486,
                 0.6507102218399754,
                 0.64928708002509194,
                 {{
                     LossShelf{28.9138603f, -27.9596863f, -0.0454978645f},
                     LossShelf{0.920290768f, 0.0338419639f, -0.0455388874f},
                     LossShelf{0.0165325813f, 0.0164325684f, -0.967023492f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                 }}},
      // MIDI 30; PT8 adaptive effective loss; 3/6 active; holdout p95 33.777%
      LossPreset{30,
                 46.2493028389543,
                 0.55393467480967684,
                 0.5525121155944076,
                 {{
                     LossShelf{0.52350229f, 0.518573523f, 0.0424013585f},
                     LossShelf{26.332962f, -25.3722973f, -0.0390352197f},
                     LossShelf{0.0352337323f, 0.00120436365f, -0.963550508f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                 }}},
      // MIDI 31; PT8 adaptive effective loss; 1/6 active; holdout p95 15.001%
      LossPreset{31,
                 48.999429497718666,
                 0.31363652975626988,
                 0.31363132726022519,
                 {{
                     LossShelf{0.726950884f, 0.140048683f, -0.13202472f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                 }}},
      // MIDI 32; PT8 adaptive effective loss; 2/6 active; holdout p95 37.535%
      LossPreset{32,
                 51.913087197493141,
                 0.50721345679074092,
                 0.50721219122174621,
                 {{
                     LossShelf{0.604217052f, 0.600735188f, 0.205594704f},
                     LossShelf{0.832158327f, 0.101522312f, -0.0658215657f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                 }}},
      // MIDI 33; PT8 adaptive effective loss; 2/6 active; holdout p95 58.912%
      LossPreset{33,
                 55,
                 0.41685862762190456,
                 0.41685618409928427,
                 {{
                     LossShelf{0.657960117f, 0.634813428f, 0.293416768f},
                     LossShelf{0.86499089f, 0.0160933416f, -0.118477352f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                 }}},
      // MIDI 34; PT8 adaptive effective loss; 2/6 active; holdout p95 49.305%
      LossPreset{34,
                 58.270470189761241,
                 0.32782032425130253,
                 0.32781867185975511,
                 {{
                     LossShelf{0.739006102f, 0.655698419f, 0.395332456f},
                     LossShelf{0.868263781f, 0.0627665743f, -0.0685505196f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                 }}},
      // MIDI 35; PT8 adaptive effective loss; 1/6 active; holdout p95 48.899%
      LossPreset{35,
                 61.735412657015502,
                 0.19673697337919718,
                 0.19673383674281522,
                 {{
                     LossShelf{0.807371855f, 0.1658521f, -0.0256482493f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                 }}},
      // MIDI 36; PT8 adaptive effective loss; 6/6 active; holdout p95 34.266%
      LossPreset{36,
                 65.406391325149656,
                 0.5113473044118928,
                 0.51135271387241532,
                 {{
                     LossShelf{0.788431108f, 0.690659821f, 0.479401529f},
                     LossShelf{1.05299413f, 0.310631841f, 0.363912314f},
                     LossShelf{0.811230659f, 0.516055286f, 0.327564687f},
                     LossShelf{0.658967912f, 0.625217378f, 0.284455001f},
                     LossShelf{0.999790013f, 0.0f, 0.0f},
                     LossShelf{0.999790013f, 0.0f, 0.0f},
                 }}},
      // MIDI 37; PT8 adaptive effective loss; 2/6 active; holdout p95 31.173%
      LossPreset{37,
                 69.295657744218019,
                 0.28767561951245485,
                 0.28767564787498062,
                 {{
                     LossShelf{0.736167431f, 0.647123337f, 0.384161472f},
                     LossShelf{0.902309716f, 0.0941902474f, -0.00287279394f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                 }}},
      // MIDI 38; PT8 adaptive effective loss; 4/6 active; holdout p95 79.895%
      LossPreset{38,
                 73.416191979351879,
                 0.32864823991805908,
                 0.32864931637905326,
                 {{
                     LossShelf{0.944784701f, 0.92770493f, 0.873017609f},
                     LossShelf{0.696295857f, 0.518881202f, 0.215519682f},
                     LossShelf{0.95420301f, 0.045854114f, 0.000339105609f},
                     LossShelf{0.995622277f, 0.00233388646f, -0.00176244543f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                 }}},
      // MIDI 39; PT8 adaptive effective loss; 4/6 active; holdout p95 33.755%
      LossPreset{39,
                 77.781745930520231,
                 0.30741891154034562,
                 0.30741852714937007,
                 {{
                     LossShelf{0.999711633f, 0.993693054f, 0.993905902f},
                     LossShelf{0.887165189f, 0.66502142f, 0.5525769f},
                     LossShelf{0.801080048f, 0.518886209f, 0.320298195f},
                     LossShelf{0.918724537f, 0.0417532027f, -0.0392807312f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                 }}},
      // MIDI 40; PT8 adaptive effective loss; 5/6 active; holdout p95 74.539%
      LossPreset{40,
                 82.406889228217494,
                 0.24560145343137246,
                 0.24560209109043304,
                 {{
                     LossShelf{0.853651464f, 0.809978247f, 0.66399157f},
                     LossShelf{0.826794684f, 0.269713521f, 0.0967467427f},
                     LossShelf{0.999782562f, -1.48979211e-08f, -0.0f},
                     LossShelf{0.999782503f, -0.0f, -0.0f},
                     LossShelf{0.999782503f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                 }}},
      // MIDI 41; PT8 adaptive effective loss; 1/6 active; holdout p95 34.027%
      LossPreset{41,
                 87.307057858250971,
                 0.11010766725869625,
                 0.11010460952830284,
                 {{
                     LossShelf{0.885708749f, 0.14449279f, 0.0310847536f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                 }}},
      // MIDI 42; PT8 adaptive effective loss; 2/6 active; holdout p95 33.716%
      LossPreset{42,
                 92.4986056779086,
                 0.11577367222702861,
                 0.11577102247131661,
                 {{
                     LossShelf{0.993454754f, 0.990474343f, 0.984836698f},
                     LossShelf{0.880521715f, 0.1755061f, 0.0565108992f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                 }}},
      // MIDI 43; PT8 adaptive effective loss; 4/6 active; holdout p95 30.087%
      LossPreset{43,
                 97.998858995437331,
                 0.18333060700377182,
                 0.18333352737874128,
                 {{
                     LossShelf{0.766832411f, 0.45989877f, 0.22698018f},
                     LossShelf{1.0416559f, -0.011029019f, 0.0308359936f},
                     LossShelf{0.978306234f, 0.0354222134f, 0.013934169f},
                     LossShelf{0.986826181f, 0.023579618f, 0.0106108701f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                 }}},
      // MIDI 44; PT8 adaptive effective loss; 6/6 active; holdout p95 25.795%
      LossPreset{44,
                 103.82617439498628,
                 0.040036636502009469,
                 0.039954648942691171,
                 {{
                     LossShelf{0.949495971f, 0.937892497f, 0.887607276f},
                     LossShelf{1.00797212f, 0.490740716f, 0.498858988f},
                     LossShelf{1.0008918f, -0.000501996838f, 0.000487383921f},
                     LossShelf{1.0008918f, -0.000502061681f, 0.000487225421f},
                     LossShelf{1.00089121f, -0.000501829374f, 0.000486933684f},
                     LossShelf{0.993668497f, -0.706793427f, -0.713096976f},
                 }}},
      // MIDI 45; PT8 adaptive effective loss; 4/6 active; holdout p95 37.112%
      LossPreset{45,
                 110,
                 0.029642786890857257,
                 0.0295971130163044,
                 {{
                     LossShelf{0.980723321f, 0.876643062f, 0.857723534f},
                     LossShelf{1.02486253f, 0.190329045f, 0.215408474f},
                     LossShelf{0.98601526f, -0.0415588766f, -0.0553750023f},
                     LossShelf{0.989385605f, -0.578181088f, -0.58872211f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                 }}},
      // MIDI 46; PT8 adaptive effective loss; 6/6 active; holdout p95 17.106%
      LossPreset{46,
                 116.54094037952248,
                 0.16975279795465156,
                 0.16970649964419832,
                 {{
                     LossShelf{0.993798852f, 0.98878026f, 0.982801795f},
                     LossShelf{0.940717161f, 0.82147032f, 0.762343526f},
                     LossShelf{0.863893092f, 0.634422541f, 0.498448342f},
                     LossShelf{0.786959469f, 0.469338268f, 0.256408989f},
                     LossShelf{1.20874548f, -0.0868035853f, 0.122041255f},
                     LossShelf{0.970886528f, -0.478615552f, -0.507685423f},
                 }}},
      // MIDI 47; PT8 adaptive effective loss; 2/6 active; holdout p95 35.839%
      LossPreset{47,
                 123.47082531403103,
                 0.035009066660717243,
                 0.034971327764171542,
                 {{
                     LossShelf{0.97442615f, 0.96230185f, 0.937287509f},
                     LossShelf{0.989805937f, -0.539001942f, -0.549065769f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                 }}},
      // MIDI 48; PT8 adaptive effective loss; 5/6 active; holdout p95 50.247%
      LossPreset{48,
                 130.81278265029931,
                 0.13495712781856303,
                 0.13424177311227903,
                 {{
                     LossShelf{1.00784063f, 0.837106407f, 0.845085025f},
                     LossShelf{0.95027703f, 0.562547743f, 0.512937903f},
                     LossShelf{0.773342729f, 0.270734906f, 0.0441557169f},
                     LossShelf{1.12592971f, -0.115390502f, 0.0106148152f},
                     LossShelf{0.998361766f, -0.890331805f, -0.891961932f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                 }}},
      // MIDI 49; PT8 adaptive effective loss; 3/6 active; holdout p95 19.009%
      LossPreset{49,
                 138.59131548843604,
                 0.058746349654687578,
                 0.054686346533082748,
                 {{
                     LossShelf{0.980364025f, 0.187261418f, 0.167761028f},
                     LossShelf{0.9718225f, 0.0104925521f, -0.017570883f},
                     LossShelf{0.999158263f, -0.953364372f, -0.954200804f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                 }}},
      // MIDI 50; PT8 adaptive effective loss; 2/6 active; holdout p95 22.326%
      LossPreset{50,
                 146.83238395870379,
                 -0.011214580178518922,
                 0.047779390590984162,
                 {{
                     LossShelf{0.990663767f, -0.60727644f, -0.615974009f},
                     LossShelf{1.00122774f, -0.996284783f, -0.995048761f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                 }}},
      // MIDI 51; PT8 adaptive effective loss; 1/6 active; holdout p95 23.060%
      LossPreset{51,
                 155.56349186104046,
                 0.017718800381101373,
                 0.017659713601635239,
                 {{
                     LossShelf{0.992038071f, -0.571211696f, -0.578966081f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                 }}},
      // MIDI 52; PT8 adaptive effective loss; 1/6 active; holdout p95 50.597%
      LossPreset{52,
                 164.81377845643496,
                 0.013269125713558446,
                 0.013145549636863417,
                 {{
                     LossShelf{0.995723963f, -0.704598844f, -0.708759844f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                 }}},
      // MIDI 53; PT8 adaptive effective loss; 1/6 active; holdout p95 9.991%
      LossPreset{53,
                 174.61411571650194,
                 0.012641015213949393,
                 0.012520407547119734,
                 {{
                     LossShelf{0.99571532f, -0.693189979f, -0.697340846f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                 }}},
      // MIDI 54; PT8 adaptive effective loss; 6/6 active; holdout p95 11.858%
      LossPreset{54,
                 184.9972113558172,
                 0.37868960276100977,
                 0.378660698344636,
                 {{
                     LossShelf{0.825021029f, 0.389514536f, 0.214622185f},
                     LossShelf{0.753541887f, 0.300063729f, 0.0536808074f},
                     LossShelf{0.998856664f, 0.000536368578f, -0.000535668631f},
                     LossShelf{0.683845937f, 0.223737091f, -0.0923522189f},
                     LossShelf{1.43834972f, -0.606888533f, -0.168479443f},
                     LossShelf{0.918005168f, -0.459794492f, -0.54175663f},
                 }}},
      // MIDI 55; PT8 adaptive effective loss; 3/6 active; holdout p95 26.036%
      LossPreset{55,
                 195.99771799087463,
                 0.12701654700847309,
                 0.12654964412045064,
                 {{
                     LossShelf{1.1681329f, -0.240081087f, -0.0718317628f},
                     LossShelf{0.720318079f, 0.207231224f, -0.0723343343f},
                     LossShelf{0.998930693f, -0.867290199f, -0.868342996f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                 }}},
      // MIDI 56; PT8 adaptive effective loss; 1/6 active; holdout p95 21.644%
      LossPreset{56,
                 207.65234878997256,
                 0.014196194535651423,
                 0.013938705100463874,
                 {{
                     LossShelf{0.995806456f, -0.729476988f, -0.733569801f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                 }}},
      // MIDI 57; PT8 adaptive effective loss; 5/6 active; holdout p95 7.477%
      LossPreset{57,
                 220,
                 0.20465774363289654,
                 0.20419387444158632,
                 {{
                     LossShelf{0.799209714f, 0.404715091f, 0.20400995f},
                     LossShelf{0.807737887f, 0.226163834f, 0.0339748636f},
                     LossShelf{1.19591594f, -0.217351571f, -0.0213664155f},
                     LossShelf{0.953136742f, 0.0250633992f, -0.021730639f},
                     LossShelf{0.999386489f, -0.884821177f, -0.885426641f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                 }}},
      // MIDI 58; PT8 adaptive effective loss; 4/6 active; holdout p95 10.617%
      LossPreset{58,
                 233.08188075904496,
                 0.038896433622899192,
                 0.038883946816770633,
                 {{
                     LossShelf{0.988568723f, 0.582398355f, 0.571201146f},
                     LossShelf{1.0142622f, 0.313005775f, 0.327465713f},
                     LossShelf{0.965545893f, 0.051009547f, 0.016706882f},
                     LossShelf{0.991001189f, 0.0109970998f, 0.00214755442f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                 }}},
      // MIDI 59; PT8 adaptive effective loss; 3/6 active; holdout p95 10.496%
      LossPreset{59,
                 246.94165062806206,
                 0.032830022749244507,
                 0.032637969351965064,
                 {{
                     LossShelf{0.990659654f, 0.57697314f, 0.567822993f},
                     LossShelf{0.979858339f, 0.0123420283f, -0.00767927337f},
                     LossShelf{0.998098254f, -0.738902152f, -0.740772426f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                 }}},
      // MIDI 60; PT8 adaptive effective loss; 1/6 active; holdout p95 11.838%
      LossPreset{60,
                 261.62556530059862,
                 0.012926738428970547,
                 0.012550762696303477,
                 {{
                     LossShelf{0.996275842f, -0.731645703f, -0.735302925f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                 }}},
      // MIDI 61; PT8 adaptive effective loss; 1/6 active; holdout p95 36.312%
      LossPreset{61,
                 277.18263097687208,
                 0.032165145178381725,
                 0.032143675067840528,
                 {{
                     LossShelf{0.969321072f, -0.0274190437f, -0.0577470772f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                 }}},
      // MIDI 62; PT8 adaptive effective loss; 3/6 active; holdout p95 12.229%
      LossPreset{62,
                 293.66476791740757,
                 0.017741009471637766,
                 0.017690396344634925,
                 {{
                     LossShelf{0.954588652f, 0.513678193f, 0.468474418f},
                     LossShelf{1.14660776f, 0.0157275535f, 0.162497997f},
                     LossShelf{0.90009129f, -0.018637307f, -0.118422605f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                 }}},
      // MIDI 63; PT8 adaptive effective loss; 2/6 active; holdout p95 10.797%
      LossPreset{63,
                 311.12698372208092,
                 0.04035237610931805,
                 0.040346030865123311,
                 {{
                     LossShelf{0.966261804f, 0.529893398f, 0.496487528f},
                     LossShelf{0.981924295f, 0.0124844424f, -0.00537041062f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                 }}},
      // MIDI 64; PT8 adaptive effective loss; 3/6 active; holdout p95 5.877%
      LossPreset{64,
                 329.62755691286992,
                 0.01715177356241207,
                 0.017120475154654959,
                 {{
                     LossShelf{1.0076642f, 0.540508091f, 0.548424304f},
                     LossShelf{1.03198195f, -0.00856817886f, 0.0235145818f},
                     LossShelf{0.950185239f, -0.0221171007f, -0.0718403682f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                 }}},
      // MIDI 65; PT8 adaptive effective loss; 2/6 active; holdout p95 23.587%
      LossPreset{65,
                 349.22823143300388,
                 0.017514410181210166,
                 0.017487510238098584,
                 {{
                     LossShelf{1.02649641f, 0.269840032f, 0.296572834f},
                     LossShelf{0.961407363f, 0.0465278327f, 0.00811895914f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                 }}},
      // MIDI 66; PT8 adaptive effective loss; 1/6 active; holdout p95 17.964%
      LossPreset{66,
                 369.9944227116344,
                 0.022722093780179364,
                 0.022703605835845998,
                 {{
                     LossShelf{0.976765215f, 0.0324434862f, 0.00950561743f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                 }}},
      // MIDI 67; PT8 adaptive effective loss; 2/6 active; holdout p95 1.396%
      LossPreset{67,
                 391.99543598174927,
                 0.014124971719019623,
                 0.014097852447143093,
                 {{
                     LossShelf{1.01360297f, 0.42427209f, 0.438141078f},
                     LossShelf{0.976349592f, 0.0126638897f, -0.0108050527f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                 }}},
      // MIDI 68; PT8 adaptive effective loss; 2/6 active; holdout p95 13.627%
      LossPreset{68,
                 415.30469757994513,
                 0.017716359605711548,
                 0.017688442084515751,
                 {{
                     LossShelf{1.00471044f, 0.536005914f, 0.540982068f},
                     LossShelf{0.979444802f, -0.004169608f, -0.0245566461f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                 }}},
      // MIDI 69; PT8 adaptive effective loss; 2/6 active; holdout p95 3.188%
      LossPreset{69,
                 440,
                 0.10176631973743934,
                 0.10188182300953239,
                 {{
                     LossShelf{0.819824994f, 0.434237868f, 0.254359961f},
                     LossShelf{1.03941965f, -0.0901459605f, -0.0505014285f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                 }}},
      // MIDI 70; PT8 adaptive effective loss; 2/6 active; holdout p95 12.109%
      LossPreset{70,
                 466.16376151808993,
                 0.12039282452329729,
                 0.12065213345614838,
                 {{
                     LossShelf{0.757579207f, 0.284194946f, 0.0419883803f},
                     LossShelf{1.10212457f, -0.190412983f, -0.0881009176f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                 }}},
      // MIDI 71; PT8 adaptive effective loss; 1/6 active; holdout p95 11.646%
      LossPreset{71,
                 493.88330125612413,
                 0.025181731279501109,
                 0.025155578041368025,
                 {{
                     LossShelf{0.973125696f, 0.0833870545f, 0.0567829385f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                 }}},
      // MIDI 72; PT8 adaptive effective loss; 2/6 active; holdout p95 12.523%
      LossPreset{72,
                 523.25113060119725,
                 0.027911015412448042,
                 0.0279016986888144,
                 {{
                     LossShelf{0.972450256f, 0.317944348f, 0.290573001f},
                     LossShelf{0.993223786f, 0.00153785862f, -0.0051008272f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                 }}},
      // MIDI 73; PT8 adaptive effective loss; 2/6 active; holdout p95 7.145%
      LossPreset{73,
                 554.36526195374415,
                 0.042002827886067781,
                 0.042012447538418859,
                 {{
                     LossShelf{0.938014209f, 0.248360977f, 0.186526358f},
                     LossShelf{1.01008284f, -0.00341879716f, 0.00679231621f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                 }}},
      // MIDI 74; PT8 adaptive effective loss; 2/6 active; holdout p95 17.013%
      LossPreset{74,
                 587.32953583481515,
                 0.022129090265598016,
                 0.022059124264627698,
                 {{
                     LossShelf{1.0099473f, 0.45746699f, 0.467586011f},
                     LossShelf{0.971104026f, 0.0205016993f, -0.00827826094f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                 }}},
      // MIDI 75; PT8 adaptive effective loss; 1/6 active; holdout p95 8.394%
      LossPreset{75,
                 622.25396744416184,
                 0.025545885093157337,
                 0.025488415178533921,
                 {{
                     LossShelf{0.973884523f, 0.0375257097f, 0.0116621768f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                 }}},
      // MIDI 76; PT8 adaptive effective loss; 1/6 active; holdout p95 10.680%
      LossPreset{76,
                 659.25511382573984,
                 0.023227019525173656,
                 0.023188578938153494,
                 {{
                     LossShelf{0.974814057f, 0.0975872427f, 0.0726766065f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                 }}},
      // MIDI 77; PT8 adaptive effective loss; 1/6 active; holdout p95 0.884%
      LossPreset{77,
                 698.45646286600777,
                 0.025192315111285182,
                 0.02512550936792984,
                 {{
                     LossShelf{0.973972499f, 0.0477763824f, 0.0220076013f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                 }}},
      // MIDI 78; PT8 adaptive effective loss; 1/6 active; holdout p95 10.539%
      LossPreset{78,
                 739.9888454232688,
                 0.022088628743562998,
                 0.022022096880221408,
                 {{
                     LossShelf{0.977236152f, 0.0444471091f, 0.0218490809f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                 }}},
      // MIDI 79; PT8 adaptive effective loss; 2/6 active; holdout p95 8.585%
      LossPreset{79,
                 783.99087196349853,
                 0.016272462500420838,
                 0.016157406570587486,
                 {{
                     LossShelf{1.01221073f, 0.496043146f, 0.508402884f},
                     LossShelf{0.975786805f, 0.00883241743f, -0.0152838463f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                 }}},
      // MIDI 80; PT8 adaptive effective loss; 2/6 active; holdout p95 9.407%
      LossPreset{80,
                 830.60939515989025,
                 0.01687967199601674,
                 0.016746417190969479,
                 {{
                     LossShelf{1.01265442f, 0.497204214f, 0.510058165f},
                     LossShelf{0.974828541f, 0.0103882253f, -0.0146530448f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                 }}},
      // MIDI 81; PT8 adaptive effective loss; 1/6 active; holdout p95 0.924%
      LossPreset{81,
                 880,
                 0.021291830495761135,
                 0.021206022398886581,
                 {{
                     LossShelf{0.977660775f, 0.0526175536f, 0.0306478254f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                 }}},
      // MIDI 82; PT8 adaptive effective loss; 1/6 active; holdout p95 1.887%
      LossPreset{82,
                 932.32752303617985,
                 0.026945658276763824,
                 0.026823726088244456,
                 {{
                     LossShelf{0.971932948f, 0.0553923808f, 0.0276567079f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                 }}},
      // MIDI 83; PT8 adaptive effective loss; 2/6 active; holdout p95 11.709%
      LossPreset{83,
                 987.76660251224826,
                 0.013125877309455318,
                 0.012981252373833442,
                 {{
                     LossShelf{1.0092535f, 0.514246464f, 0.523883522f},
                     LossShelf{0.980557621f, 0.00677067926f, -0.0124234464f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                 }}},
      // MIDI 84; PT8 adaptive effective loss; 2/6 active; holdout p95 6.179%
      LossPreset{84,
                 1046.5022612023945,
                 0.013127152809474006,
                 0.012962497238108966,
                 {{
                     LossShelf{1.00960517f, 0.503719687f, 0.513614357f},
                     LossShelf{0.980383813f, 0.00651569711f, -0.0129119251f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                 }}},
      // MIDI 85; PT8 adaptive effective loss; 2/6 active; holdout p95 31.166%
      LossPreset{85,
                 1108.7305239074883,
                 0.010336494106209904,
                 0.010184361339962124,
                 {{
                     LossShelf{1.00755727f, 0.496961176f, 0.504841506f},
                     LossShelf{0.98449862f, -0.00317055616f, -0.0184620284f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                 }}},
      // MIDI 86; PT8 adaptive effective loss; 2/6 active; holdout p95 1.413%
      LossPreset{86,
                 1174.6590716696303,
                 0.0094712241497262994,
                 0.0092987001935452753,
                 {{
                     LossShelf{1.0093801f, 0.412381947f, 0.422089189f},
                     LossShelf{0.983681977f, 0.00238205376f, -0.0137106208f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                 }}},
      // MIDI 87; PT8 adaptive effective loss; 2/6 active; holdout p95 28.732%
      LossPreset{87,
                 1244.5079348883237,
                 0.02663527389084696,
                 0.026429796975497123,
                 {{
                     LossShelf{0.991836905f, 0.543615341f, 0.535686731f},
                     LossShelf{0.978570402f, 0.0100743789f, -0.0112041971f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                 }}},
      // MIDI 88; PT8 adaptive effective loss; 1/6 active; holdout p95 29.986%
      LossPreset{88,
                 1318.5102276514797,
                 0.024242723525079588,
                 0.024112421216637842,
                 {{
                     LossShelf{0.973033249f, 0.124768361f, 0.0981109813f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                 }}},
      // MIDI 89; PT8 adaptive effective loss; 2/6 active; holdout p95 31.945%
      LossPreset{89,
                 1396.9129257320155,
                 0.015820217400752193,
                 0.015631776755286413,
                 {{
                     LossShelf{0.998215139f, 0.74948889f, 0.748051047f},
                     LossShelf{0.984867454f, 0.0102757076f, -0.00465928577f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                 }}},
      // MIDI 90; PT8 adaptive effective loss; 2/6 active; holdout p95 35.932%
      LossPreset{90,
                 1479.9776908465376,
                 0.011949162604826494,
                 0.011838660788130716,
                 {{
                     LossShelf{0.994577289f, 0.628071725f, 0.623101294f},
                     LossShelf{0.990929067f, 0.0043034968f, -0.00449006725f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                 }}},
      // MIDI 91; PT8 adaptive effective loss; 2/6 active; holdout p95 2.256%
      LossPreset{91,
                 1567.9817439269971,
                 0.0094224663703528447,
                 0.0094029046271016572,
                 {{
                     LossShelf{0.98817575f, 0.221790612f, 0.210475758f},
                     LossShelf{0.999571681f, 3.75041191e-06f, -3.75212448e-06f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                 }}},
      // MIDI 92; PT8 adaptive effective loss; 2/6 active; holdout p95 17.951%
      LossPreset{92,
                 1661.2187903197805,
                 0.05980265986965088,
                 0.059651421433927286,
                 {{
                     LossShelf{0.965376496f, 0.285068303f, 0.250776976f},
                     LossShelf{0.967900574f, 0.0145719349f, -0.0172664709f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                 }}},
      // MIDI 93; PT8 adaptive effective loss; 1/6 active; holdout p95 0.000%
      LossPreset{93,
                 1760,
                 0.031335558273157184,
                 0.031178911490643207,
                 {{
                     LossShelf{0.96814698f, 0.0235108417f, -0.00764639117f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                 }}},
      // MIDI 94; PT8 adaptive effective loss; 1/6 active; holdout p95 0.000%
      LossPreset{94,
                 1864.6550460723597,
                 0.038536421715804986,
                 0.038316661095661279,
                 {{
                     LossShelf{0.961406112f, 0.0244043134f, -0.0137004927f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                 }}},
      // MIDI 95; PT8 adaptive effective loss; 1/6 active; holdout p95 0.000%
      LossPreset{95,
                 1975.5332050244961,
                 0.026272560648429276,
                 0.026108283424544517,
                 {{
                     LossShelf{0.972850621f, 0.0222706478f, -0.00395919289f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                 }}},
      // MIDI 96; PT8 adaptive effective loss; 2/6 active; holdout p95 0.000%
      LossPreset{96,
                 2093.004522404789,
                 0.078584138919259847,
                 0.078459934557733438,
                 {{
                     LossShelf{0.926423907f, 0.920865417f, 0.847760558f},
                     LossShelf{0.96115613f, 0.0294361282f, -0.00915507972f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                 }}},
      // MIDI 97; PT8 adaptive effective loss; 2/6 active; holdout p95 0.000%
      LossPreset{97,
                 2217.4610478149766,
                 0.074208225893439422,
                 0.074119103851506607,
                 {{
                     LossShelf{0.924043834f, 0.918499589f, 0.843072057f},
                     LossShelf{0.966758668f, 0.0262892637f, -0.00666718977f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                 }}},
      // MIDI 98; PT8 adaptive effective loss; 2/6 active; holdout p95 0.000%
      LossPreset{98,
                 2349.3181433392601,
                 0.040668510748089269,
                 0.040306363796772555,
                 {{
                     LossShelf{0.999687552f, 0.984731495f, 0.984975755f},
                     LossShelf{0.959391832f, 0.028217487f, -0.012113628f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                 }}},
      // MIDI 99; PT8 adaptive effective loss; 1/6 active; holdout p95 0.000%
      LossPreset{99,
                 2489.0158697766474,
                 0.0087108935035869594,
                 0.0086563702963833918,
                 {{
                     LossShelf{0.989404142f, 0.082995683f, 0.0737152323f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                 }}},
      // MIDI 100; PT8 adaptive effective loss; 1/6 active; holdout p95 0.000%
      LossPreset{100,
                 2637.0204553029598,
                 0.001076057116530133,
                 0.0010687145280212321,
                 {{
                     LossShelf{0.997546852f, 0.0832279176f, 0.0821666643f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                 }}},
      // MIDI 101; PT8 adaptive effective loss; 1/6 active; holdout p95 0.000%
      LossPreset{101,
                 2793.8258514640311,
                 0.0010528550964885128,
                 0.0010447837373837056,
                 {{
                     LossShelf{0.997409046f, 0.0831896812f, 0.0821666643f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                 }}},
      // MIDI 102; PT8 adaptive effective loss; 1/6 active; holdout p95 0.000%
      LossPreset{102,
                 2959.9553816930752,
                 0.00076717842403185919,
                 0.00076056264574003122,
                 {{
                     LossShelf{0.997653842f, 0.0828748196f, 0.0821666643f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                 }}},
      // MIDI 103; PT8 adaptive effective loss; 1/6 active; holdout p95 0.000%
      LossPreset{103,
                 3135.9634878539946,
                 0.00078442827509554649,
                 0.00077682824987908696,
                 {{
                     LossShelf{0.997455895f, 0.0828791335f, 0.0821666643f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                 }}},
      // MIDI 104; PT8 adaptive effective loss; 1/6 active; holdout p95 0.000%
      LossPreset{104,
                 3322.437580639561,
                 0.00084422519217250045,
                 0.00083503615180350362,
                 {{
                     LossShelf{0.997120202f, 0.0829221159f, 0.0821666643f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                 }}},
      // MIDI 105; PT8 adaptive effective loss; 1/6 active; holdout p95 0.000%
      LossPreset{105,
                 3520,
                 0.00082491670391699856,
                 0.00081482458100261561,
                 {{
                     LossShelf{0.997010171f, 0.0828909054f, 0.0821666643f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                 }}},
      // MIDI 106; PT8 adaptive effective loss; 1/6 active; holdout p95 0.000%
      LossPreset{106,
                 3729.3100921447194,
                 0.00090940322020846348,
                 0.00089690178229269554,
                 {{
                     LossShelf{0.996842861f, 0.082977213f, 0.0821666643f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                 }}},
      // MIDI 107; PT8 adaptive effective loss; 1/6 active; holdout p95 0.000%
      LossPreset{107,
                 3951.0664100489921,
                 0.0015049289270278963,
                 0.0014841647661936032,
                 {{
                     LossShelf{0.995968819f, 0.0981342122f, 0.0967058763f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                 }}},
      // MIDI 108; PT8 adaptive effective loss; 1/6 active; holdout p95 0.000%
      LossPreset{108,
                 4186.009044809578,
                 0.00081468721060068069,
                 0.0008005229738574707,
                 {{
                     LossShelf{0.996237099f, 0.0828170404f, 0.0821666643f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                     LossShelf{1.0f, 0.0f, 0.0f},
                 }}},
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
      if (size < 2)
        throw std::runtime_error("string_model: size is too small, size: " +
                                 std::to_string(size));
      if (position > 1.0 || position <= 0.0)
        throw std::runtime_error(
            "string_model: position isn't good, position: " +
            std::to_string(position));
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
  float *left_boundary_point = nullptr;
  float *right_boundary_point = nullptr;

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
  StringModel(double sample_rate, int midi_n,
              TunningPresets::Temperament temperament,
              TunningPresets::StringIndex string_index,
              const Configuration *configuration)
      : configuration_(configuration), midi_n_(midi_n),
        f0(configuration->tuning_presets.get_frequency(midi_n_, temperament,
                                                       string_index)),
        sample_rate_(sample_rate),
        internal_sample_rate_(midi_n_ >= 92 ? 2.0 * sample_rate_
                                            : sample_rate_),
        loss_filter(midi_n_), dispersion_filter(f0),
        // loss_filter.get_phase_delay(sample_rate, f0)
        loss_phase_delay(
            loss_filter.get_phase_delay(internal_sample_rate_, f0)),
        //        dispersion_phase_delay(dispersion_filter.get_phase_delay(sample_rate_,
        //        f0)), delay((internal_sample_rate_ / f0 - loss_phase_delay)
        //        / 2.0), delay((sample_rate_ / f0 - loss_phase_delay -
        //        dispersion_phase_delay) / 2.0),
        delay(dispersion_filter.get_loop_delay_samples() / 2.0),
        // delay_int 是数组节点数；真实单程整数延迟为 delay_int - 1。
        delay_int(static_cast<int>(std::floor(delay)) + 1),
        // fractional filter 每圈一次，因此承担完整 round-trip residual。
        delay_frac(2.0 * (delay - std::floor(delay))),
        traveling_wave_max_index(delay_int - 1),
        strike_port(delay_int, strike_point),
        fractional_filter(delay_frac, 2.0 * std::numbers::pi_v<double> * f0 /
                                          internal_sample_rate_) {

    if (delay_int < 4)
      throw std::runtime_error("string_model: delay_int is too small: " +
                               std::to_string(delay_int));
    if (strike_port.index_a + 1 > traveling_wave_max_index ||
        strike_port.index_a - 1 < 0) {
      throw std::runtime_error("string_model: next_index doesn't exist: " +
                               std::to_string(delay_int));
    }
    if (std::abs(dispersion_filter.design_sample_rate() -
                 internal_sample_rate_) > 0.5) {
      throw std::runtime_error(
          "string_model: dispersion preset sample rate mismatch");
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

    z_ = configuration->string_impedance_presets.get_characteristic_impedance(
        midi_n_);
    c_z = 1.0 / (2.0 * z_);
  }

  inline void propagate() {
    // ======================== ========================
    // Reverse moving boundary index
    // 反向移动边界下标
    // ======================== ========================
    if (left_head == traveling_wave_max_index)
      left_head = 0;
    else
      left_head++;
    if (right_head == 0)
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
    return real_index + head <= traveling_wave_max_index
               ? real_index + head
               : real_index + head - delay_int;
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
    left[get_i(strike_port.index_a, left_head)] +=
        inject_v * strike_port.weight_a;
    left[get_i(strike_port.index_b, left_head)] +=
        inject_v * strike_port.weight_b;
    right[get_i(strike_port.index_a, right_head)] +=
        inject_v * strike_port.weight_a;
    right[get_i(strike_port.index_b, right_head)] +=
        inject_v * strike_port.weight_b;
  }

  inline double get_bridge_force() {
    return -2.0 * z_ * static_cast<double>(*right_boundary_point);
  }

  inline double get_string_vs() {
    return strike_port.weight_a * left[get_i(strike_port.index_a, left_head)] +
           strike_port.weight_b * left[get_i(strike_port.index_b, left_head)] +
           strike_port.weight_a *
               right[get_i(strike_port.index_a, right_head)] +
           strike_port.weight_b * right[get_i(strike_port.index_b, right_head)];
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
    if (damper_active) {
      damper.process(*left_boundary_point);
    }
  }

  int inactive_probe_count_ = 0;
  int activity_probe_counter_ = 0;
  inline void check_active() {
    const int probe_interval = midi_n_ >= 92 ? 128 : 64;
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
      rail_energy += left[static_cast<std::size_t>(index)] *
                         left[static_cast<std::size_t>(index)] +
                     right[static_cast<std::size_t>(index)] *
                         right[static_cast<std::size_t>(index)];
    }
    rail_energy /= static_cast<float>(2 * delay_int);

    float state_energy =
        std::max(loss_filter.state_energy(), fractional_filter.state_energy());
    state_energy = std::max(
        state_energy, static_cast<float>(dispersion_filter.state_energy()));
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

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>

class SoundboardModel {
  static constexpr bool soundboard_active_ = false;

  SoundboardModel(const SoundboardModel &) = delete;
  SoundboardModel &operator=(const SoundboardModel &) = delete;

  static constexpr std::size_t kKeyCount = 88;
  static constexpr std::size_t kFDNSize = 8;
  static constexpr std::size_t kMaxDelay = 721;

  // Bank:
  //
  // 37, 87, 181, 271, 359, 492, 687, 721 samples
  //
  // Short delays are essential for preserving the early energy
  // of the piano soundboard impulse response.
  static constexpr std::array<std::size_t, kFDNSize> kDelayLengths{
      37, 87, 181, 271, 359, 492, 687, 721};

  // ================================================================
  // Delay line
  // ================================================================

  struct DelayLine {
    std::array<float, kMaxDelay> data{};

    std::size_t length = 1;
    std::size_t index = 0;

    inline void set_length(std::size_t new_length) noexcept {
      length = std::clamp<std::size_t>(new_length, 1, kMaxDelay);

      index = 0;
      data.fill(0.0f);
    }

    inline float read() const noexcept { return data[index]; }

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

  static constexpr std::size_t kLossSectionCount = 8;

  // ================================================================
  // 单一二阶filter的公式：
  //              b0 + b1 z^-1 + b2 z^-2
  // H(z) = ---------------------------------
  //              1 + a1 z^-1 + a2 z^-2
  // 此处阶数的意思是系统差分方程需要记忆多少阶过去状态
  // ================================================================

  struct LossSection {
    float b0 = 1.0f;
    float b1 = 0.0f;
    float b2 = 0.0f;

    float a1 = 0.0f;
    float a2 = 0.0f;

    float s1 = 0.0f;
    float s2 = 0.0f;

    inline float process(float x) noexcept {
      const float y = b0 * x + s1;

      s1 = b1 * x - a1 * y + s2;

      s2 = b2 * x - a2 * y;

      return y;
    }

    inline void reset() noexcept {
      s1 = 0.0f;
      s2 = 0.0f;
    }
  };

  // ================================================================
  // 二阶filters的串联公式：
  // H_total(z) = H_1(z) H_2(z) ... H_M(z)
  // ================================================================

  struct LossFilter {
    std::array<LossSection, kLossSectionCount> sections{};

    inline float process(float x) noexcept {
      for (auto &section : sections) {
        x = section.process(x);
      }

      return x;
    }

    inline void reset() noexcept {
      for (auto &section : sections) {
        section.reset();
      }
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
      low_state = alpha * low_state + (1.0f - alpha) * x;

      const float low = low_state;
      const float high = x - low;

      return low_gain * low + high_gain * high;
    }

    inline void reset() noexcept { low_state = 0.0f; }
  };

  std::array<DelayLine, kFDNSize> delay_lines_{};
  std::array<LossFilter, kFDNSize> loss_filters_{};

  RegionShaper bass_shaper_{};
  RegionShaper middle_shaper_{};
  RegionShaper treble_shaper_{};

  double sample_rate_ = 44100.0;

public:
  mutable std::array<float, kKeyCount> bridge_force{};

  explicit SoundboardModel(double sample_rate = 44100.0)
      : sample_rate_(sample_rate) {
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
    if (!soundboard_active_) {
      float sum = 0.0;
      for (int i = 0; i < bridge_force.size(); i++) {
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

    const float shaped_force = bass_shaper_.process(bass_force) +
                               middle_shaper_.process(middle_force) +
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

      delayed[i] = loss_filters_[i].process(delay_lines_[i].read());

      delayed_sum += delayed[i];
    }

    // ------------------------------------------------------------
    // 4. FDN output vector c
    //
    // A sign-alternating normalized vector avoids all delay lines
    // contributing with identical phase/sign.
    // ------------------------------------------------------------

    constexpr float q = 0.3535533905932738f; // 1 / sqrt(8)

    const float fdn_output =
        q * (delayed[0] - delayed[1] + delayed[2] - delayed[3] + delayed[4] -
             delayed[5] + delayed[6] - delayed[7]);

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

    const float common = 0.25f * delayed_sum;

    // ------------------------------------------------------------
    // Input vector b.
    //
    // Normalized equal excitation of all delay lines.
    // ------------------------------------------------------------

    const float input = q * shaped_force;

    for (std::size_t i = 0; i < kFDNSize; ++i) {

      const std::size_t shifted = (i + 1) & 7;

      const float feedback = delayed[shifted] - common;

      delay_lines_[i].write(feedback + input);
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

    return fdn_output + direct_gain * shaped_force;
  }

  inline void system_reset() noexcept {

    bridge_force.fill(0.0f);

    for (auto &delay : delay_lines_) {
      delay.reset();
    }

    for (auto &loss : loss_filters_) {
      loss.reset();
    }

    bass_shaper_.reset();
    middle_shaper_.reset();
    treble_shaper_.reset();
  }

private:
  inline void initialize_delays() noexcept {

    for (std::size_t i = 0; i < kFDNSize; ++i) {
      delay_lines_[i].set_length(kDelayLengths[i]);
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
          static_cast<double>(kDelayLengths[i]) / sample_rate_;

      const double gain = std::pow(10.0, -3.0 * delay_seconds / target_t60);

      // H(z) = 1
      //
      // b0 = 1
      // b1 = b2 = a1 = a2 = 0

      for (auto &section : loss_filters_[i].sections) {
        section.b0 = 1.0f;
        section.b1 = 0.0f;
        section.b2 = 0.0f;

        section.a1 = 0.0f;
        section.a2 = 0.0f;

        section.s1 = 0.0f;
        section.s2 = 0.0f;
      }

      // 让二阶filter塌缩成一阶filter，以此先证明两者可在此参数设置下等价:
      //
      //            g (1 - a)
      // H(z) = ----------------
      //            1 - a z^-1
      //
      // 通过进行以下的参数代入：
      // b0 = g (1 - a)
      // b1 = 0
      // b2 = 0
      // a1 = -a
      // a2 = 0

      auto &section = loss_filters_[i].sections[0];

      section.b0 = static_cast<float>(gain) * (1.0f - loss_pole);

      section.b1 = 0.0f;
      section.b2 = 0.0f;

      section.a1 = -loss_pole;
      section.a2 = 0.0f;
    }
  }

  inline void initialize_region_shapers() noexcept {

    // One-pole crossover around 1.8 kHz.
    //
    // alpha = exp(-2πfc/Fs)

    constexpr double crossover_hz = 1800.0;

    const float alpha = static_cast<float>(
        std::exp(-2.0 * 3.14159265358979323846 * crossover_hz / sample_rate_));

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
// class SoundboardModel {
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
// public:
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
// private:
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
// #endif /* soundboard_model_hpp */
//

// #ifndef soundboard_model_hpp
// #define soundboard_model_hpp
//
// #include <array>
// class SoundboardModel {
//     SoundboardModel(const SoundboardModel&) = delete;
//     SoundboardModel& operator=(const SoundboardModel&) = delete;
//
//     // Temporary bridge-force -> acoustic-radiation proxy.
//     //
//     // This filter is intentionally placed outside the mechanical
//     // string/bridge feedback loop. It does not represent the soundboard
//     // mobility itself; it only approximates the spectral coloration
//     // between total bridge excitation and radiated acoustic output.
//     //
//     // Fs = 44100 Hz
//     float radiation_s1_ = 0.0f;
//     float radiation_s2_ = 0.0f;
//
// public:
//     mutable std::array<float, 88> bridge_force{};
//
//     explicit SoundboardModel() = default;
//
//     inline float get_sample() {
//         float bridge_excitation = 0.0f;
//
//         for (std::size_t i = 0; i < bridge_force.size(); ++i) {
//             bridge_excitation += bridge_force[i];
//         }
//
//         /*
//             Temporary radiation transfer function:
//
//                          b0 + b1 z^-1 + b2 z^-2
//                 H(z) = -------------------------
//                           1 + a1 z^-1 + a2 z^-2
//
//             This is NOT a physical soundboard model.
//
//             It is a low-order approximation of the transfer from
//             aggregated bridge excitation to radiated acoustic output,
//             introduced only to avoid treating raw bridge force as PCM.
//         */
//
//         constexpr float b0 = 0.08315987f;
//         constexpr float b1 = 0.16631974f;
//         constexpr float b2 = 0.08315987f;
//
//         constexpr float a1 = -1.03517121f;
//         constexpr float a2 = 0.36781069f;
//
//         const float radiation =
//             b0 * bridge_excitation + radiation_s1_;
//
//         radiation_s1_ =
//             b1 * bridge_excitation
//             - a1 * radiation
//             + radiation_s2_;
//
//         radiation_s2_ =
//             b2 * bridge_excitation
//             - a2 * radiation;
//
//         return radiation;
//     }
//
//     inline void system_reset() {
//         bridge_force.fill(0.0f);
//
//         radiation_s1_ = 0.0f;
//         radiation_s2_ = 0.0f;
//     }
// };
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
// #endif /* soundboard_model_hpp */


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
            if(strings_[i].midi_n_ < 92) {
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
            if(strings_[i].midi_n_ < 92) {
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
