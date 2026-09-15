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

#ifndef cli_helper_hpp
#define cli_helper_hpp

int cli_helper(int argc, char* argv[], const char* version, const char* logo);

#endif /* cli_helper_hpp */
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
int cli_entry(int argc, char* argv[], const char* version, const char* logo);

// ======================== ======================== ========================
// Services
// 服务
// ======================== ======================== ========================
void midi_service(const std::string& midi_path, std::stop_token stop_token);
void piano_service(std::stop_token stop_token);
void keyboard_service(std::stop_token stop_token);
void export_service(std::string export_midi_path_string, std::stop_token stop_token);
void record_service(std::stop_token stop_token);
void test_service(std::stop_token stop_token);
void internal_test_service(std::stop_token stop_token);

#endif /* controller_hardware_hpp */
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
double get_engine_rate() noexcept;

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
    std::cout<<logo<<"\n\n";
    std::cout<<version<<"\n";
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
            0.95);

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


// N / (m/s) = kg/s.
// ASSUMPTION / 待标定：当前 StringModel 对所有键都使用 2.31。
// 先逐键保存该值以保持现有行为；这不是 88 键实测阻抗表。
constexpr double kCurrentReferenceStringImpedance = 2.31;


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

        computeCharacteristicImpedance(
            tables.characteristicImpedance
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


    static void computeCharacteristicImpedance(
        StringImpedanceTable& characteristicImpedance
    )
    {
        characteristicImpedance.fill(
            Parameters::Tuning::kCurrentReferenceStringImpedance
        );
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
    HammerPreset{6.12081538397e+12, 7268117.35239, 0.173047550188, 84.5865652624, 3.38793275297, 2.46996503446, 1.70189411996, 1.39072232225, 0.0110001, -0.000240832882693},  // MIDI 21
    HammerPreset{2.05088568496e+13, 2039736.84416, 1.01552603213, 88.3384147364, 3.51182437587, 2.40432865557, 1.55693252736, 1.5019573828, 0.0109264, -0.00018028613841},  // MIDI 22
    HammerPreset{1.70094749778e+16, 21507176.7739, 1.74610687156, 75.6096122833, 4.31941733801, 2.28770707675, 1.83759093501, 1.42834399857, 0.0108529, -0.000163800701705},  // MIDI 23
    HammerPreset{1.43066472528e+18, 269201381.657, 0.44817226607, 55.5525611047, 4.89861580358, 1.4855834808, 2.01692459807, 1.24354040394, 0.0107796, -0.000213657113702},  // MIDI 24
    HammerPreset{1.28047700014e+16, 9958335.98381, 0.587093274444, 74.1579959467, 4.28376738725, 1.20404692556, 1.65976525549, 1.57424506987, 0.0107065, -0.000186292327257},  // MIDI 25
    HammerPreset{3.32790595344e+16, 83677125.7068, 0.1864900088, 61.7541413228, 4.40007248295, 1.20475883841, 1.90449259098, 1.41819302732, 0.0106336, -0.000134891306302},  // MIDI 26
    HammerPreset{3.05903227125e+16, 211852543.065, 0.0467939187322, 69.199896095, 4.38243676889, 1.24619014953, 2.05177120188, 1.44927089406, 0.0105609, -0.000208910462526},  // MIDI 27
    HammerPreset{3.04343261984e+14, 16475294834.3, 0.0613979825323, 65.1614795684, 3.82922860312, 1.4790957827, 2.58173152874, 1.32189541943, 0.0104884, -0.000175317084769},  // MIDI 28
    HammerPreset{3.93806865967e+14, 41028014460.9, 0.382855849117, 76.1472058154, 3.8316174556, 1.55237740074, 2.72555419449, 1.39804563065, 0.0104161, -0.000171318877713},  // MIDI 29
    HammerPreset{5.91379458356e+13, 4867145600.73, 0.211964190447, 74.60252053, 3.59281107716, 1.91819022189, 2.50085086362, 1.39277325047, 0.010344, -0.000147904446437},  // MIDI 30
    HammerPreset{1.75912745691e+15, 59774063.9782, 0.077483832071, 74.4874948296, 4.01927469017, 1.76581867744, 1.9252691081, 1.39169248333, 0.0102721, -0.000217273777138},  // MIDI 31
    HammerPreset{1.63734661394e+16, 98674109.6427, 0.200361291071, 71.8114384336, 4.2815642894, 1.99500419498, 1.92997505294, 1.40670434132, 0.0102004, -0.000154190357793},  // MIDI 32
    HammerPreset{5.94916213618e+14, 234050607.738, 0.0173613736074, 71.8583331437, 3.87379692279, 1.72886116894, 2.07952324174, 1.42872370063, 0.0101289, -0.000152232643808},  // MIDI 33
    HammerPreset{5.47166894428e+15, 1227205414.64, 0.0422979634125, 69.2404172645, 4.16172584334, 1.51888066202, 2.42534408947, 1.31816746731, 0.0100576, -0.000134955265835},  // MIDI 34
    HammerPreset{2.99308168484e+17, 9978375029.65, 0.348537008845, 73.508323946, 4.63313341362, 1.04557661809, 2.524521658, 1.40454740355, 0.0099865, -0.000129499528745},  // MIDI 35
    HammerPreset{1.80789786066e+16, 2939909865.95, 0.132646171183, 61.5460575085, 4.28129183158, 0.822177378051, 2.36751717769, 1.31846574807, 0.0099156, -0.00011467998292},  // MIDI 36
    HammerPreset{1.11161451847e+17, 7948661983.11, 0.0611758474368, 67.046651141, 4.48286761069, 0.960186364174, 2.42864301657, 1.47701212091, 0.0098449, -0.00015593603103},  // MIDI 37
    HammerPreset{4.3873299959e+16, 14066672533.3, 0.602356114745, 77.2698922774, 4.33937569638, 1.10145575138, 2.48473433962, 1.46390632598, 0.0097744, -0.000142140399416},  // MIDI 38
    HammerPreset{6.64036522952e+15, 10885944735.2, 0.482796352246, 118.409198106, 4.1169797795, 1.92069137648, 2.60414154691, 1.82924643484, 0.0097041, -0.000106101910165},  // MIDI 39
    HammerPreset{7.52015082932e+15, 1461948234.27, 0.329085795901, 81.9724928385, 4.14838456475, 2.07168784066, 2.31923999311, 1.52963689192, 0.009634, -0.000139245873089},  // MIDI 40
    HammerPreset{3.63842757684e+17, 9320916907.38, 0.218811062133, 55.2565646144, 4.61479726558, 2.26048438152, 2.46487321948, 1.28640617819, 0.0095641, -0.000143197277015},  // MIDI 41
    HammerPreset{3.58403288348e+16, 8565007519.99, 0.927386311232, 102.433280027, 4.28916483916, 1.9945573967, 2.51191038189, 1.7792231948, 0.0094944, -0.0001224398261},  // MIDI 42
    HammerPreset{8.23076317641e+16, 649778394.407, 2.28641234541, 83.7367590475, 4.42359679766, 2.19690143581, 2.32083997998, 1.43902473264, 0.0094249, -0.000122482726022},  // MIDI 43
    HammerPreset{3.3979801765e+16, 818055823.337, 0.8490773077, 91.795290029, 4.28817491186, 2.0979330401, 2.23498494235, 1.67794392947, 0.0093556, -0.00011048420585},  // MIDI 44
    HammerPreset{6.85316129755e+16, 24537212.608, 0.0990729420225, 89.570297158, 4.36089984709, 2.19478368016, 1.87339784481, 1.70747583916, 0.0092865, -9.86116088299e-05},  // MIDI 45
    HammerPreset{1.14836898136e+15, 1106156.11785, 0.0206232866243, 104.996871955, 3.86466527428, 1.95457331721, 1.50260124357, 1.69572915827, 0.0092176, -0.000150059205021},  // MIDI 46
    HammerPreset{5.9386297485e+15, 615012.218485, 0.00395170464125, 109.687897491, 4.08416674014, 1.70329086095, 1.42638327433, 1.75936700139, 0.0091489, -0.000126863498162},  // MIDI 47
    HammerPreset{8.40867323391e+16, 16886379.6639, 0.0154874576752, 75.1823009166, 4.37897395449, 1.29985707264, 1.81187361598, 1.53113952189, 0.0090804, -8.25289782926e-05},  // MIDI 48
    HammerPreset{1.75075561354e+17, 695572536.641, 0.0295452819837, 68.2299094554, 4.46578984111, 1.54091858355, 2.24949569842, 1.44845846783, 0.0090121, -9.06305407311e-05},  // MIDI 49
    HammerPreset{1.95553212497e+16, 501889284.916, 0.0268602111684, 77.7457099064, 4.20698732544, 1.84179000891, 2.24347524952, 1.51827359521, 0.008944, -0.000116152020079},  // MIDI 50
    HammerPreset{6.80773669038e+14, 830099507.144, 0.162766497881, 152.21452683, 3.78442648176, 1.31053598299, 2.30096661871, 1.92111512139, 0.0088761, -9.61739005427e-05},  // MIDI 51
    HammerPreset{1.86249577319e+16, 6878735053.43, 0.0310467483149, 77.0641796563, 4.19422447696, 1.73461964107, 2.52028759044, 1.47872785955, 0.0088084, -0.000101569192447},  // MIDI 52
    HammerPreset{3.08480440022e+16, 1434321835.9, 0.0133523040884, 84.0466555179, 4.23548215362, 2.19637389171, 2.28998871888, 1.60544471869, 0.0087409, -9.2426268564e-05},  // MIDI 53
    HammerPreset{4.03736424957e+16, 2608087797.63, 0.00462598413432, 79.4944705655, 4.24872957473, 1.89452471514, 2.33777731055, 1.66095134083, 0.0086736, -9.252302921e-05},  // MIDI 54
    HammerPreset{1.58658823971e+17, 1537489440.88, 0.0468354619328, 93.9515075811, 4.42983583775, 1.16967583018, 2.31486853522, 1.69043809226, 0.0086065, -9.23118369882e-05},  // MIDI 55
    HammerPreset{1.84240793618e+16, 2159031010.05, 0.0111939988424, 86.3877695931, 4.15564296003, 1.28349079737, 2.35904250499, 1.64937717906, 0.0085396, -0.00010664590568},  // MIDI 56
    HammerPreset{3.17279131622e+17, 284003292.833, 0.0265099644602, 57.7681478094, 4.51290967527, 1.15477107304, 2.05994848584, 1.31804440641, 0.0084729, -0.000116107345643},  // MIDI 57
    HammerPreset{3.01517714711e+18, 2108863.39846, 0.00307640867698, 67.1211212683, 4.76784670986, 1.17244891847, 1.55611459377, 1.48555840795, 0.0084064, -8.72959524805e-05},  // MIDI 58
    HammerPreset{7.40364748179e+17, 1366651.53153, 0.0135924407086, 101.04703182, 4.57979593098, 0.863547487894, 1.47063604581, 1.81417028644, 0.0083401, -9.08197687832e-05},  // MIDI 59
    HammerPreset{2.69668710128e+17, 6320896.86965, 0.0153778449451, 91.8014437053, 4.45649895447, 1.18555507615, 1.72364702433, 1.68364822372, 0.008274, -0.000100024312038},  // MIDI 60
    HammerPreset{7.01022741856e+16, 1495587.71863, 0.0187125258374, 77.6207155999, 4.30684825465, 0.880924261839, 1.5523973248, 1.54502162314, 0.0082081, -5.94637994639e-05},  // MIDI 61
    HammerPreset{3.79992684818e+17, 54382408.4461, 0.131274471917, 111.112906718, 4.48263033949, 0.93591460207, 1.86966028203, 1.81767717585, 0.0081424, -7.58998294437e-05},  // MIDI 62
    HammerPreset{8.87112318882e+17, 305565116.86, 0.0767186834196, 63.5116681676, 4.61057339598, 0.970558875582, 2.16273983112, 1.35689462368, 0.0080769, -5.61894732513e-05},  // MIDI 63
    HammerPreset{3.85890821265e+17, 357365011.642, 0.0361168622469, 83.7804616143, 4.49485645864, 1.24692341398, 2.26275070249, 1.57643956966, 0.0080116, -7.25966743016e-05},  // MIDI 64
    HammerPreset{9.23149354767e+17, 931989055.868, 0.386288934707, 89.2485834017, 4.5847289672, 1.6960394939, 2.3645157475, 1.63462513089, 0.0079465, -6.4849644817e-05},  // MIDI 65
    HammerPreset{2.29849323929e+17, 65002137.8398, 0.0779058794289, 75.688103673, 4.42215902784, 1.68132291708, 2.13943670296, 1.52560293439, 0.0078816, -7.05457166508e-05},  // MIDI 66
    HammerPreset{1.58001184157e+19, 78506929.8641, 0.0469724246026, 60.2836006408, 4.93635670466, 1.66168238879, 1.95738624106, 1.38694441485, 0.0078169, -7.13536785106e-05},  // MIDI 67
    HammerPreset{5.52945049014e+19, 512003117.553, 0.0122250676744, 62.8529001507, 5.06533567608, 1.98084421652, 2.26332921528, 1.44539645386, 0.0077524, -5.47052854692e-05},  // MIDI 68
    HammerPreset{2.66411818534e+20, 400898669.294, 0.00495141283471, 51.5446398547, 5.24726355653, 1.89297802452, 2.0817456657, 1.28229569878, 0.0076881, -6.62128909781e-05},  // MIDI 69
    HammerPreset{2.0470731423e+18, 3885808.26328, 0.00341396395117, 67.022264337, 4.65826508365, 2.22742559166, 1.63940639635, 1.53679517104, 0.007624, -6.28707126703e-05},  // MIDI 70
    HammerPreset{3.34948590137e+18, 12952701.5597, 0.0097176651895, 63.4140424471, 4.73515719862, 1.85074888271, 1.78488722286, 1.40682362896, 0.0075601, -6.39758013627e-05},  // MIDI 71
    HammerPreset{6.99713810569e+18, 89772122.2919, 0.125955286852, 63.9844916791, 4.79977526118, 1.50599270882, 2.07440713967, 1.45909911609, 0.0074964, -4.89610360494e-05},  // MIDI 72
    HammerPreset{2.03291992018e+20, 165614438.325, 0.2377033212, 65.0174809964, 5.21540875517, 1.52683837021, 2.019935665, 1.37547327547, 0.0074329, -7.99541636494e-05},  // MIDI 73
    HammerPreset{5.30882559405e+18, 10699726.875, 0.103146188937, 84.0632908434, 4.74640783061, 2.17085850623, 1.80337361018, 1.72149854665, 0.0073696, -4.26791095164e-05},  // MIDI 74
    HammerPreset{8.6103548347e+19, 49199367.119, 0.150880288497, 77.8692064912, 5.06802677928, 2.20383444454, 2.02828444257, 1.64897132571, 0.0073065, -5.20397478966e-05},  // MIDI 75
    HammerPreset{6.09898835753e+18, 140779974.598, 0.0201824689513, 81.9324344629, 4.77288305291, 1.93851085594, 2.1907378878, 1.58712308643, 0.0072436, -5.15129894092e-05},  // MIDI 76
    HammerPreset{2.87954360485e+20, 11809206407, 0.0136201524187, 79.780519705, 5.21177115036, 1.57758020434, 2.64144720702, 1.67035325939, 0.0071809, -6.00313913622e-05},  // MIDI 77
    HammerPreset{3.2107004926e+19, 484619209.124, 0.0219179598819, 69.1525817477, 4.94907749709, 1.70512358731, 2.34566083126, 1.52072666309, 0.0071184, -4.45263687638e-05},  // MIDI 78
    HammerPreset{6.68451838058e+18, 431658885.444, 0.00286483642128, 73.5452789079, 4.75309880571, 1.89821085522, 2.20768849561, 1.60597264682, 0.0070561, -6.34014896232e-05},  // MIDI 79
    HammerPreset{2.75958393866e+21, 1239715862.9, 0.00350949528286, 69.7554760276, 5.4750500309, 2.09530766973, 2.42229351194, 1.56574199757, 0.006994, -4.32489177768e-05},  // MIDI 80
    HammerPreset{7.518286448e+20, 610900905.43, 0.0156682731818, 66.1106585821, 5.30004577637, 2.06663798191, 2.4814627321, 1.5494106736, 0.0069321, -4.0318126869e-05},  // MIDI 81
    HammerPreset{7.62179737645e+18, 852725158.576, 0.0216623219946, 78.8231914797, 4.76360601146, 2.12292249894, 2.48761900103, 1.60263733894, 0.0068704, -5.45801703171e-05},  // MIDI 82
    HammerPreset{2.12794580884e+18, 27623951.7227, 0.0305077782504, 78.2913899486, 4.5947127491, 1.541871116, 1.97014555643, 1.61340296991, 0.0068089, -5.92707444379e-05},  // MIDI 83
    HammerPreset{1.16871965036e+20, 51539825.8653, 0.0810568058881, 80.789083344, 5.07655295168, 1.35504019548, 1.98314208378, 1.64017842455, 0.0067476, -5.39880996761e-05},  // MIDI 84
    HammerPreset{3.24137404567e+20, 5072063.73559, 0.568468257758, 61.57318377, 5.18910506974, 1.48539430692, 1.72983151503, 1.45804461915, 0.0066865, -5.78325673e-05},  // MIDI 85
    HammerPreset{4.00751304778e+22, 146924550.932, 0.0443016654732, 57.5921579617, 5.74595753395, 1.25195520644, 2.15930930931, 1.4821114199, 0.0066256, -3.52198499655e-05},  // MIDI 86
    HammerPreset{9.46768625127e+20, 1329432732.41, 0.035283372649, 57.1182786246, 5.31012438602, 1.58670898436, 2.42082539278, 1.39122194855, 0.0065649, -4.93889624351e-05},  // MIDI 87
    HammerPreset{4.54970804487e+19, 332918846.218, 0.0259927209181, 93.8738585264, 4.91629767066, 1.26622815242, 2.24120238765, 1.8378597154, 0.0065044, -4.8388438852e-05},  // MIDI 88
    HammerPreset{8.42939926719e+17, 47775187.8263, 0.0212259118121, 73.8820979692, 4.49623532243, 1.90688037593, 2.13590236617, 1.57428882706, 0.0064441, -4.89430745838e-05},  // MIDI 89
    HammerPreset{3.71241760075e+19, 3344156.26191, 0.0601518293813, 80.4405827537, 4.91046527265, 1.57502390008, 1.7726230429, 1.65666970099, 0.006384, -3.95109830121e-05},  // MIDI 90
    HammerPreset{8.38522332429e+19, 89963.4880359, 0.115503487942, 72.2407673436, 4.99936010094, 1.24346029204, 1.39562707666, 1.60284429297, 0.0063241, -4.20363373062e-05},  // MIDI 91
    HammerPreset{8.1599422871e+20, 2994947.65869, 0.00597002740455, 53.5205451065, 5.29016624252, 1.83041950294, 1.75059841621, 1.34529935649, 0.0062644, -5.05389212316e-05},  // MIDI 92
    HammerPreset{1.64528268705e+21, 47423786.1228, 0.0341444737295, 59.6540445007, 5.35378023914, 1.78423223013, 1.91036077599, 1.43410727222, 0.0062049, -4.46456914795e-05},  // MIDI 93
    HammerPreset{4.55748742769e+20, 76260437.7861, 0.113895241839, 69.3773930113, 5.19087473149, 1.75362498278, 2.03097593981, 1.55844130702, 0.0061456, -4.25945959224e-05},  // MIDI 94
    HammerPreset{5.42368592292e+21, 406429.801268, 0.325306171305, 78.967283677, 5.43977089541, 1.64976720327, 1.51653963048, 1.77028353264, 0.0060865, -3.66512463134e-05},  // MIDI 95
    HammerPreset{2.69009076545e+21, 1348183.87672, 0.0629505455017, 70.9960153032, 5.37811721796, 1.90756606314, 1.6203009308, 1.62802544856, 0.0060276, -4.30352919142e-05},  // MIDI 96
    HammerPreset{3.80159652288e+20, 5830266.06929, 0.0110490651358, 81.7787329807, 5.12350303094, 1.6938407938, 1.78426337346, 1.82868902896, 0.0059689, -3.45655368927e-05},  // MIDI 97
    HammerPreset{5.45140735911e+19, 247318.681515, 0.0203426337268, 63.7844932619, 4.92433852586, 1.79391889099, 1.44966098995, 1.57035165751, 0.0059104, -4.76855458406e-05},  // MIDI 98
    HammerPreset{1.15906857342e+20, 7136604.90082, 0.0275828672486, 99.2710201841, 4.99431774514, 1.99546195276, 1.83179261041, 1.84885339967, 0.0058521, -5.77172394249e-05},  // MIDI 99
    HammerPreset{4.69015845548e+20, 63244978.2904, 0.0633674431757, 69.5119732568, 5.15068922668, 2.06318440708, 2.11336602306, 1.6648003566, 0.005794, -3.75561240567e-05},  // MIDI 100
    HammerPreset{7.22285784332e+20, 174231642.895, 0.00493062277846, 83.1349974469, 5.19944125144, 1.97588801451, 2.22014410961, 1.73027686866, 0.0057361, -3.97668413356e-05},  // MIDI 101
    HammerPreset{5.60286217725e+20, 116511694.151, 0.00842675675405, 77.3752303013, 5.16264458347, 1.27033964, 2.23849777832, 1.68480765862, 0.0056784, -3.89291593564e-05},  // MIDI 102
    HammerPreset{1.58998921391e+19, 10198182060.7, 0.0794004699382, 94.0330025781, 4.7494907978, 1.28292650364, 2.6595467747, 1.84365668086, 0.0056209, -3.28062357347e-05},  // MIDI 103
    HammerPreset{1.95479056967e+20, 277870749.672, 0.0357174964528, 91.0184821, 5.0294422286, 1.51648833519, 2.25808271842, 1.81251568537, 0.0055636, -3.65732605707e-05},  // MIDI 104
    HammerPreset{5.29008842315e+20, 33989225.639, 0.00545791120395, 86.7454329897, 5.14072781649, 1.32959930804, 2.0896802887, 1.71433288236, 0.0055065, -6.28285973846e-05},  // MIDI 105
    HammerPreset{2.55630605508e+22, 57085259.2244, 0.0309389687422, 63.0303939658, 5.58842075436, 1.76256088718, 2.16874780095, 1.60941254252, 0.0054496, -3.93350768256e-05},  // MIDI 106
    HammerPreset{5.40371453112e+21, 80671319.5347, 0.11835275404, 54.7815359372, 5.41288357537, 1.82050754398, 2.1023839908, 1.5076551051, 0.0053929, -5.68561651874e-05},  // MIDI 107
    HammerPreset{9.23752411116e+21, 6308262.87978, 0.0677048221337, 50.6273356288, 5.47204979767, 2.13194835042, 1.85308941552, 1.40797452509, 0.0053364, -4.21693002932e-05},  // MIDI 108
}};

}  // namespace Parameters::Hammer::LegacyFit


class HammerPresets {
    HammerPresets(const HammerPresets&) = delete;
    HammerPresets& operator=(const HammerPresets&) = delete;

public:
    using Preset =
        Parameters::Hammer::LegacyFit::HammerPreset;

    using PresetTable =
        std::array<
            Preset,
            88
        >;

    constexpr HammerPresets() noexcept = default;


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
            Parameters::Hammer::LegacyFit::kHammerPresets;
    }
};


#endif /* hammer_preset_hpp */

class Configuration {
    Configuration(const Configuration&) = delete;
    Configuration& operator=(const Configuration&) = delete;
public:
    
    static constexpr TunningPresets tuning_presets{};
    static constexpr HammerPresets hammer_presets{};

    constexpr Configuration() noexcept = default;
    
};



#endif /* configuration_hpp */

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

#include <condition_variable>
#include <cstdint>
#include <filesystem>
#include <iostream>
#include <mutex>



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
  explicit MidiKeyboard(MidiInputHub& input_hub) noexcept
      : input_hub_(input_hub) {}

  MidiKeyboard(const MidiKeyboard&) = delete;
//  MidiKeyboard& operator=(const MidiKeyboard&) = delete;
  MidiKeyboard(MidiKeyboard&&) = delete;
  MidiKeyboard& operator=(MidiKeyboard&&) = delete;

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
    subscription_
        ->reset();  // Synchronizes with an in-flight CoreMIDI callback.
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

  MidiInputHub& input_hub_;
  std::optional<MidiInputHub::Subscription> subscription_;
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
        ActiveNote* free_slot = nullptr;
        for (ActiveNote& active_note : active_notes_) {
            if (active_note.active && active_note.note == note) {
//                note_off(note, 0.0);
                free_slot = &active_note;
                break;
            }
            if (!active_note.active && free_slot == nullptr) {
                free_slot = &active_note;
            }
        }
        if (free_slot == nullptr) {
            // The terminal map exposes at most 30 simultaneous notes. This branch is
            // defensive and avoids allocation in the input loop.
            return;
        }

        note_on(note, velocity);
        
        print_line_two_status("NoteOn: MIDI ", note, ", velocity ", velocity);
        
        free_slot->note = note;
        free_slot->release_time =
        Clock::now() + std::chrono::milliseconds(note_duration_ms_.load());
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

    std::atomic<int> note_duration_ms_{650};
    int base_note_ = 60;
    std::array<ActiveNote, 30> active_notes_{};
    bool soft_pedal_down_ = false;
    bool harmonic_pedal_down_ = false;
    bool sostenuto_pedal_down_ = false;
    bool sustain_pedal_down_ = false;
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


namespace {

void wait_for_stop(std::stop_token stop_token) {
    std::mutex mutex;
    std::condition_variable_any condition;
    std::unique_lock lock(mutex);
    condition.wait(lock, stop_token, [] { return false; });
}

}

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
void midi_service(const std::string& midi_path, std::stop_token stop_token) {
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
  std::cout << "Test service started (Ctrl-C exits).\n";
  wait_for_stop(stop_token);
}
void internal_test_service(std::stop_token stop_token) {
    std::cout << "Internal test started (Ctrl-C exits).\n";
    sustainpedal_control(0.6);
    note_on(69, 128.0);
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
    
    FractionalFilter(double frac) {
        a1 = (1 - frac) / (1 + frac);
    }
    
    inline void process(float &x) {
         // y = a1 * x + x1 - a1 * y1;
         float y = static_cast<float>(a1 * x)
             + static_cast<float>(x1)
             - static_cast<float>(a1 * y1);
         x1 = x;
         y1 = y;
         x = y;
     }
    
    inline void system_reset() {
        x1 = 0.0;
        y1 = 0.0;
    }
};




#endif /* fractional_filter_hpp */
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
// 2026-04-06
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


#ifndef damper_model_hpp
#define damper_model_hpp

#include <iostream>


class Damper {
    
    double z1 = 0.0;
    double z2 = 0.0;

public:
    
    inline void process(float &x) {
        // ==========================
        // Temporary Damper Controls
        // ==========================
        constexpr float lowLoss   = 0.020; // 低频耗散：0.020~0.070
        constexpr float highLoss  = 0.25;  // 高频抓取：0.25~0.70
        constexpr float damperMix = 0.38;  // 毛毡低通占比：0.30~0.70

        const float loopGain = 1.0 - lowLoss;
        const float wet = damperMix * highLoss;
        const float dry = 1.0 - wet;

        // Gentle second-order low-pass damper color.
        constexpr float b0 = 0.292893218813;
        constexpr float b1 = 0.585786437627;
        constexpr float b2 = 0.292893218813;

        constexpr float a1 = 0.000000000000;
        constexpr float a2 = 0.171572875254;

        const float y = b0 * x + z1;

        z1 = b1 * x - a1 * y + z2;
        z2 = b2 * x - a2 * y;

        x = loopGain * (dry * x + wet * y);
    }
    
    inline void system_reset() noexcept {
      z1 = 0.0;
      z2 = 0.0;
    }
    
};




#endif /* damper_model_hpp */
//
//  tunning_preset.cpp
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

#ifndef hammer_model_hpp
#define hammer_model_hpp

#include <iostream>
#include <cmath>


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
    const Parameters::Hammer::LegacyFit::HammerPreset* hammer_presets = nullptr;
    static constexpr double compression_max_a = 1e-3;
    static constexpr double compression_max_b = 1e-3;
    
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

    
public:
    
    HammerModel(double sample_rate,
                int midi_n,
                const Configuration* configuration) :
        ts_(1.0 / sample_rate),
        midi_n_(midi_n),
        hammer_presets(configuration->hammer_presets.find_preset(midi_n_))
    {}
    
    inline double hammer_movement(double string_v) {
        if(!is_contacting_)
            return 0.0;
        
        // 算 f (解出 middle_v)
        double hammer_force = solve_f(string_v);
        // 击锤的空间移动
        if(hammer_v_ < 0 || release_distance < 0.0) {
            release_distance += hammer_v_ * ts_;
            if(release_distance > 0.0) {
                release_distance = 0.0;
            }
        }
        // 检查是否接触结束
        if(release_distance < hammer_presets->release_threshold_m) {
            system_reset();
            return 0.0;
        }
        
        // 更新压缩量
        w_a_1_ = w_a_1_ + (middle_v_ - string_v) * ts_;
        w_b_1_ = w_b_1_ + (hammer_v_ - middle_v_) * ts_;
        
        // 更新击锤的速度
        // 负力不更新！
        if(hammer_force > 0.0)
            hammer_v_ -= (hammer_force / hammer_presets->mass_kg) * ts_;
       
        
        return hammer_force;
    }
    
    inline void trigger(double hammer_v) {
        if(is_contacting_)
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
    inline double scope_pow_a(double x, double p) {
        if(x > compression_max_a)
            x = compression_max_a;
        return signed_pow(x, p);
    }
    inline double scope_pow_b(double x, double p) {
        if(x > compression_max_b)
            x = compression_max_b;
        return signed_pow(x, p);
    }
    
    inline double solve_f(double string_v) {
        double upper_limit = 20.0;
        double lower_limit = -20.0;
        double middle_v_suppose = (upper_limit + lower_limit) / 2.0;
        for(int i = 0; i < 20; i++) {
            
            double f_a = hammer_presets->k_a * scope_pow_a(w_a_1_, hammer_presets->p1) +
            hammer_presets->c_a *  signed_pow(middle_v_suppose - string_v, hammer_presets->p2);
            double f_b = hammer_presets->k_b * scope_pow_b(w_b_1_, hammer_presets->p3) +
            hammer_presets->c_b * signed_pow(hammer_v_ - middle_v_suppose, hammer_presets->p4);
            
            if(f_a - f_b > 0) {
                upper_limit = middle_v_suppose;
            } else {
                lower_limit = middle_v_suppose;
            }
            middle_v_suppose = (upper_limit + lower_limit) / 2.0;
        }
        middle_v_ = middle_v_suppose;
        
        
        
        if(compare_float_equal(std::abs(middle_v_suppose), 20.0f))
            std::cout<<"二分法无解\n";
        
        // 上层已经压到最大压缩量 上下层构成连续体 返回下层力-上层力
        if(compare_float_equal(middle_v_suppose, upper_limit) &&
           compare_float_equal(w_a_1_, compression_max_a)) {
            std::cout<<"上层已经压到最大压缩量\n";
            return hammer_presets->k_b * scope_pow_b(w_b_1_, hammer_presets->p3) +
            hammer_presets->c_b * signed_pow(hammer_v_ - middle_v_suppose, hammer_presets->p4) -
            (hammer_presets->k_a * scope_pow_a(w_a_1_, hammer_presets->p1) +
             hammer_presets->c_a *  signed_pow(middle_v_suppose - string_v, hammer_presets->p2));
        // 下层已经压到最大压缩量 上下层构成连续体 返回上层力-下层力
        } else if(compare_float_equal(middle_v_suppose, lower_limit) &&
                  compare_float_equal(w_b_1_, compression_max_b)) {
            std::cout<<"下层已经压到最大压缩量\n";
            return hammer_presets->k_a * scope_pow_a(w_a_1_, hammer_presets->p1) +
            hammer_presets->c_a *  signed_pow(middle_v_suppose - string_v, hammer_presets->p2) -
                    (hammer_presets->k_b * scope_pow_b(w_b_1_, hammer_presets->p3) +
                     hammer_presets->c_b * signed_pow(hammer_v_ - middle_v_suppose, hammer_presets->p4));
        } else {
            return hammer_presets->k_a * scope_pow_a(w_a_1_, hammer_presets->p1) +
            hammer_presets->c_a *  signed_pow(middle_v_suppose - string_v, hammer_presets->p2);
        }
        
        
        
    }
    
    inline bool compare_float_equal(float x, float y) {
        return std::abs(x - y) < 1e-5;
    }

};

#endif /* hammer_model_hpp */

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

#include <iostream>

class LossFilter {
public:
    
    inline static constexpr int kD274LossMidiMin = 21;
    inline static constexpr int kD274LossMidiMax = 108;
    inline static constexpr int kD274LossSectionCount = 4;
    
    struct LossBiquad {
        double b0 = 0.0;
        double b1 = 0.0;
        double b2 = 0.0;
        double a1 = 0.0;
        double a2 = 0.0;
    };
    struct LossPreset {
        int midi;
        double referenceF1;
        double B;
        double phaseDelaySamples;
        double groupDelaySamples;
        int sectionCount;
        std::array<LossBiquad, kD274LossSectionCount> sections;
    };
    
    LossPreset lossPreset;
    std::array<float, kD274LossSectionCount> x1{};
    std::array<float, kD274LossSectionCount> x2{};
    std::array<float, kD274LossSectionCount> y1{};
    std::array<float, kD274LossSectionCount> y2{};
    
    LossFilter(int midi_n) {
        const int m = std::clamp(midi_n, kD274LossMidiMin, kD274LossMidiMax);
        lossPreset = kD274LossPresets[
            static_cast<std::size_t>(m - kD274LossMidiMin)];
    }
    
    inline constexpr const LossPreset& getD274LossPreset(int midi) {
        const int m = std::clamp(
            midi, kD274LossMidiMin, kD274LossMidiMax);
        return kD274LossPresets[
            static_cast<std::size_t>(m - kD274LossMidiMin)];
    }

    inline constexpr const LossPreset& getD274LossPreset(
        int midi, int /*stringIndex*/) {
        return getD274LossPreset(midi);
    }
    
    
    inline void process(float& x) {
        
        const std::size_t sectionCount = std::min<std::size_t>(
            static_cast<std::size_t>(lossPreset.sectionCount),
            lossPreset.sections.size()
        );

        for (std::size_t i = 0; i < sectionCount; ++i) {
            const auto& c = lossPreset.sections[i];

            const float b0 = static_cast<float>(c.b0);
            const float b1 = static_cast<float>(c.b1);
            const float b2 = static_cast<float>(c.b2);
            const float a1 = static_cast<float>(c.a1);
            const float a2 = static_cast<float>(c.a2);

            // H(z) = (b0 + b1 z^-1 + b2 z^-2)
            //      / (1  + a1 z^-1 + a2 z^-2)
            // Difference equation:
            // y[n] = b0*x[n] + b1*x[n-1] + b2*x[n-2]
            //      - a1*y[n-1] - a2*y[n-2]

            const float y = b0 * x
                + b1 * x1[i]
                + b2 * x2[i]
                - a1 * y1[i]
                - a2 * y2[i];

            x2[i] = x1[i];
            x1[i] = x;

            y2[i] = y1[i];
            y1[i] = y;

            x = y;
        }
    }

    
    inline void system_reset() {
        x1.fill(0.0f);
        x2.fill(0.0f);
        y1.fill(0.0f);
        y2.fill(0.0f);
    }
    
    
    inline static constexpr std::array<LossPreset, 88> kD274LossPresets = {{
        // MIDI 21 A0; perceptual RMSE 0.00473177; compensated partial-cent RMS 0.112803
        LossPreset{
            21, 27.500752747255206, 5.4746004184965221e-05, 0.99990087000431871, 0.99860572014366777, 1, {{
                LossBiquad{0.85524455404366084, -1.4574578798127882, 0.62093426352466863, -1.724548608074796, 0.74352366709610784},
                LossBiquad{1.0, 0.0, 0.0, 0.0, 0.0},
                LossBiquad{1.0, 0.0, 0.0, 0.0, 0.0},
                LossBiquad{1.0, 0.0, 0.0, 0.0, 0.0}
            }}
        },
        // MIDI 22 A#0; perceptual RMSE 0.00467098; compensated partial-cent RMS 0.112779
        LossPreset{
            22, 29.136014703845198, 5.3517288776830332e-05, 0.93504707674792753, 0.93382238178436761, 1, {{
                LossBiquad{0.85770692127969217, -1.4492543424992075, 0.6122012660443078, -1.7107044502407514, 0.73163401370474712},
                LossBiquad{1.0, 0.0, 0.0, 0.0, 0.0},
                LossBiquad{1.0, 0.0, 0.0, 0.0, 0.0},
                LossBiquad{1.0, 0.0, 0.0, 0.0, 0.0}
            }}
        },
        // MIDI 23 B0; perceptual RMSE 0.00461611; compensated partial-cent RMS 0.112899
        LossPreset{
            23, 30.868516860440025, 5.2517188366529486e-05, 0.8748355316821087, 0.87367587198799346, 1, {{
                LossBiquad{0.86015165920358638, -1.4405113590468699, 0.6031179586496469, -1.6963531920572441, 0.71941001272139082},
                LossBiquad{1.0, 0.0, 0.0, 0.0, 0.0},
                LossBiquad{1.0, 0.0, 0.0, 0.0, 0.0},
                LossBiquad{1.0, 0.0, 0.0, 0.0, 0.0}
            }}
        },
        // MIDI 24 C1; perceptual RMSE 0.0045699; compensated partial-cent RMS 0.113136
        LossPreset{
            24, 32.704041582192367, 5.1733816278101171e-05, 0.81893240468839823, 0.81783313890174492, 1, {{
                LossBiquad{0.86255861962049163, -1.4311436248975087, 0.59363806185289203, -1.6814442562985892, 0.70682005812131798},
                LossBiquad{1.0, 0.0, 0.0, 0.0, 0.0},
                LossBiquad{1.0, 0.0, 0.0, 0.0, 0.0},
                LossBiquad{1.0, 0.0, 0.0, 0.0, 0.0}
            }}
        },
        // MIDI 25 C#1; perceptual RMSE 0.00453685; compensated partial-cent RMS 0.113436
        LossPreset{
            25, 34.648715116965342, 5.1157963985472533e-05, 0.76703196787656458, 0.76598942660794456, 1, {{
                LossBiquad{0.86489226299101574, -1.4209914435949118, 0.58366658549069195, -1.6658763290910472, 0.69379223006780388},
                LossBiquad{1.0, 0.0, 0.0, 0.0, 0.0},
                LossBiquad{1.0, 0.0, 0.0, 0.0, 0.0},
                LossBiquad{1.0, 0.0, 0.0, 0.0, 0.0}
            }}
        },
        // MIDI 26 D1; perceptual RMSE 0.00452425; compensated partial-cent RMS 0.113704
        LossPreset{
            26, 36.709028050002956, 5.0782920493655748e-05, 0.71885670071086694, 0.71786845472387029, 1, {{
                LossBiquad{0.86709517948818959, -1.4097797517925814, 0.57303306574583635, -1.6494652698304613, 0.68019004078622103},
                LossBiquad{1.0, 0.0, 0.0, 0.0, 0.0},
                LossBiquad{1.0, 0.0, 0.0, 0.0, 0.0},
                LossBiquad{1.0, 0.0, 0.0, 0.0, 0.0}
            }}
        },
        // MIDI 27 D#1; perceptual RMSE 0.00454556; compensated partial-cent RMS 0.112393
        LossPreset{
            27, 38.891856976326189, 5.0604341825923305e-05, 0.67263918255781652, 0.67171770326604963, 1, {{
                LossBiquad{0.86865499988017369, -1.3944742249331028, 0.55965137638110207, -1.6299180842727934, 0.6641642178199012},
                LossBiquad{1.0, 0.0, 0.0, 0.0, 0.0},
                LossBiquad{1.0, 0.0, 0.0, 0.0, 0.0},
                LossBiquad{1.0, 0.0, 0.0, 0.0, 0.0}
            }}
        },
        // MIDI 28 E1; perceptual RMSE 0.00461483; compensated partial-cent RMS 0.108512
        LossPreset{
            28, 41.204487463536495, 5.0620167059091969e-05, 0.62767739938064815, 0.62684163152552208, 1, {{
                LossBiquad{0.86918822246093452, -1.3724500534556132, 0.5417802649727671, -1.6051190239861004, 0.64410756724123719},
                LossBiquad{1.0, 0.0, 0.0, 0.0, 0.0},
                LossBiquad{1.0, 0.0, 0.0, 0.0, 0.0},
                LossBiquad{1.0, 0.0, 0.0, 0.0, 0.0}
            }}
        },
        // MIDI 29 F1; perceptual RMSE 0.004754; compensated partial-cent RMS 0.103098
        LossPreset{
            29, 43.654638382094525, 5.0830578631877072e-05, 0.58558296399633225, 0.58483914228126765, 1, {{
                LossBiquad{0.86882497360235134, -1.3443365505360123, 0.52002908562762529, -1.5754684420802127, 0.62053078775799864},
                LossBiquad{1.0, 0.0, 0.0, 0.0, 0.0},
                LossBiquad{1.0, 0.0, 0.0, 0.0, 0.0},
                LossBiquad{1.0, 0.0, 0.0, 0.0, 0.0}
            }}
        },
        // MIDI 30 F#1; perceptual RMSE 0.00500247; compensated partial-cent RMS 0.0954476
        LossPreset{
            30, 46.250487684802032, 5.1238005854742099e-05, 0.54632626223306779, 0.54568395456538099, 1, {{
                LossBiquad{0.86695901898078509, -1.3063904046800647, 0.49214299986223864, -1.5380103434085513, 0.59137427646044349},
                LossBiquad{1.0, 0.0, 0.0, 0.0, 0.0},
                LossBiquad{1.0, 0.0, 0.0, 0.0, 0.0},
                LossBiquad{1.0, 0.0, 0.0, 0.0, 0.0}
            }}
        },
        // MIDI 31 G1; perceptual RMSE 0.00541637; compensated partial-cent RMS 0.0846908
        LossPreset{
            31, 49.000699722172065, 5.1847171715901196e-05, 0.51032302998256895, 0.50979464579434741, 1, {{
                LossBiquad{0.86243105251553032, -1.2516725210665922, 0.45415172484367605, -1.4872506623678001, 0.55298361001592466},
                LossBiquad{1.0, 0.0, 0.0, 0.0, 0.0},
                LossBiquad{1.0, 0.0, 0.0, 0.0, 0.0},
                LossBiquad{1.0, 0.0, 0.0, 0.0, 0.0}
            }}
        },
        // MIDI 32 G#1; perceptual RMSE 0.00606116; compensated partial-cent RMS 0.0700184
        LossPreset{
            32, 51.914454185646264, 5.2665184247641983e-05, 0.47935375409376169, 0.47895194184990197, 1, {{
                LossBiquad{0.85274887554407564, -1.1663392460452988, 0.39881597005805564, -1.4122970195833964, 0.49865020571942409},
                LossBiquad{1.0, 0.0, 0.0, 0.0, 0.0},
                LossBiquad{1.0, 0.0, 0.0, 0.0, 0.0},
                LossBiquad{1.0, 0.0, 0.0, 0.0, 0.0}
            }}
        },
        // MIDI 33 A1; perceptual RMSE 0.00697631; compensated partial-cent RMS 0.0513263
        LossPreset{
            33, 55.001476776234526, 5.3701674930605477e-05, 0.45983655566041376, 0.4595679834782862, 1, {{
                LossBiquad{0.83143852538203611, -1.0194311940941201, 0.31248531340016855, -1.2893541627758245, 0.41561227976613324},
                LossBiquad{1.0, 0.0, 0.0, 0.0, 0.0},
                LossBiquad{1.0, 0.0, 0.0, 0.0, 0.0},
                LossBiquad{1.0, 0.0, 0.0, 0.0, 0.0}
            }}
        },
        // MIDI 34 A#1; perceptual RMSE 0.00809171; compensated partial-cent RMS 0.0302903
        LossPreset{
            34, 58.272071702138966, 5.4968987913623004e-05, 0.47813665915292708, 0.47799297926856432, 1, {{
                LossBiquad{0.77524336849611453, -0.72725174851842356, 0.17055930024710272, -1.05772487477488, 0.27969799495378661},
                LossBiquad{1.0, 0.0, 0.0, 0.0, 0.0},
                LossBiquad{1.0, 0.0, 0.0, 0.0, 0.0},
                LossBiquad{1.0, 0.0, 0.0, 0.0, 0.0}
            }}
        },
        // MIDI 35 B1; perceptual RMSE 0.00912949; compensated partial-cent RMS 0.0103137
        LossPreset{
            35, 61.737156115312786, 5.648242525940063e-05, 0.73105899462071122, 0.73101442764413593, 1, {{
                LossBiquad{0.52713822478512429, -2.2638141294984311e-17, 2.4305297433952198e-34, -0.53538039280963146, 0.071658686177489633},
                LossBiquad{1.0, 0.0, 0.0, 0.0, 0.0},
                LossBiquad{1.0, 0.0, 0.0, 0.0, 0.0},
                LossBiquad{1.0, 0.0, 0.0, 0.0, 0.0}
            }}
        },
        // MIDI 36 C2; perceptual RMSE 0.0103757; compensated partial-cent RMS 0.00327544
        LossPreset{
            36, 65.408296603730406, 5.8260555043704514e-05, 0.88320510112563344, 0.88322034188138276, 1, {{
                LossBiquad{0.42039073342913613, -8.4861120607071809e-18, 5.6650318337393614e-35, -0.76630629035406705, 0.19419710231333145},
                LossBiquad{1.0, 0.0, 0.0, 0.0, 0.0},
                LossBiquad{1.0, 0.0, 0.0, 0.0, 0.0},
                LossBiquad{1.0, 0.0, 0.0, 0.0, 0.0}
            }}
        },
        // MIDI 37 C#2; perceptual RMSE 0.0121843; compensated partial-cent RMS 0.0213119
        LossPreset{
            37, 69.29774786345007, 6.0325590997685519e-05, 0.96286525218389529, 0.96295161932147366, 1, {{
                LossBiquad{0.36928514752976388, -1.0413780964603002e-17, 9.7994226851961131e-35, -0.88535291592647636, 0.26156419740035541},
                LossBiquad{1.0, 0.0, 0.0, 0.0, 0.0},
                LossBiquad{1.0, 0.0, 0.0, 0.0, 0.0},
                LossBiquad{1.0, 0.0, 0.0, 0.0, 0.0}
            }}
        },
        // MIDI 38 D2; perceptual RMSE 0.0148184; compensated partial-cent RMS 0.0446783
        LossPreset{
            38, 73.418493682383158, 6.2703854560252503e-05, 1.0076043040363933, 1.0077738348986749, 1, {{
                LossBiquad{0.33807533049917304, -2.5843098569112109e-19, 6.5567651397817594e-38, -0.9628619911193993, 0.30771029566440372},
                LossBiquad{1.0, 0.0, 0.0, 0.0, 0.0},
                LossBiquad{1.0, 0.0, 0.0, 0.0, 0.0},
                LossBiquad{1.0, 0.0, 0.0, 0.0, 0.0}
            }}
        },
        // MIDI 39 D#2; perceptual RMSE 0.0187321; compensated partial-cent RMS 0.0752846
        LossPreset{
            39, 77.784290376099392, 6.5426332784645604e-05, 1.0321750116315993, 1.0324435905345239, 1, {{
                LossBiquad{0.3159774413283522, -4.1518020798592777e-23, 1.799610106258456e-45, -1.0210775685682199, 0.34393683326906593},
                LossBiquad{1.0, 0.0, 0.0, 0.0, 0.0},
                LossBiquad{1.0, 0.0, 0.0, 0.0, 0.0},
                LossBiquad{1.0, 0.0, 0.0, 0.0, 0.0}
            }}
        },
        // MIDI 40 E2; perceptual RMSE 0.0246988; compensated partial-cent RMS 0.115307
        LossPreset{
            40, 82.409712825063934, 6.8529348623202173e-05, 1.0427609028546918, 1.0431477965243854, 1, {{
                LossBiquad{0.29900215671173597, -6.245442704809064e-19, 4.280953456170851e-37, -1.0683605249365391, 0.37456227620912297},
                LossBiquad{1.0, 0.0, 0.0, 0.0, 0.0},
                LossBiquad{1.0, 0.0, 0.0, 0.0, 0.0},
                LossBiquad{1.0, 0.0, 0.0, 0.0, 0.0}
            }}
        },
        // MIDI 41 F2; perceptual RMSE 0.0340046; compensated partial-cent RMS 0.166934
        LossPreset{
            41, 87.310203272500118, 7.2055363822390889e-05, 1.0425937299198396, 1.0431202231042724, 1, {{
                LossBiquad{0.28544344513483461, -2.2758168323175716e-20, 5.9295487195682841e-40, -1.1081794803326106, 0.40131677562620671},
                LossBiquad{1.0, 0.0, 0.0, 0.0, 0.0},
                LossBiquad{1.0, 0.0, 0.0, 0.0, 0.0},
                LossBiquad{1.0, 0.0, 0.0, 0.0, 0.0}
            }}
        },
        // MIDI 42 F#2; perceptual RMSE 0.0486829; compensated partial-cent RMS 0.232193
        LossPreset{
            42, 92.502123052696234, 7.6053939151254118e-05, 1.0341237094936899, 1.0348114642827715, 1, {{
                LossBiquad{0.27448141843685259, -1.4772202804621997e-21, 2.5896681074631917e-42, -1.1420216369780192, 0.42483002144534987},
                LossBiquad{1.0, 0.0, 0.0, 0.0, 0.0},
                LossBiquad{1.0, 0.0, 0.0, 0.0, 0.0},
                LossBiquad{1.0, 0.0, 0.0, 0.0, 0.0}
            }}
        },
        // MIDI 43 G2; perceptual RMSE 0.0716376; compensated partial-cent RMS 0.31321
        LossPreset{
            43, 98.002807431147573, 8.0582882152181823e-05, 1.0197964868636062, 1.0206670158470608, 1, {{
                LossBiquad{0.26556817790895781, -5.1631392110548553e-23, 3.2618368541082098e-45, -1.1708125142448689, 0.44543599721615779},
                LossBiquad{1.0, 0.0, 0.0, 0.0, 0.0},
                LossBiquad{1.0, 0.0, 0.0, 0.0, 0.0},
                LossBiquad{1.0, 0.0, 0.0, 0.0, 0.0}
            }}
        },
        // MIDI 44 G#2; perceptual RMSE 0.0282883; compensated partial-cent RMS 0.0446488
        LossPreset{
            44, 103.83062375058964, 8.5709619281934343e-05, 1.2001283873396378, 1.2002372133204768, 3, {{
                LossBiquad{0.90683565959872947, -0.7921685244027199, 0.78007241946289874, -0.76504465300753921, 0.65978420766644752},
                LossBiquad{0.40097431278483964, 0.52482150620294377, 0.20662999721244549, 0.12753328328581018, 0.0048925329144187844},
                LossBiquad{0.86838415192402452, -0.34505419980556035, 0.61305279839910587, -0.31920360565322414, 0.45558635617079413},
                LossBiquad{1.0, 0.0, 0.0, 0.0, 0.0}
            }}
        },
        // MIDI 45 A2; perceptual RMSE 0.0182602; compensated partial-cent RMS 0.135112
        LossPreset{
            45, 110.00503309091708, 9.1512837500875427e-05, 1.3930405979823997, 1.3933456773450001, 3, {{
                LossBiquad{0.74692683079775435, -0.47989610666528099, 0.60014189307431931, -0.18110890979495672, 0.063843572899267523},
                LossBiquad{0.51341335424685397, 0.28330812556023444, 0.20327852019291148, 1.1291358221782439e-43, 1.6578031534344206e-86},
                LossBiquad{0.60118050511203036, -6.2841561442582118e-45, 4.054120865232601e-89, -0.91079748138448136, 0.51197798649651172},
                LossBiquad{1.0, 0.0, 0.0, 0.0, 0.0}
            }}
        },
        // MIDI 46 A#2; perceptual RMSE 0.069845; compensated partial-cent RMS 0.0675711
        LossPreset{
            46, 116.54665566642632, 9.8084450442480643e-05, 1.3737953504401821, 1.3739418297286574, 3, {{
                LossBiquad{0.84204817504418916, -0.56613526389717839, 0.74883459933751961, -0.53194440161667522, 0.55669191210120539},
                LossBiquad{0.79365545127737025, -0.18502510560430913, 0.59922027113962872, -0.16349793791643263, 0.37134855472912237},
                LossBiquad{0.44873354797492604, 0.46433332226147728, 0.21040734625028279, 0.11743494651634738, 0.0060392699703388664},
                LossBiquad{1.0, 0.0, 0.0, 0.0, 0.0}
            }}
        },
        // MIDI 47 B2; perceptual RMSE 0.110282; compensated partial-cent RMS 0.0917533
        LossPreset{
            47, 123.47734020105011, 0.00010553195672191441, 1.2852311063607338, 1.2854189766240314, 3, {{
                LossBiquad{0.78237637959802242, -0.20785022086082602, 0.63383476900800062, -0.18515517227898787, 0.39351610002418491},
                LossBiquad{0.59869981100932923, 0.30347583931011274, 0.36337802920662815, 0.18484353997793687, 0.080710139548133358},
                LossBiquad{0.81977971022898033, -0.4777116899396327, 0.73471529861911655, -0.44436245047016404, 0.52114576937862833},
                LossBiquad{1.0, 0.0, 0.0, 0.0, 0.0}
            }}
        },
        // MIDI 48 C3; perceptual RMSE 0.154149; compensated partial-cent RMS 0.114045
        LossPreset{
            48, 130.82023754164183, 0.00011398127331188704, 1.8563710233626813, 1.856590252823465, 4, {{
                LossBiquad{0.7809653538889253, -0.16958373396678675, 0.66524755881595921, -0.15447915017945416, 0.43110832891755202},
                LossBiquad{0.6821128293019989, 0.21051683376480962, 0.50452914241835689, 0.17075043866918482, 0.22640836681598073},
                LossBiquad{0.40005003355938473, 0.49605584318128126, 0.21944501377562878, 0.11114382443576572, 0.0044070660805291019},
                LossBiquad{0.81083360203183263, -0.37879150666754663, 0.7375055850559975, -0.3546491238331258, 0.52419680425340898}
            }}
        },
        // MIDI 49 C#3; perceptual RMSE 0.152831; compensated partial-cent RMS 0.132269
        LossPreset{
            49, 138.59987879137987, 0.00012358014597107486, 1.8419372657560449, 1.8421757710764595, 4, {{
                LossBiquad{0.67815963024209347, 0.23265426230064981, 0.49779134015444565, 0.1880520410793437, 0.22055319161784523},
                LossBiquad{0.81023669773285056, -0.36048647542119172, 0.73552530832367613, -0.33769291998177892, 0.52296845061711383},
                LossBiquad{0.39852247386749357, 0.50540129428607627, 0.22043717942586633, 0.11945342635801463, 0.004907521221421669},
                LossBiquad{0.77996586084158726, -0.14720412976368771, 0.66046503295988479, -0.13406651242892254, 0.42729327646670701}
            }}
        },
        // MIDI 50 D3; perceptual RMSE 0.14749; compensated partial-cent RMS 0.15364
        LossPreset{
            50, 146.84225827060348, 0.00013450226239783287, 1.8259943578943805, 1.8262540233505724, 4, {{
                LossBiquad{0.3971740508222103, 0.51541910990856021, 0.2218637454048536, 0.12894203681663166, 0.0055148693189926712},
                LossBiquad{0.77950544941917121, -0.12522192759127132, 0.65539685978630569, -0.11404033302765662, 0.42372071464186239},
                LossBiquad{0.8096241663668301, -0.34314513945564618, 0.73398785217688733, -0.32164143235182946, 0.52210831143990089},
                LossBiquad{0.67455755137782403, 0.25479311252198161, 0.49083018272399914, 0.20527519322440732, 0.2149056533993976}
            }}
        },
        // MIDI 51 D#3; perceptual RMSE 0.141734; compensated partial-cent RMS 0.178772
        LossPreset{
            51, 155.57492164164427, 0.00014695222329557325, 1.7925038026421427, 1.7927866245143291, 4, {{
                LossBiquad{0.81216454840059971, -0.33434640898098977, 0.73389120027200094, -0.31400206901925859, 0.5257114087108693},
                LossBiquad{0.78184667879953962, -0.10816666372720689, 0.65117524825924578, -0.098654116443599463, 0.42350937977517794},
                LossBiquad{0.67418385641557443, 0.27485649939756951, 0.48470087919722538, 0.22150706623984862, 0.21223416877052068},
                LossBiquad{0.39814596165299798, 0.52899270293779987, 0.22467237876124496, 0.14508247671737887, 0.0067285666346639126}
            }}
        },
        // MIDI 52 E3; perceptual RMSE 0.159031; compensated partial-cent RMS 0.20837
        LossPreset{
            52, 164.82705956849367, 0.00016117156339635311, 1.8318520270629184, 1.8321607486002394, 4, {{
                LossBiquad{0.77158682133214906, -0.073829549458318891, 0.64614971695405565, -0.067028258385040521, 0.41093524721292629},
                LossBiquad{0.39076640575834254, 0.52336851989223643, 0.22305495704437939, 0.13167284038894816, 0.0055170423060103349},
                LossBiquad{0.80464783654193039, -0.29504974108237353, 0.72898418404078735, -0.27647948910171721, 0.51506176860206143},
                LossBiquad{0.661925353478789, 0.29453555147798777, 0.47873361359970568, 0.23443563961366914, 0.20075887894281369}
            }}
        },
        // MIDI 53 F3; perceptual RMSE 0.148293; compensated partial-cent RMS 0.243437
        LossPreset{
            53, 174.62960732278373, 0.00017744606063331591, 1.7777087021441078, 1.7780454298823132, 4, {{
                LossBiquad{0.66552332264769232, 0.31269994256519362, 0.47341569255596339, 0.25009667701274041, 0.20154228075610886},
                LossBiquad{0.77761475967765514, -0.062918118435130052, 0.64246865931546704, -0.057308075563664025, 0.4144733761216563},
                LossBiquad{0.81044463750491424, -0.29543900360909492, 0.72983465355777344, -0.27768077595418567, 0.5225210634077786},
                LossBiquad{0.3946577235278238, 0.5417611887263335, 0.22788010551120377, 0.15676843659313303, 0.0075305811722281221}
            }}
        },
        // MIDI 54 F#3; perceptual RMSE 0.16255; compensated partial-cent RMS 0.284694
        LossPreset{
            54, 185.01531484113596, 0.00019572588119413348, 1.8027929828392102, 1.8031608550239093, 4, {{
                LossBiquad{0.76992651654348632, -0.033240211859066664, 0.63786261221782892, -0.030175557669934835, 0.40472447457218347},
                LossBiquad{0.38899950685405255, 0.5393464017350208, 0.22711686175424747, 0.14874326251791362, 0.0067195078254070149},
                LossBiquad{0.80538759194076182, -0.26318681685482853, 0.72621609689911026, -0.24708310766026198, 0.51549997964530558},
                LossBiquad{0.65556120482435254, 0.33043338384663162, 0.4675649986490571, 0.26153675631115497, 0.19202283100888626}
            }}
        },
        // MIDI 55 G3; perceptual RMSE 0.175371; compensated partial-cent RMS 0.333299
        LossPreset{
            55, 196.0188708926301, 0.00021586009702127855, 1.8220418203826252, 1.8224436983790775, 4, {{
                LossBiquad{0.76317866522055766, -0.0048671068125598687, 0.6327531615941383, -0.0044045034080393442, 0.39546922341017554},
                LossBiquad{0.64648187684162373, 0.34810473556535698, 0.46129246193741896, 0.27277068577121577, 0.18310838857318404},
                LossBiquad{0.80099796851662364, -0.23374235382430994, 0.72271993043339056, -0.21921923739195007, 0.50919478251765427},
                LossBiquad{0.38418003854879795, 0.53904900056260285, 0.22697972671807487, 0.14398702269117472, 0.0062217431383009602}
            }}
        },
        // MIDI 56 G#3; perceptual RMSE 0.143292; compensated partial-cent RMS 0.392464
        LossPreset{
            56, 207.67706146198978, 0.00023803383575226843, 1.7662672396236248, 1.7667075039997193, 4, {{
                LossBiquad{0.80413901496088391, -0.2327740157227085, 0.72204689541035305, -0.21858470380490486, 0.51199659845343348},
                LossBiquad{0.77004394525262798, 0.008372571400022016, 0.62588039521627525, 0.0075960359289465066, 0.39670087593997894},
                LossBiquad{0.38942331393042634, 0.56168869716737202, 0.23457026565637559, 0.17664747916941342, 0.0090347975847603641},
                LossBiquad{0.65088483988566415, 0.37004690111106014, 0.45466009143912534, 0.29170227260772918, 0.18388955982812061}
            }}
        },
        // MIDI 57 A3; perceptual RMSE 0.147153; compensated partial-cent RMS 0.462027
        LossPreset{
            57, 220.02886764999613, 0.00026245039957700279, 1.7652270331806645, 1.7657089514032218, 4, {{
                LossBiquad{0.80273270743186387, -0.21180731399037436, 0.72033124395852921, -0.19896050252595668, 0.51021713992597539},
                LossBiquad{0.76711069527806519, 0.030361724609454206, 0.62198874017926842, 0.027518051116711011, 0.39194310895007695},
                LossBiquad{0.38743463399745015, 0.5653504198731395, 0.23612278221303939, 0.17966842257094376, 0.0092394135126852885},
                LossBiquad{0.64594194268653071, 0.38511854677851515, 0.44986413304384226, 0.30210751923153562, 0.17881710327735231}
            }}
        },
        // MIDI 58 A#3; perceptual RMSE 0.148334; compensated partial-cent RMS 0.545516
        LossPreset{
            58, 233.11559746243384, 0.00028933301843624387, 1.7517608113022625, 1.7522886874582739, 4, {{
                LossBiquad{0.64364457281810739, 0.39920230140250573, 0.44589417582727081, 0.31267451672344276, 0.17606653332444119},
                LossBiquad{0.76634015810174716, 0.048852371377210831, 0.61861553087462817, 0.044282528699174512, 0.38952553165441145},
                LossBiquad{0.80322807473756053, -0.19621778194822642, 0.71934760646654561, -0.18450166262510986, 0.5108595618809898},
                LossBiquad{0.38725438061115197, 0.57172555073064457, 0.23875002918435118, 0.18775839603035138, 0.0099715644957964825}
            }}
        },
        // MIDI 59 B3; perceptual RMSE 0.147007; compensated partial-cent RMS 0.646362
        LossPreset{
            59, 246.98102564028997, 0.00031892676924959808, 1.722511782808311, 1.7230903762131455, 4, {{
                LossBiquad{0.64480035512729794, 0.41241963130847409, 0.44307157213631421, 0.32398464355795298, 0.17630691501413318},
                LossBiquad{0.7683299239991378, 0.063014730068522651, 0.61598298852704536, 0.057211189879150921, 0.39011645271555506},
                LossBiquad{0.80610838604814983, -0.18750898730915164, 0.71922438831447588, -0.17663092369359887, 0.51445471074707305},
                LossBiquad{0.38947679206223429, 0.58158596072273561, 0.24277936138576664, 0.2023909960165805, 0.011451118154156028}
            }}
        },
        // MIDI 60 C4; perceptual RMSE 0.180402; compensated partial-cent RMS 0.758306
        LossPreset{
            60, 261.67154204234828, 0.00035150067659325503, 1.8241763177887147, 1.8248077445283379, 4, {{
                LossBiquad{0.6186833486229727, 0.42698841402721499, 0.43231160052857981, 0.32399102838849331, 0.15399233479027424},
                LossBiquad{0.37384227470394205, 0.56131522453380656, 0.23565782061620855, 0.16335399192462111, 0.0074613279293359669},
                LossBiquad{0.74756064900937746, 0.10861969717761091, 0.60779062072676571, 0.097544302806611111, 0.36642666410714286},
                LossBiquad{0.79024035787028468, -0.12458712483128905, 0.71057313957165946, -0.11673333069006227, 0.49295970330071742}
            }}
        },
        // MIDI 61 C#4; perceptual RMSE 0.0717504; compensated partial-cent RMS 2.28898
        LossPreset{
            61, 277.23630912698644, 0.00038735001165072271, 1.9239666419353099, 1.9272074401705011, 4, {{
                LossBiquad{0.30638489906467958, -6.2410885793764811e-50, 3.9135850467646334e-99, -1.0027602891330949, 0.30953922642078202},
                LossBiquad{0.813528759254464, -5.1609109647338134e-44, 3.1447127462667622e-87, -0.79776195144128503, 0.61129071069574903},
                LossBiquad{0.46665974171823499, 0.24818176211925769, 0.28515849616250755, 3.9521386117670613e-41, 3.3745142041420906e-81},
                LossBiquad{0.73660617247472315, 0.019733016865706797, 0.64167062105954809, 0.01770060661787028, 0.38030920378210792}
            }}
        },
        // MIDI 62 D4; perceptual RMSE 0.0450407; compensated partial-cent RMS 2.26836
        LossPreset{
            62, 293.72742911860752, 0.00042679880778770526, 2.1610749485494587, 2.1621826650352611, 4, {{
                LossBiquad{0.83131736405068957, -3.7875069123247481e-40, 1.8146165458798272e-79, -0.73172604922186246, 0.56304341327255203},
                LossBiquad{0.53008642903194325, 0.031319063247417782, 0.45934858109223836, 0.0073486092971235635, 0.013405464074475772},
                LossBiquad{0.30951568459609674, 0.48681082686030291, 0.20367348854360029, 2.2630662354532913e-41, 1.3623579700966239e-82},
                LossBiquad{0.44051681594186654, 0.27545726106119706, 0.28402592299693646, 3.5118967405930994e-42, 2.0337403137915133e-83}
            }}
        },
        // MIDI 63 D#4; perceptual RMSE 0.0657496; compensated partial-cent RMS 2.81324
        LossPreset{
            63, 311.20012148602342, 0.00047020261276590553, 2.1864389583355623, 2.1875844144357983, 4, {{
                LossBiquad{0.84109147180108479, -1.2638455670783654e-43, 2.0361479961067161e-86, -0.72952358886369373, 0.57061506066477852},
                LossBiquad{0.43597115903510014, 0.27700939877601088, 0.28701944218888903, 1.1259589930344634e-41, 2.0674033459831961e-82},
                LossBiquad{0.31408781579836786, 0.48425140829239877, 0.2016607759092332, 2.4448182893011489e-42, 1.614447437706064e-84},
                LossBiquad{0.50082566867945677, 0.061612027606464857, 0.43756230371407839, 3.8318574634296431e-41, 8.4764532065839248e-80}
            }}
        },
        // MIDI 64 E4; perceptual RMSE 0.0657451; compensated partial-cent RMS 3.58351
        LossPreset{
            64, 329.71291140560572, 0.00051795149942170777, 1.928707234431728, 1.9318848535460593, 4, {{
                LossBiquad{0.9184500139232844, -1.4364847448825133e-38, 2.8207126572462874e-76, -0.70427393734915655, 0.62272395127244096},
                LossBiquad{0.40414440545329416, -1.1999415693086445e-44, 1.2652937739721639e-88, -0.85621521047358851, 0.26035961592688267},
                LossBiquad{0.42848841816496142, 0.37507913857684028, 0.19643244325819831, 6.626232035549544e-41, 2.6268771690193748e-81},
                LossBiquad{0.51366319594019116, 0.054415264705176575, 0.43192153935463229, 6.9495603623771096e-38, 3.6187325133546361e-73}
            }}
        },
        // MIDI 65 F4; perceptual RMSE 0.0866365; compensated partial-cent RMS 4.31498
        LossPreset{
            65, 349.32782993150971, 0.00057047335860718198, 1.9574757876331579, 1.9610653047728726, 4, {{
                LossBiquad{0.93492897919899287, -1.1737139364047039e-41, 1.9143964330198564e-82, -0.69792505280601824, 0.63285403200501111},
                LossBiquad{0.39440794489233139, -3.8213897357063722e-46, 1.294808741220689e-91, -0.87073584260537518, 0.26514378749770656},
                LossBiquad{0.42641470764316003, 0.37073659196930131, 0.20284870038753872, 1.8993918476267672e-39, 2.2704013583229737e-78},
                LossBiquad{0.49752472066342557, 0.081594557195582534, 0.42088072214099181, 2.2578240874489454e-38, 1.6033617641545745e-74}
            }}
        },
        // MIDI 66 F#4; perceptual RMSE 0.0628287; compensated partial-cent RMS 4.79297
        LossPreset{
            66, 370.11062664921354, 0.0006282375003364934, 1.8713294379805907, 1.8742449590281991, 4, {{
                LossBiquad{0.94074818118247328, -3.1252951619650871e-36, 1.4202169507347681e-71, -0.66602383561227252, 0.60677201679474579},
                LossBiquad{0.43636762268974572, -2.7140000760277919e-38, 6.0869915492715461e-76, -0.78695807512045957, 0.22332569781020528},
                LossBiquad{0.42933090073792363, 0.38277834607251765, 0.18789075318955875, 9.3919716962083904e-38, 4.8564215993479688e-75},
                LossBiquad{0.51209429376731896, 0.062359547996954409, 0.4255461582357265, 8.0885737580518696e-33, 3.6663556735469277e-63}
            }}
        },
        // MIDI 67 G4; perceptual RMSE 0.0786256; compensated partial-cent RMS 5.61152
        LossPreset{
            67, 392.13099564747938, 0.00069175859141567208, 1.8943576785377716, 1.8972845361672501, 4, {{
                LossBiquad{0.94918355985457403, -1.5733975301045952e-33, 3.6482568241664642e-66, -0.65983690370455428, 0.60902046355912831},
                LossBiquad{0.43547780713347622, -1.0673611105924069e-37, 9.2712036158079239e-75, -0.78029536976322722, 0.21577317689670344},
                LossBiquad{0.4270582191395908, 0.38275759273255144, 0.19018418812785789, 3.482226736999403e-33, 6.7224557799373304e-66},
                LossBiquad{0.49910172322318413, 0.084785221749654971, 0.41611305502716084, 3.2185735491948606e-30, 2.9928638857699541e-58}
            }}
        },
        // MIDI 68 G#4; perceptual RMSE 0.0969204; compensated partial-cent RMS 6.31074
        LossPreset{
            68, 415.46281570820787, 0.00076160096037399168, 1.9186597273597237, 1.9213211202859977, 4, {{
                LossBiquad{0.95189438729196829, -2.172452146674629e-40, 6.9864560788571804e-80, -0.65776579309825267, 0.60966018039022096},
                LossBiquad{0.43784204929611248, -2.5461216329864874e-38, 5.1182046692424643e-76, -0.76385414325985113, 0.2016961925559636},
                LossBiquad{0.42441754339242582, 0.38576952306137663, 0.18981293354619755, 4.7703916290517981e-39, 1.231888408596557e-77},
                LossBiquad{0.48805977923344812, 0.10528572259563578, 0.40665449817091592, 2.7196926822242915e-41, 1.3243394034058489e-80}
            }}
        },
        // MIDI 69 A4; perceptual RMSE 0.111724; compensated partial-cent RMS 4.59006
        LossPreset{
            69, 440.1844056841573, 0.00083838330327871036, 1.5397239014500914, 1.5395318579045452, 3, {{
                LossBiquad{0.81939940903379604, -4.5762166083920735e-35, 2.5730981408007958e-69, -0.75596693989281605, 0.57536634892661209},
                LossBiquad{0.30570587941298066, -1.3961402878224216e-33, 1.7592418700864173e-66, -0.93604110597138024, 0.24174698538436087},
                LossBiquad{0.6604705739463792, 0.055757662924567984, 0.51191369650108021, 0.041434341357714635, 0.18670759201431267},
                LossBiquad{1.0, 0.0, 0.0, 0.0, 0.0}
            }}
        },
        // MIDI 70 A#4; perceptual RMSE 0.0816922; compensated partial-cent RMS 5.32549
        LossPreset{
            70, 466.37879611185355, 0.0009227838270206535, 1.6995081965154242, 1.6969866675750764, 4, {{
                LossBiquad{0.8249804917317014, -2.2673988993536562e-37, 6.526338653734352e-74, -0.72405471675082156, 0.54903520848252296},
                LossBiquad{0.39776862794779327, -5.4867992298432199e-37, 1.892132424460408e-73, -0.73862392536009136, 0.13639255330788463},
                LossBiquad{0.49164273728618058, 0.24741015703006231, 0.26094710568375723, 5.194592830570893e-35, 5.6554880090514096e-69},
                LossBiquad{0.826511306080154, 0.087171448697988527, 0.69181167086686679, 0.083304802786387938, 0.52218962285862114}
            }}
        },
        // MIDI 71 B4; perceptual RMSE 0.0778601; compensated partial-cent RMS 5.81323
        LossPreset{
            71, 494.13401819204773, 0.0010155458699268624, 1.7377858496045961, 1.7355437636857822, 4, {{
                LossBiquad{0.47279885804412658, 0.28055453730088736, 0.24664660465498592, 5.5487265359987527e-34, 4.5614572578290812e-67},
                LossBiquad{0.84375082038595683, -7.0546120143213622e-33, 6.473668360180192e-65, -0.71086409270833173, 0.55461491309428856},
                LossBiquad{0.38536122668380524, -1.5803702636915289e-36, 1.6922230084182991e-72, -0.76907269362081299, 0.15443392030461825},
                LossBiquad{0.80153296098219295, 0.13360930904459054, 0.66747303942014036, 0.1260955134297152, 0.47651979601720862}
            }}
        },
        // MIDI 72 C5; perceptual RMSE 0.0634323; compensated partial-cent RMS 6.98904
        LossPreset{
            72, 523.54341136371761, 0.0011174840431107992, 1.9194205856350413, 1.9076708193982626, 4, {{
                LossBiquad{0.59541948147801482, -6.8468488121673229e-22, 4.7824285444992898e-43, -0.93046702919755953, 0.52588651067557435},
                LossBiquad{1.2363554901045501, -3.4565256171392165e-17, 3.6955439696543937e-33, -0.41163629718215611, 0.64799178728670626},
                LossBiquad{0.25526993255653652, -7.3936170994525123e-21, 5.3537509548177583e-41, -0.98951946192201123, 0.24478939447854778},
                LossBiquad{0.49956879703451884, 0.11493791817568985, 0.38549328478979145, 9.7261734360766485e-18, 1.3790165794807108e-33}
            }}
        },
        // MIDI 73 C#5; perceptual RMSE 0.0678019; compensated partial-cent RMS 8.28039
        LossPreset{
            73, 554.70595080091482, 0.0012294909398345667, 1.8697967381891152, 1.8578719536461605, 4, {{
                LossBiquad{0.59513734312680155, -2.1280396439599575e-32, 4.6292634706729986e-64, -0.92174934489241933, 0.51688668801922089},
                LossBiquad{1.2367525707986902, -1.1905203734575172e-28, 4.4151628589297397e-56, -0.40959565042679441, 0.64634822122548452},
                LossBiquad{0.26176580336535693, -9.9902098661850298e-30, 9.5319165195870356e-59, -0.97674312747693881, 0.23850893084229571},
                LossBiquad{0.50534870072958837, 0.12021318322076259, 0.37443811604964905, 3.7448047730570376e-26, 1.836222791281893e-50}
            }}
        },
        // MIDI 74 D5; perceptual RMSE 0.10881; compensated partial-cent RMS 7.46567
        LossPreset{
            74, 587.726596275931, 0.0013525444643578469, 2.2721560597064556, 2.2497487616297134, 4, {{
                LossBiquad{0.50601613121066114, -5.0814939198974783e-10, 2.5798864858244047e-19, -1.020633771210389, 0.52664990191290073},
                LossBiquad{1.1068805259825993, -1.4082336322663443e-08, 3.813122249958832e-16, -0.55959999103340186, 0.66648050293366534},
                LossBiquad{0.23768898785669135, -0.026056077545083309, 0.00071408996843597117, -1.0783831952136855, 0.29073019549372953},
                LossBiquad{0.3664121531866158, 0.44448230485458357, 0.18910555513069316, 1.3171892452300609e-08, 6.0850136487015347e-17}
            }}
        },
        // MIDI 75 D#5; perceptual RMSE 0.108661; compensated partial-cent RMS 7.89008
        LossPreset{
            75, 622.71666395867419, 0.0014877158363130931, 1.9557644989831819, 1.9549046039842655, 4, {{
                LossBiquad{0.35044198973667962, -1.6925713823151483e-31, 2.2543704191277419e-62, -0.84774736572881604, 0.19818935546549565},
                LossBiquad{0.8193850073257346, -7.0955038103824902e-33, 6.3992301691956794e-65, -0.71896489066268876, 0.53834989798842336},
                LossBiquad{0.25381016155296293, 0.49993687445431367, 0.24625296399272339, 1.490034648235062e-31, 5.5520435653692295e-63},
                LossBiquad{0.9017875701537541, 0.086060648824081556, 0.74716127667026799, 0.084557647486080259, 0.6504518481620235}
            }}
        },
        // MIDI 76 E5; perceptual RMSE 0.157059; compensated partial-cent RMS 7.12794
        LossPreset{
            76, 659.79422286235888, 0.001636178331607003, 1.8006808520174964, 1.7984917669906975, 4, {{
                LossBiquad{0.79841502089760352, -3.8082930807583384e-33, 1.8762097841767287e-65, -0.68199583466876212, 0.48041085556636565},
                LossBiquad{0.42108806392356657, -1.649542529276959e-38, 1.6285636265493627e-76, -0.70372346012087539, 0.12481152404444198},
                LossBiquad{0.25381016126079203, 0.49993687502995621, 0.24625296370925193, 7.8391280113376565e-38, 1.5367231774271624e-75},
                LossBiquad{0.93525308297040399, -0.095204853133242021, 0.78656451688925388, -0.094243561385755792, 0.7208563081121715}
            }}
        },
        // MIDI 77 F5; perceptual RMSE 0.171429; compensated partial-cent RMS 5.20042
        LossPreset{
            77, 699.08451780140376, 0.0017992168262377724, 1.7218728744503611, 1.7225267427300839, 4, {{
                LossBiquad{0.77686805544331128, -2.8567294007943108e-13, 9.5631444036885878e-26, -0.78705475303714645, 0.56392280848017207},
                LossBiquad{0.25381017126500244, 0.49993685532002735, 0.24625297341558694, 6.1687330449681065e-13, 9.5159499635919918e-26},
                LossBiquad{0.29444606844752802, -9.0857632931115183e-14, 7.8262306108853067e-27, -0.96609375417025278, 0.26053982261768993},
                LossBiquad{1.5012895643365627, 0.70564118219731831, 0.36188255142517772, 0.82542268528384766, 0.74339061267521112}
            }}
        },
        // MIDI 78 F#5; perceptual RMSE 0.171633; compensated partial-cent RMS 5.57932
        LossPreset{
            78, 740.72042090083153, 0.0019782382152723548, 1.3477050117397311, 1.3524241661697456, 4, {{
                LossBiquad{1.1506304430989278, -2.4201458091989297e-08, 3.1551828093314013e-15, -0.25619046162759324, 0.40682088053847421},
                LossBiquad{0.61560515447419406, -2.6045378634018488e-21, 3.9608100410198681e-42, -0.46067569231610001, 0.07628084679029401},
                LossBiquad{0.25381016011855723, 0.49993687728041569, 0.24625296260102714, 1.3489078789152533e-24, 4.5501394153049702e-49},
                LossBiquad{0.80142262908969064, -0.17176132369933242, 0.60326056006614537, -0.15357260618450891, 0.38649447164101258}
            }}
        },
        // MIDI 79 G5; perceptual RMSE 0.315844; compensated partial-cent RMS 0.525506
        LossPreset{
            79, 784.84291388976635, 0.0021747827855870525, 2.0998745036794801, 2.100302844605678, 4, {{
                LossBiquad{0.42482988146917677, 0.51205048427415256, 0.15429452462164095, 0.089186339668248765, 0.0019885506967213793},
                LossBiquad{0.34281625677786376, 0.544651993583401, 0.20372787878461812, 0.089206671946951671, 0.0019894571989313679},
                LossBiquad{0.46660628882181049, 0.92047022701319992, 0.45395093014944154, 0.80702528402599616, 0.034002161958455276},
                LossBiquad{0.78713499271026621, 1.552972233893025, 0.76598130602517678, 1.5248197303225797, 0.58126880230588773}
            }}
        },
        // MIDI 80 G#5; perceptual RMSE 0.271392; compensated partial-cent RMS 0.532469
        LossPreset{
            80, 831.6016036274757, 0.0023905366278806632, 1.9762871820984709, 1.9767624931441057, 3, {{
                LossBiquad{0.51777577070670378, 0.47969153347023874, 0.11110213160009218, 0.10577248134934411, 0.0027969544276904802},
                LossBiquad{0.27753781578083575, 0.55455005172486382, 0.27701248476972784, 0.10627666964761093, 0.0028236826278163888},
                LossBiquad{0.44239191488536783, 0.88422303301880378, 0.44183125250524974, 0.7323537834166951, 0.036092416992726258},
                LossBiquad{1.0, 0.0, 0.0, 0.0, 0.0}
            }}
        },
        // MIDI 81 A5; perceptual RMSE 0.0838398; compensated partial-cent RMS 5.71478
        LossPreset{
            81, 881.15527355179665, 0.0026273451809679005, 1.8708605165672132, 1.8707476880446992, 4, {{
                LossBiquad{0.25380826832008513, 0.49994060454906014, 0.24625112713085451, 3.9433671261629507e-22, 3.8884954418099932e-44},
                LossBiquad{0.28585248879988273, 0.56305993105882479, 0.27734123094586616, 0.12250109538231924, 0.0037525554222545832},
                LossBiquad{0.88952208092469398, -0.0064729040827279415, 0.71042475087685919, -0.0063060548915932204, 0.59977998261041843},
                LossBiquad{1.0383830520835158, -0.76997097176181872, 0.54447841516365059, -0.80481443341564596, 0.6177049289009936}
            }}
        },
        // MIDI 82 A#5; perceptual RMSE 0.297992; compensated partial-cent RMS 0.547035
        LossPreset{
            82, 933.67247401013549, 0.0028872280095014581, 2.0388747381894436, 2.0394596395426214, 4, {{
                LossBiquad{0.45112344629003676, 0.5061998888374255, 0.14200011635724805, 0.09697253343732698, 0.0023509180473833898},
                LossBiquad{0.35501186156001641, 0.54840482786114375, 0.19593154486578732, 0.096996170065073967, 0.002352064221873975},
                LossBiquad{0.78016445339767337, 1.5351675201087673, 0.75520593911588041, 1.50459008308955, 0.56594782953277112},
                LossBiquad{0.46687075127423489, 0.91856289695870263, 0.45181550655131658, 0.80077660608903134, 0.036472548695222949}
            }}
        },
        // MIDI 83 B5; perceptual RMSE 0.0917707; compensated partial-cent RMS 6.20159
        LossPreset{
            83, 989.33215473644816, 0.0031723949251077372, 1.9024172889302722, 1.9018663137522731, 4, {{
                LossBiquad{1.042764534267739, -0.72987722420299184, 0.53778445420681609, -0.76437573681212478, 0.61504750108368789},
                LossBiquad{0.89575029717809751, 0.043047618447192237, 0.72690166510496435, 0.042127074117200104, 0.62357250661305408},
                LossBiquad{0.25380826832008513, 0.49994060454906014, 0.24625112713085451, 4.1383520760944167e-20, 4.2825460654108689e-40},
                LossBiquad{0.27134525539412535, 0.53448420700060773, 0.26326595041476519, 0.067941128779785656, 0.0011542840297125866}
            }}
        },
        // MIDI 84 C6; perceptual RMSE 0.1524; compensated partial-cent RMS 4.27407
        LossPreset{
            84, 1048.3243430781151, 0.0034852635705114195, 1.0594020733073863, 1.0640945818572931, 4, {{
                LossBiquad{1.1548371675316658, 0.4211469536699663, 0.37428563430560308, 0.45211853184439016, 0.49815122366284514},
                LossBiquad{1.0213940519196587, -1.2788329610590397e-14, 3.4867114476539521e-28, -0.47969800997874573, 0.50109206189839162},
                LossBiquad{0.73677023909081396, -0.20688757298020105, 0.44629981725134699, -0.03138442002002742, 0.0075669033819872784},
                LossBiquad{0.25380826832008513, 0.49994060454906014, 0.24625112713085451, 2.4790979307549648e-49, 1.5368608115867825e-98}
            }}
        },
        // MIDI 85 C#6; perceptual RMSE 0.153762; compensated partial-cent RMS 3.63668
        LossPreset{
            85, 1110.8508719595052, 0.003828478596631016, 1.7633217485142672, 1.7666973013945726, 3, {{
                LossBiquad{0.90902533052221868, 0.092900928235506497, 0.73675295099176741, 0.091335298494345629, 0.64734391125514712},
                LossBiquad{0.30775396841189562, 0.58667199832076145, 0.27966250791524611, 0.16710567490406752, 0.0069827997438355769},
                LossBiquad{0.29949232731417752, 0.58657250091071078, 0.28887859854703063, 0.1678583648881832, 0.0070850618837356083},
                LossBiquad{1.0, 0.0, 0.0, 0.0, 0.0}
            }}
        },
        // MIDI 86 D6; perceptual RMSE 0.0978852; compensated partial-cent RMS 3.04118
        LossPreset{
            86, 1177.120911749653, 0.0041959746291991085, 1.5620620380571175, 1.5656155363348243, 3, {{
                LossBiquad{0.94054569790510689, -0.10277527232420591, 0.7743826264466821, -0.1017515354379267, 0.71390458746550955},
                LossBiquad{0.28262852139953343, 0.55670949881386578, 0.27421325717486228, 0.11049806832518054, 0.0030532090630807827},
                LossBiquad{0.41676926970298822, 0.69652535441578844, 0.29108869683938249, 0.37012646845165059, 0.034256852506508673},
                LossBiquad{1.0, 0.0, 0.0, 0.0, 0.0}
            }}
        },
        // MIDI 87 D#6; perceptual RMSE 0.136219; compensated partial-cent RMS 3.97896
        LossPreset{
            87, 1247.3537111270525, 0.0045785644772919076, 1.7881362855494654, 1.7922276964864026, 3, {{
                LossBiquad{0.29523742121146646, 0.58154596705187656, 0.28644672699489498, 0.15706151357995357, 0.006168601678284525},
                LossBiquad{0.91873394310890844, 0.074190594296544773, 0.75427279598088659, 0.073169272247545686, 0.67402806113879399},
                LossBiquad{0.29522801220751543, 0.58152743360080095, 0.28643759814403658, 0.15702714188034395, 0.0061659020720090914},
                LossBiquad{1.0, 0.0, 0.0, 0.0, 0.0}
            }}
        },
        // MIDI 88 E6; perceptual RMSE 0.0767756; compensated partial-cent RMS 3.01636
        LossPreset{
            88, 1321.7853695473279, 0.0049741132176691462, 1.4570313419875911, 1.4574051141483038, 4, {{
                LossBiquad{0.54655201047155422, 0.94821039270077379, 0.4113628771155094, 0.76122402588188964, 0.1449012544059475},
                LossBiquad{0.25380826832008513, 0.49994060454906014, 0.24625112713085451, 1.0625107529960579e-28, 2.8230192453726091e-57},
                LossBiquad{1.5086526169519763, -0.13011226328119824, 0.087575417328505448, -0.3167129979145975, 0.78282876891388076},
                LossBiquad{0.61528735962619863, -0.15753932707784274, 0.52302546628020363, -0.036521147810284663, 0.017294646638844304}
            }}
        },
        // MIDI 89 F6; perceptual RMSE 0.0905232; compensated partial-cent RMS 4.26593
        LossPreset{
            89, 1400.6656635601635, 0.005380118591033785, 1.7232347092229401, 1.6761248629556442, 4, {{
                LossBiquad{0.31088149764518741, 0.55285446276690409, 0.24585210712067804, 0.10673905799997774, 0.0028490095327915973},
                LossBiquad{0.25380826832008513, 0.49994060454906014, 0.24625112713085451, 3.6945350675641147e-25, 3.4132394551794613e-50},
                LossBiquad{1.2169611280560235, -1.3155844829053791, 0.42158010321794248, -0.9376705192505973, 0.26062726761918409},
                LossBiquad{0.92443345841255853, -0.016021151914802377, 0.76319501035771264, -0.015814291481467419, 0.68742160833693611}
            }}
        },
        // MIDI 90 F#6; perceptual RMSE 0.130314; compensated partial-cent RMS 5.31276
        LossPreset{
            90, 1484.2587908408173, 0.0057937250598484348, 1.8635311143413418, 1.864741068645668, 4, {{
                LossBiquad{0.28898613490878466, 0.56923245231077835, 0.28038157274187553, 0.13410313757971767, 0.0044970223817208379},
                LossBiquad{0.26227798306981692, 0.51662388417728311, 0.25446865612391306, 0.033096609402523014, 0.00027391396849001006},
                LossBiquad{0.91988495865767594, 0.15809079774294621, 0.76931581072487565, 0.15622400174795037, 0.69106756537754765},
                LossBiquad{1.025122469813875, -0.70330274875462273, 0.78057927736337707, -0.70760100157737027, 0.80999999999999983}
            }}
        },
        // MIDI 91 G6; perceptual RMSE 0.0773792; compensated partial-cent RMS 2.32361
        LossPreset{
            91, 1572.8441576234102, 0.0062117471027249554, 1.3555610236070983, 1.3584356419803381, 4, {{
                LossBiquad{0.25380826832008513, 0.49994060454906014, 0.24625112713085451, 5.4740011789809913e-37, 7.4930209036184441e-74},
                LossBiquad{1.1183496665094224, -0.36633220003782246, 0.67987628925599508, -0.37810624427240508, 0.80999999999999983},
                LossBiquad{0.61653415633500641, 0.9533109937137656, 0.36860331100660143, 0.78453621837102638, 0.15391224268434758},
                LossBiquad{0.85216827437198883, -0.21677222085909906, 0.7113094672586282, -0.20722332767972199, 0.5539288484512398}
            }}
        },
        // MIDI 92 G#6; perceptual RMSE 0.184447; compensated partial-cent RMS 3.45737
        LossPreset{
            92, 1666.7172140392568, 0.0066307018611877029, 0.95484745081519651, 0.97950411431483786, 4, {{
                LossBiquad{0.96618582614800952, 0.017132177042211715, 0.81271285808385785, 0.017063700968161229, 0.77896716030591795},
                LossBiquad{0.29676136198063663, 0.58454775999770392, 0.28792529242766313, 0.1626213564408836, 0.0066130579651200814},
                LossBiquad{0.81633534376788941, -0.71180490547500497, 0.22558976428799121, -1.1534779349948177, 0.48359813757569337},
                LossBiquad{1.1142704501543739, -1.3979887086031719, 0.61036774479161726, -1.0770069167046121, 0.40365640304743128}
            }}
        },
        // MIDI 93 A6; perceptual RMSE 0.262743; compensated partial-cent RMS 0.559003
        LossPreset{
            93, 1766.1903423903611, 0.0070468509662259069, 1.6144129817487587, 1.6157309760247456, 3, {{
                LossBiquad{0.49012173397677766, 0.4836209634333235, 0.11930160408126131, 0.0909751806561509, 0.0020691208352116375},
                LossBiquad{0.27927195487909234, 0.54646010395910904, 0.267318860132222, 0.0909815102250775, 0.0020694087453457547},
                LossBiquad{0.77422347724640284, 1.5151467697642336, 0.74128136171086312, 1.4817533564124923, 0.54889825230900702},
                LossBiquad{1.0, 0.0, 0.0, 0.0, 0.0}
            }}
        },
        // MIDI 94 A#6; perceptual RMSE 0.0596291; compensated partial-cent RMS 1.97334
        LossPreset{
            94, 1871.5938039022826, 0.0074562510678259147, 0.93153301129185218, 0.93687801239804414, 4, {{
                LossBiquad{0.86253688118292982, 0.58875218071464552, 0.18171418488561894, 0.513687185084023, 0.11931606169917129},
                LossBiquad{1.0680078046612123, -0.47891320420628869, 0.73413503069152941, -0.48677036885354702, 0.80999999999999983},
                LossBiquad{0.32460320106342794, 0.63938941647697334, 0.3149381407517643, 0.26179267837547182, 0.017138079916693888},
                LossBiquad{0.90006575917513887, -0.28561930823660592, 0.73058809530549607, -0.27802094169656094, 0.62305548794058985}
            }}
        },
        // MIDI 95 B6; perceptual RMSE 0.0805832; compensated partial-cent RMS 1.75119
        LossPreset{
            95, 1983.2767499858901, 0.0078548122818324299, 1.3531285214151354, 1.3561637820199477, 4, {{
                LossBiquad{0.2538103198914628, 0.49993656249184282, 0.24625311761669444, 3.7949832281436875e-27, 3.6014794102877818e-54},
                LossBiquad{0.86417911458991736, -0.15823822000844837, 0.70109207335370816, -0.15199882520439426, 0.55903179313957141},
                LossBiquad{1.1055695342172753, -0.37516062346599738, 0.69417204882411387, -0.38541904042460834, 0.80999999999999983},
                LossBiquad{0.61375684004062159, 0.91307435369717205, 0.33967463302516182, 0.73237825700137738, 0.13412756976157814}
            }}
        },
        // MIDI 96 C7; perceptual RMSE 0.173194; compensated partial-cent RMS 2.44785
        LossPreset{
            96, 2101.6083044778529, 0.0082383634672643612, 0.95922296616437186, 0.9624810790288264, 3, {{
                LossBiquad{0.87774704778613055, 0.036008373347515235, 0.72736975256339975, 0.035082677780675191, 0.60604249591637005},
                LossBiquad{0.27494387626496108, 0.54157261552888936, 0.26675742234917182, 0.081608515793732114, 0.0016653983492901487},
                LossBiquad{1.0861814908752172, -0.14143436108338325, 0.72145898421512045, -0.14379388593489342, 0.80999999994184779},
                LossBiquad{1.0, 0.0, 0.0, 0.0, 0.0}
            }}
        },
        // MIDI 97 C#7; perceptual RMSE 0.0425146; compensated partial-cent RMS 1.71265
        LossPreset{
            97, 2226.9787237091823, 0.0086027229663233999, 0.83320520600641479, 0.84213551044950641, 3, {{
                LossBiquad{0.49618812163070014, 0.97662900604284186, 0.48070939169098875, 0.79533845926313507, 0.15818806010139586},
                LossBiquad{0.63998330812284121, 0.51596750634927968, 0.17399760667171685, 0.29383462387268083, 0.036113797271156919},
                LossBiquad{0.97868243661863064, -0.39694875911573718, 0.8158938979184176, -0.39564772755706806, 0.79327530297837889},
                LossBiquad{1.0, 0.0, 0.0, 0.0, 0.0}
            }}
        },
        // MIDI 98 D7; perceptual RMSE 0.0436846; compensated partial-cent RMS 0.85881
        LossPreset{
            98, 2359.8006415554264, 0.0089437731913762364, 0.96406104922997105, 0.96214417636791194, 3, {{
                LossBiquad{1.0756380023036713, -0.55911528618972139, 0.67046199593680011, -0.57454625594264797, 0.76153096799339792},
                LossBiquad{0.26054124769467046, 0.51319532008337732, 0.25278363204452531, 0.026346615803167186, 0.00017358401940618056},
                LossBiquad{0.91509602865260609, -0.35542937883653408, 0.72511641044696118, -0.34674746407795892, 0.63153052434099211},
                LossBiquad{1.0, 0.0, 0.0, 0.0, 0.0}
            }}
        },
        // MIDI 99 D#7; perceptual RMSE 0.072524; compensated partial-cent RMS 2.41448
        LossPreset{
            99, 2500.5104068388341, 0.0092575372400099439, 0.93452441634829031, 0.94255255564975182, 4, {{
                LossBiquad{1.7836150729543079, 0.58365394307027718, 0.18602964239432737, 0.84983500147580648, 0.70346365694310597},
                LossBiquad{0.25380826832008513, 0.49994060454906014, 0.24625112713085451, 1.518024780082326e-27, 5.7624197855157179e-55},
                LossBiquad{0.63314101891170904, 0.54467982052821562, 0.49252731185996973, 0.45384523319349795, 0.21650291810639677},
                LossBiquad{0.94858811797194065, -0.076255640675083031, 0.77084496945183867, -0.075604600602846975, 0.71878204735154327}
            }}
        },
        // MIDI 100 E7; perceptual RMSE 0.184644; compensated partial-cent RMS 0.5181
        LossPreset{
            100, 2649.5695205729303, 0.0095402555713281437, 1.3208448314275882, 1.3233601457495745, 3, {{
                LossBiquad{0.58790480875923412, 0.46427885233311966, 0.091662310595201621, 0.13901469998498789, 0.0048312717025675777},
                LossBiquad{0.4102238946319316, 0.57181771828029415, 0.16182480914102723, 0.13903382164011469, 0.004832600413138116},
                LossBiquad{0.66047232379721788, 1.3204492175952152, 0.65997698670532867, 1.2501683206254399, 0.39073020747232157},
                LossBiquad{1.0, 0.0, 0.0, 0.0, 0.0}
            }}
        },
        // MIDI 101 F7; perceptual RMSE 0.0551645; compensated partial-cent RMS 0.135537
        LossPreset{
            101, 2807.4661805581104, 0.0097884606924807434, 0.780997028088941, 0.7818006468955534, 2, {{
                LossBiquad{0.53764749987844307, 0.4482941074409118, 0.093447661714620753, 0.077873209831799864, 0.0015160592021755226},
                LossBiquad{0.63020549212040189, 1.1907117200845989, 0.56243337211606714, 1.0876208214878096, 0.29572976283325847},
                LossBiquad{1.0, 0.0, 0.0, 0.0, 0.0},
                LossBiquad{1.0, 0.0, 0.0, 0.0, 0.0}
            }}
        },
        // MIDI 102 F#7; perceptual RMSE 0.0702558; compensated partial-cent RMS 0.181123
        LossPreset{
            102, 2974.7169407523006, 0.0099990477900469728, 0.88568272621865451, 0.88696519757810144, 2, {{
                LossBiquad{0.70227203397691096, 0.3816463261084872, 0.051850960508153542, 0.13144957308174918, 0.0043197475118023866},
                LossBiquad{0.41662029058306932, 0.81490880625657569, 0.39849016928685899, 0.59522060402151855, 0.034798662104985327},
                LossBiquad{1.0, 0.0, 0.0, 0.0, 0.0},
                LossBiquad{1.0, 0.0, 0.0, 0.0, 0.0}
            }}
        },
        // MIDI 103 G7; perceptual RMSE 0.0525788; compensated partial-cent RMS 1.42057
        LossPreset{
            103, 3151.868492657848, 0.010169339299493733, 0.013049888848034315, 1.0638102674823051, 4, {{
                LossBiquad{1.0481933866475628, -0.26052299729462697, 0.13169754432035999, -0.57607826475289015, 0.67497850026949591},
                LossBiquad{1.0535211319724285, -1.2199299283000902, 0.56562538411597296, -1.0060533706585468, 0.40526995844685815},
                LossBiquad{0.97352656424783546, -1.3598989079792561, 0.61436828321769188, -1.4906378114523859, 0.71863375093865722},
                LossBiquad{0.75420030932802473, -0.34720107001453193, 0.57450266923376714, -0.25829173525246335, 0.23979364379972326}
            }}
        },
        // MIDI 104 G#7; perceptual RMSE 0.0350991; compensated partial-cent RMS 1.02068
        LossPreset{
            104, 3339.4995756943131, 0.010297141537553957, 0.5820332730330029, 1.0426688248702074, 4, {{
                LossBiquad{1.3124841978445625, 0.92000030814510136, 0.30916446770252004, 1.2482887460117607, 0.74702962073536527},
                LossBiquad{0.87311508588532194, -2.2568895874266374e-19, 6.6382695344037845e-38, -0.72500634557725152, 0.59812143146257346},
                LossBiquad{0.19536474372391743, -1.9443807737922464e-21, 5.7271831347466718e-42, -1.3214019391755742, 0.51676668289949168},
                LossBiquad{1.882030572966811, 0.20187647046787921, 0.061422510390911199, 0.48316180660687896, 0.6621677472187224}
            }}
        },
        // MIDI 105 A7; perceptual RMSE 0.0717026; compensated partial-cent RMS 2.07266
        LossPreset{
            105, 3538.2230231819567, 0.010380791723823489, 0.87958049303705133, 0.89713977789604782, 3, {{
                LossBiquad{0.3044859901114223, 0.58102897950646348, 0.27778668612299473, 0.15711681544493009, 0.0061848402959505068},
                LossBiquad{1.022044562474534, 0.88189179157978081, 0.72504034265623352, 0.88415123186896594, 0.7448254648415823},
                LossBiquad{0.9513518582940701, 0.039476334494305002, 0.74851306509766613, 0.039145012054931837, 0.70019624583110929},
                LossBiquad{1.0, 0.0, 0.0, 0.0, 0.0}
            }}
        },
        // MIDI 106 A#7; perceptual RMSE 0.0906675; compensated partial-cent RMS 2.73339
        LossPreset{
            106, 3748.6879501631079, 0.010419193982802631, 1.0208742179955408, 1.0416316341761414, 4, {{
                LossBiquad{0.46799608004059606, 0.67300618784408073, 0.24209501428856339, 0.35208795868307252, 0.031009323490167547},
                LossBiquad{2.1492705522776028, 0.3894311721250816, 0.017779664770177877, 1.1960348284815601, 0.36044656069130232},
                LossBiquad{0.93781958989057479, 0.17984431198101331, 0.74117703988981798, 0.17799235458496865, 0.68084858717643737},
                LossBiquad{0.25403385613335472, 0.50037619435000669, 0.24646903625109828, 0.00087889356731545194, 1.931671444699642e-07}
            }}
        },
        // MIDI 107 B7; perceptual RMSE 0.0266898; compensated partial-cent RMS 0.0279878
        LossPreset{
            107, 3971.5820888637736, 0.01041184323671299, 0.48413942707064433, 0.48434992256746501, 1, {{
                LossBiquad{0.51099400171690135, 0.79105557770063339, 0.28064403544536887, 0.57910721031405876, 0.0035864045488448125},
                LossBiquad{1.0, 0.0, 0.0, 0.0, 0.0},
                LossBiquad{1.0, 0.0, 0.0, 0.0, 0.0},
                LossBiquad{1.0, 0.0, 0.0, 0.0, 0.0}
            }}
        },
        // MIDI 108 C8; perceptual RMSE 0.0344749; compensated partial-cent RMS 0.18803
        LossPreset{
            108, 4207.6342771754062, 0.010358836260864999, 0.51393716091099384, 0.51449160907648139, 1, {{
                LossBiquad{0.49020045081507407, 0.70540352556504449, 0.21536527339235753, 0.41055969008239385, 0.00040955969008239394},
                LossBiquad{1.0, 0.0, 0.0, 0.0, 0.0},
                LossBiquad{1.0, 0.0, 0.0, 0.0, 0.0},
                LossBiquad{1.0, 0.0, 0.0, 0.0, 0.0}
            }}
        }
    }};
};




#endif /* loss_filter_hpp */
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



class StringModel {
    
public:
    
    struct SpatialPort {
        int size = 0;
        int max_index = 0;
        double position = 0.0;
        double accurate_index = 0.0;;
        
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
    double samplerate = 44100.0;
    
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
    double strike_point = 1.0 / 9.0;
    double pickup_point = 7.0 / 9.0;
    SpatialPort strike_port;
    SpatialPort pickup_port;
    
    // ======================== ========================
    // Fine-tuning coefficient
    // 微调系数
    // 弦特性阻抗
    // ======================== ========================
    double z = 0.0;
    
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
    LossFilter loss_filter;
    
    // ======================== ========================
    // State
    // 状态
    // ======================== ========================
    bool is_active = false;

    
public:
    
    StringModel(double samplerate,
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
        delay(samplerate / (2 * f0)),
        delay_int(delay),
        delay_frac(delay - delay_int),
        traveling_wave_max_index(delay_int - 1),
        samplerate(samplerate),
        strike_port(delay_int, strike_point),
        pickup_port(delay_int, pickup_point),
        fractional_filter(delay_frac),
        loss_filter(midi_n)
    {
        if(delay_int < 5)
            throw std::runtime_error("string_model: delay_int is too small: " + std::to_string(delay_int));
        
        left.resize(delay_int, 0.0);
        right.resize(delay_int, 0.0);
            
        left_head = 0;
        right_head = 0;
        
        left_boundary_point = &left[left_head];
        right_boundary_point = &right[get_i(traveling_wave_max_index, right_head)];
        
        z = 2.31;
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
            inject(hammer_force / (2 * z));
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
    
    inline double get_string_vs() {
        return strike_port.weight_a * left[get_i(strike_port.index_a, left_head)] + strike_port.weight_b * left[get_i(strike_port.index_b, left_head)] + strike_port.weight_a * right[get_i(strike_port.index_a, right_head)] + strike_port.weight_b * right[get_i(strike_port.index_b, right_head)];
    }
    
    inline float get_sample() {
        return pickup_port.weight_a * left[get_i(pickup_port.index_a, left_head)] +         pickup_port.weight_b * left[get_i(pickup_port.index_b, left_head)] + pickup_port.weight_a * right[get_i(pickup_port.index_a, right_head)] + pickup_port.weight_b * right[get_i(pickup_port.index_b, right_head)];
    }
    

    inline void system_reset() {
        
        left_head = 0;
        right_head = 0;
        is_active = false;
        
        left_boundary_point = &left[left_head];
        right_boundary_point = &right[get_i(traveling_wave_max_index, right_head)];
        
        std::fill(left.begin(), left.end(), 0.0);
        std::fill(right.begin(), right.end(), 0.0);
        
        loss_filter.system_reset();
        fractional_filter.system_reset();
        
        damper.system_reset();
        damper_active = false;
        
    }
    

private:
    inline void filter() {
        fractional_filter.process(*left_boundary_point);
        loss_filter.process(*left_boundary_point);
        if(damper_active) {
            damper.process(*left_boundary_point);
        }
    }
    
    
    int inactive_probe_count_ = 0;
    int activity_probe_counter_ = 0;
    inline void check_active() {
        constexpr int kProbeInterval = 64;
        constexpr int kInactiveProbeCount = 8;

        if (++activity_probe_counter_ < kProbeInterval) {
            return;
        }

        activity_probe_counter_ = 0;

        constexpr float kVelocityThreshold = 1.0e-6f;
        constexpr float kEnergyThreshold = kVelocityThreshold * kVelocityThreshold;

        if (activity_probe() < kEnergyThreshold) {
            if (++inactive_probe_count_ >= kInactiveProbeCount) {
                is_active = false;
                system_reset();
            }
        } else {
            inactive_probe_count_ = 0;
            is_active = true;
        }
    }

    inline float activity_probe() {
        constexpr int kProbeCount = 8;

        float energy = 0.0f;

        for (int i = 1; i <= kProbeCount; ++i) {
            const int index =
                (traveling_wave_max_index * i) / (kProbeCount + 1);

            const float l = left[get_i(index, left_head)];
            const float r = right[get_i(index, right_head)];

            energy += l * l + r * r;
        }

        return energy / static_cast<float>(kProbeCount);
    }
    
};

#endif /* string_model_hpp */


class KeyModel {
    
    // 弦锤耦合与tunning
    
public:
    
    double samplerate_ = 0.0;
    
    int midi_n_;
    int string_count_ = 3;
    
    HammerModel hammer_;
    std::array<StringModel, 3> strings_;
    
    std::array<double, 3> string_vs_ = {0.0, 0.0, 0.0};
    
    bool key_down_ = false;
    bool key_active_ = false;
    
    bool sustainpedal_active_ = false;
    
    const Configuration *configuration_ = nullptr;
    
    KeyModel(int midi_n,
             double sample_rate,
             int string_count,
             TunningPresets::Temperament temperament,
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
        configuration_(configuration)
    {}
    
    void key_movement() {
        
        sustainpedal_controller();
        
        for(int i = 0; i < string_count_; i++) {
            string_vs_[i] = strings_[i].get_string_vs();
        }
        
        double hammer_force = hammer_.hammer_movement(string_vs_[0]);
        
        for(int i = 0; i < string_count_; i++) {
            strings_[i].string_movement(hammer_force / double(string_count_));
        }
        
        check_active();
        
    }
    
    void trigger(double velocity_mps) {
        for(int i = 0; i < string_count_; i++) {
            string_vs_[i] = strings_[i].is_active = true;
        }
        hammer_.trigger(velocity_mps);
    }
    
    float get_sample() {
        float result = 0.0;
        for(auto& string : strings_) {
            result += string.get_sample();
        }
        return result;
    }
    
    void system_reset() {
        for(int i = 0; i < string_count_; i++) {
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
    
    void sustainpedal_controller() {
        // key 抬起且延音踏板未踩下则启动制音器
        const bool damper_should_touch = !key_down_ && !sustainpedal_active_;
        for (int index = 0; index < string_count_; ++index) {
            strings_[index].damper_active = damper_should_touch;
        }
    }
};

#endif /* key_model_hpp */

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

#ifndef piano_model_hpp
#define piano_model_hpp

#include <iostream>
#include <array>



class PianoModel {
    PianoModel(const PianoModel&) = delete;
    PianoModel& operator=(const PianoModel&) = delete;
public:
    
    static constexpr Configuration configuration{};
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

        piano_keys[index] = new KeyModel(
            midi_n,
            sample_rate,
            string_count,
            TunningPresets::Temperament::equal,
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
        float samples = 0.0;
        for(int i = 0; i < piano_keys.size(); i++) {
            samples += piano_keys[i]->get_sample();
        }
        return samples * (1.0f / 500.0f);
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
    }
    
    ~PianoModel() noexcept {
        for(int i = 0; i < piano_keys.size(); i++) {
            delete piano_keys[i];
        }
    }
};

#endif /* piano_model_hpp */
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

#ifndef engine_rate_eval_hpp
#define engine_rate_eval_hpp

class EngineEval {

    uint64_t start_ = 0;
    uint64_t end_ = 0;
    uint64_t actualNs_ = 0;
    double bufferNs_ = 0.0;
    double sample_rate_ = 44100.0;
    
    double instant_rate = 0.0;
public:
    
    explicit EngineEval(double sample_rate) : sample_rate_(sample_rate) {}
    
    inline void start_timing() noexcept {
        start_ = clock_gettime_nsec_np(CLOCK_UPTIME_RAW);
    }
    inline void end_timing(int frame_count) noexcept {
        end_ = clock_gettime_nsec_np(CLOCK_UPTIME_RAW);
        actualNs_ = end_ - start_;
        bufferNs_ = 1'000'000'000.0 * double(frame_count) / double(sample_rate_);
        const double current_rate = actualNs_ / bufferNs_;
        
        // 指数平滑
        instant_rate = 0.9 * instant_rate + 0.1 * current_rate;
    }
    inline double engine_rate() const noexcept {
        return instant_rate;
    }
};

#endif /* engine_rate_eval_hpp */

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
            key->key_active_ = true;
            // TODO: 此处为临时映射
            const float tem_velocity = command.value * (7.0f / 128.0f);
            key->trigger(tem_velocity);
            break;
        }

        case PianoCommandType::note_off:
            if (is_playable_note(command.note)) {
                bBpiano->piano_keys[command.note - k_lowest_midi_note]->key_down_ = false;
            }
            break;

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
// Hardware callback and test
// 硬件回调与测试
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
double get_engine_rate() noexcept {
    return engine_eval
        ? engine_eval->engine_rate()
        : 0.0;
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

// ============================================================================
// bBpiano Physical Modeling Engine
// Backend Command Line Interface
//
// Authorship:
//   Ziyang Tan & Zhuoran Chen
//
// Design & Architecture:
//   Ziyang Tan
//
// Conceptual Development & Engineering:
//   Ziyang Tan & Zhuoran Chen
//
// AI Disclosure:
//   AI tools were used for learning programming techniques and domain knowledge
//   in physical modeling pianos and synthesizers.
//
//   CORE CLI IMPLEMENTATION:
//   DESIGNED AND DEVELOPED WITHOUT AI-GENERATED CODE.
//
// Project Initiated:
//   April 1, 2026
// ============================================================================

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

// --------------------------------------------------------------------
// MARK: version & logo

static constexpr const char* version = "L0-Coda";
static constexpr const char* logo =
    R"(
        ┌────────────────────────────┐──╭────╮
        │                            │==│╲╱╲╱│
        │  bBpiano 0                 │==│╱╲╱╲│
        │  L0-Coda/260903            │==│╲╱╲╱│
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

    )";

// --------------------------------------------------------------------
// MARK: library


#include <cstdlib>
#include <exception>
#include <iostream>


int main(int argc, char* argv[]) {
    // --------------------------------------------------------------------
    // MARK: arg tmp test
//        char* testArgv[] = {
//            argv[0],
//            "-k"
//        };
//        argc = 2;
//        argv = testArgv;
    
//            char* testArgv[] = {
//                argv[0],
//                "-m",
//                "/Users/opusarc/Projects/XCodeProjects/bBpiano/Cli/bBpiano Lite/core/bbpl/resource/midi_samples/Fancy's selection/midi/Sonata in D Minor, K. 141_2018.midi"
//            };
//            argc = 3;
//            argv = testArgv;
//    char* testArgv[] = {
//                argv[0],
//                "-m",
//                "/Users/opusarc/Projects/XCodeProjects/bBpiano/Cli/bBpiano Lite/core/bbpl/resource/midi_samples/Fancy's selection/midi/Sonata No. 28 in A Major, Op. 101 (Complete)_2014.midi"
//            };
//            argc = 3;
//            argv = testArgv;
    
    try {
        
      return cli_entry(argc, argv, version, logo);
        
    } catch (const std::exception& error) {
        
      std::cerr << "bbpl: " << error.what() << '\n';
      return EXIT_FAILURE;
        
    } catch (...) {
        
      std::cerr << "bbpl: unknown fatal error.\n";
      return EXIT_FAILURE;
        
    }
}


