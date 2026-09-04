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
// 2026-09-03
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

#include <iostream>
#include <csignal>
#include <cstdlib>
#include <getopt.h>
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>

#include "../cli_helper.hpp"
#include "../../controller_hardware.hpp"

// ======================== ======================== ========================
// Internal declarations
// 内部声明
// ======================== ======================== ========================
namespace {

// ------------------------ ------------------------
// CLI functions
// cli 函数
// ------------------------ ------------------------
void print_logo_and_version(const char* version, const char* logo);
void signal_handler(int);
void help();

// ------------------------ ------------------------
// Engine functions
// engine 函数
// ------------------------ ------------------------
void init_engine();
void shutdown_engine();

}

// ======================== ======================== ========================
// Public entry
// 公开入口
// ======================== ======================== ========================
int cli_helper(int argc, char* argv[], const char* version, const char* logo) {
    
    // ------------------------ ------------------------
    // Register a signal handler
    // 注册信号处理函数
    // ------------------------ ------------------------
    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);
    
    // ------------------------ ------------------------
    // Parse
    // 解析
    // ------------------------ ------------------------
    static const option long_options[] = {
        // ------------------------
        // CLI commands
        // Cli命令
        // ------------------------
        {"help",     no_argument,       nullptr, 'h'},
        {"version",  no_argument,       nullptr, 'v'},
        // ------------------------
        // Service commands
        // Service命令
        // ------------------------
        {"midi",     required_argument, nullptr, 'm'},// midi_path
        {"piano",    no_argument,       nullptr, 'p'},
        {"keyboard",  no_argument,       nullptr, 'k'},
        {"export",   required_argument, nullptr, 'e'},// midi_path
        {"record",   no_argument,       nullptr, 'r'},
        {"test",     no_argument,       nullptr, 't'},
        {"internal-test", no_argument,   nullptr, 'i'},
        // ------------------------
        // Basic option ending
        // 基本option结尾
        // ------------------------
        {nullptr,   0,                  nullptr,  0 }
    };
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
        std::optional<std::string> midi = "";
        bool piano = false;
        bool keyboard = false;
        std::optional<std::string> export_path = "";
        bool record = false;
        bool test = false;
        bool internal_test = false;
    };
    CliOptions options;
    
    while ((opt = getopt_long(argc, argv, "hvm:pke:rti", long_options, nullptr)) != -1) {
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
    if(!(options.help || options.version || options.midi || options.piano || options.keyboard ||
         options.export_path || options.record || options.test || options.internal_test)) {
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
    init_engine();
    if (options.midi) {
        midi_service(*options.midi);
        shutdown_engine();
        return EXIT_SUCCESS;
    }
    if (options.piano) {
        piano_service();
        shutdown_engine();
        return EXIT_SUCCESS;
    }
    if (options.keyboard) {
        keyboard_service();
        shutdown_engine();
        return EXIT_SUCCESS;
    }
    if (options.export_path) {
        export_service(*options.export_path);
        shutdown_engine();
        return EXIT_SUCCESS;
    }
    if (options.record) {
        record_service();
        shutdown_engine();
        return EXIT_SUCCESS;
    }
    if (options.test) {
        test_service();
        shutdown_engine();
        return EXIT_SUCCESS;
    }
    if (options.internal_test) {
        internal_test_service();
        shutdown_engine();
        return EXIT_SUCCESS;
    }

    throw std::runtime_error("Failed to correctly parse the command.");
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
void print_logo_and_version(const char* version, const char* logo) {
    std::cout<<logo<<"\n\n";
    std::cout<<version<<"\n";
}
void signal_handler(int) {
    // TODO: Replace direct shutdown with signal-safe cancellation.
    
    // ------------------------
    // Function to execute before exiting
    // 退出前需要执行的函数
    // ------------------------
    shutdown_engine();
    
    // ------------------------
    // Exit
    // 退出程序
    // ------------------------
    std::exit(EXIT_SUCCESS);
}
void help() {
    std::cout<<"help..."<<"\n";
}

// ------------------------ ------------------------
// Engine functions
// Engine 函数
// ------------------------ ------------------------
void init_engine() {
    
}
void shutdown_engine() {
    
}


}
