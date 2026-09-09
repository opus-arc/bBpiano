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
#include "./controller_hardware.hpp"
#include "./components/cli_helper.hpp"


// ======================== ======================== ========================
// Basic support
// 基础支持
// ======================== ======================== ========================
int cli_entry(int argc, char* argv[], const char* version, const char* logo) {
    return cli_helper(argc, argv, version, logo);
}

// ======================== ======================== ========================
// Services
// 服务
// ======================== ======================== ========================
void midi_service(std::string midi_path_string) {
    std::cout<<"midi: " + midi_path_string<<"\n";
}
void piano_service() {
    
}
void keyboard_service() {
    
}
void export_service(std::string export_midi_path_string) {
    
}
void record_service() {
    
}
void test_service() {
    
}
void internal_test_service() {
    
}

