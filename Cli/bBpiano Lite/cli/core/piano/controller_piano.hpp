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
#ifndef controller_piano_hpp
#define controller_piano_hpp


void bbpiano_init();
void bbpiano_shutdown();

void get_next_buffer(float* out, int frame_count, double amplitude_limiter);
void note_on(int midi_n, double velocity);
void note_off(int midi_n, double velocity);
void note_aftertouch(int midi_n, double pressure);

void softpedal_control(double depth);
void harmonicpedal_control(double depth);
void sostenutopedal_control(double depth);
void sustainpedal_control(double depth);

double get_engineRate();

void all_silence();

#endif /* controller_piano_hpp */
