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
// 2026-09-19
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

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
