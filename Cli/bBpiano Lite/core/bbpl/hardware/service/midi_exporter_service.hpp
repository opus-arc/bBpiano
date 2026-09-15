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

#include "../../piano_controller.hpp"
#include "midi_service.hpp"

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
