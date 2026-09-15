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

#include <csignal>
#include <cstdlib>
#include <getopt.h>
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <mutex>
#include <pthread.h>
#include <signal.h>
#include <stop_token>
#include <system_error>
#include <thread>
#include <unistd.h>
#include <exception>
#include <optional>

#include "../cli_helper.hpp"
#include "../soundcard_helper.hpp"

#include "../../../service_controller.hpp"
#include "../../piano_controller.hpp"

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

    EngineLifetime(const EngineLifetime&) = delete;
    EngineLifetime& operator=(
        const EngineLifetime&) = delete;

    ~EngineLifetime() {
        shutdown_engine();
    }
};

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
    // std::signal(SIGINT, signal_handler);
    // std::signal(SIGTERM, signal_handler);
    sigset_t signals;
    sigemptyset(&signals);

    sigaddset(&signals, SIGINT);
    sigaddset(&signals, SIGTERM);
    sigaddset(&signals, SIGUSR1);

    const int mask_status =
        pthread_sigmask(
            SIG_BLOCK,
            &signals,
            nullptr);

    if (mask_status != 0) {
        std::cerr
            << "Failed to block process signals.\n";
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
        std::optional<std::string> midi = std::nullopt;
        bool piano = false;
        bool keyboard = false;
        std::optional<std::string> export_path = std::nullopt;
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
    std::stop_source service_stop;
    const std::stop_token stop_token = service_stop.get_token();
    
    // 在导出模式之外都启动声卡
    const bool start_soundcard = !options.export_path.has_value();
    EngineLifetime engine(start_soundcard);
    
    // 信号等待线程：
    std::jthread signal_waiter(
        [signals, &service_stop](
            std::stop_token thread_stop) {

            const pthread_t own_thread =
                pthread_self();

            // jthread 自己销毁时，用 SIGUSR1 唤醒 sigwait。
            std::stop_callback wake_on_thread_stop(
                thread_stop,
                [own_thread] {
                    static_cast<void>(
                        pthread_kill(
                            own_thread,
                            SIGUSR1));
                });

            int received_signal = 0;

            if(sigwait(&signals, &received_signal) == 0 &&
                    received_signal != SIGUSR1) {
                service_stop.request_stop();
            }
        });
    
    // 音频故障等待线程
    std::jthread engine_watcher(
        [&service_stop] {
            const bool failed =
                wait_for_stop_or_audio_failure(
                    service_stop.get_token());

            if (failed &&
                !service_stop.stop_requested()) {
                service_stop.request_stop();
            }
        });
    
    std::exception_ptr service_error;

    try {
        if (options.midi) {
            midi_service(*options.midi, stop_token);
        } else if (options.piano) {
            piano_service(stop_token);
        } else if (options.keyboard) {
            keyboard_service(stop_token);
        } else if (options.export_path) {
            export_service(*options.export_path, stop_token);
        } else if (options.record) {
            record_service(stop_token);
        } else if (options.test) {
            test_service(stop_token);
        } else if (options.internal_test) {
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

    const bool render_failed =
        audio_render_failed();

    if (service_error) {
        try {
            std::rethrow_exception(service_error);
        } catch (const std::exception& error) {
            std::cerr
                << "bbpl: "
                << error.what()
                << '\n';
        } catch (...) {
            std::cerr
                << "bbpl: unknown service failure.\n";
        }

        return EXIT_FAILURE;
    }

    if (render_failed) {
        std::cerr
            << "The audio render callback failed.\n";
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
void print_logo_and_version(const char* version, const char* logo) {
    std::cout<<logo;
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



}
