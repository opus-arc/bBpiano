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




class Soundcard {
    
    AudioUnit audio_unit_ = nullptr;
    bool running_ = false;
    
public:
    // 禁止资源拥有类复制
    Soundcard(const Soundcard&) = delete;
    Soundcard& operator=(const Soundcard&) = delete;
    
    Soundcard(double sample_rate) {
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
        AudioBufferList* io_data)
    {
        // 恢复指针类型，从万用指针 void* 变成 Soundcard*
        auto* soundcard = static_cast<Soundcard*>(input_proc_ref_con);
        
        // 恢复之后就能直接调用 Soundcard 类下面的 render 函数
        return soundcard->render(
            in_number_frames,
            io_data);
    }
    
    // ======================== ======================== ========================
    // Fill the buffer
    // 用 PCM 填充 Core Audio 给的音频缓冲区
    // ======================== ======================== ========================
    OSStatus render(UInt32 frames_count, AudioBufferList* data);
    
    void start() {
        // 函数幂等化：
        /// 设计初始化的函数不会重复启动
        if(running_)
            return ;
        
        OSStatus status = AudioUnitInitialize(audio_unit_);
        if (status != noErr) {
            throw std::runtime_error("Failed to initialize audio unit.");
        }
        status = AudioOutputUnitStart(audio_unit_);
        if (status != noErr) {
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
        
        AudioOutputUnitStop(audio_unit_);
        AudioUnitUninitialize(audio_unit_);
        
        running_ = false;
    }
    
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













