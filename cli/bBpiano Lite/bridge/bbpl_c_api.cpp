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

#include "./bbpl_c_api.h"

#include "../core/service_controller.hpp"

#include <thread>
#include <string>

namespace {

// 静态存储期
/// 程序结束之后会自动析构
std::jthread midi_thread;
std::jthread piano_thread;
std::jthread keyboard_thread;
std::jthread export_thread;
std::jthread record_thread;
std::jthread test_thread;
std::jthread internal_test_thread;

}

// ========================
// MIDI Service
// ========================

void midi_service_start(const char* midi_path) {
    if(midi_path == nullptr) {
        return ;
    }
    
    // 这个 std::jthread 对象当前是否关联着一个可以被 join 的执行线程?
    if(midi_thread.joinable()) {
        midi_thread.request_stop(); // stop_token.stop_requested() = true;
        midi_thread.join(); // 确保上一个线程真的结束
    }
    
    std::string midi_path_string(midi_path);
    
    // 这里的线程接受一个 lambda
    midi_thread = std::jthread(
        [midi_path_string = std::move(midi_path_string)](std::stop_token stop_token){
            midi_service(midi_path_string, stop_token);
        }
    );
    
}
void midi_service_stop(void) {
    if(midi_thread.joinable()) {
        midi_thread.request_stop();
        midi_thread.join();
    }
}


// ========================
// Piano Service
// ========================

void piano_service_start(void) {
    if(piano_thread.joinable()) {
        piano_thread.request_stop();
        piano_thread.join();
    }
    
    piano_thread = std::jthread(
        [](std::stop_token stop_token){
            piano_service(stop_token);
        }
    );
}
void piano_service_stop(void) {
    if(piano_thread.joinable()) {
        piano_thread.request_stop();
        piano_thread.join();
    }
}


// ========================
// Keyboard Service
// ========================

void keyboard_service_start(void) {
    if(keyboard_thread.joinable()) {
        keyboard_thread.request_stop();
        keyboard_thread.join();
    }
    
    keyboard_thread = std::jthread(
        [](std::stop_token stop_token){
            keyboard_service(stop_token);
        }
    );
}
void keyboard_service_stop(void) {
    if(keyboard_thread.joinable()) {
        keyboard_thread.request_stop();
        keyboard_thread.join();
    }
}


// ========================
// Export Service
// ========================

void export_service_start(const char* midi_path) {
    if(midi_path == nullptr) {
        return ;
    }

    if(export_thread.joinable()) {
        export_thread.request_stop();
        export_thread.join();
    }
    
    std::string midi_path_string(midi_path);
    
    export_thread = std::jthread(
        [midi_path_string = std::move(midi_path_string)](std::stop_token stop_token){
            export_service(midi_path_string, stop_token);
        }
    );
}
void export_service_stop(void) {
    if(export_thread.joinable()) {
        export_thread.request_stop();
        export_thread.join();
    }
}


// ========================
// Record Service
// ========================

void record_service_start(void) {
    if(record_thread.joinable()) {
        record_thread.request_stop();
        record_thread.join();
    }
    
    record_thread = std::jthread(
        [](std::stop_token stop_token){
            record_service(stop_token);
        }
    );
}
void record_service_stop(void) {
    if(record_thread.joinable()) {
        record_thread.request_stop();
        record_thread.join();
    }
}


// ========================
// Test Service
// ========================

void test_service_start(void) {
    if(test_thread.joinable()) {
        test_thread.request_stop();
        test_thread.join();
    }
    
    test_thread = std::jthread(
        [](std::stop_token stop_token){
            test_service(stop_token);
        }
    );
}
void test_service_stop(void) {
    if(test_thread.joinable()) {
        test_thread.request_stop();
        test_thread.join();
    }
}


// ========================
// Internal Test Service
// ========================

void internal_test_service_start(void) {
    if(internal_test_thread.joinable()) {
        internal_test_thread.request_stop();
        internal_test_thread.join();
    }
    
    internal_test_thread = std::jthread(
        [](std::stop_token stop_token){
            internal_test_service(stop_token);
        }
    );
}
void internal_test_service_stop(void) {
    if(internal_test_thread.joinable()) {
        internal_test_thread.request_stop();
        internal_test_thread.join();
    }
}

// ========================
// Shotdown All Service
// ========================
void close_all_service(void) {
    midi_service_stop();
    piano_service_stop();
    keyboard_service_stop();
    record_service_stop();
    export_service_stop();
    test_service_stop();
    internal_test_service_stop();
}
