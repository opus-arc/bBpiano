#ifndef piano_controller_hpp
#define piano_controller_hpp

#include <stop_token>
#include "./configuration/configuration.hpp"

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
