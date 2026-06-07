//
//  soundCard.hpp
//  bBpiano Lite
//
//  Created by opus arc on 2026/6/7.
//

#ifndef soundCard_hpp
#define soundCard_hpp

#include "../external/RtAudio.h"
#include "../core/controller.hpp"

#include <algorithm>
#include <stdexcept>

class SoundCard {
public:
    
    static SoundCard& shared() {
        static SoundCard instance;
        return instance;
    }

    SoundCard(const SoundCard&) = delete;
    SoundCard& operator=(const SoundCard&) = delete;

    void start() {
        if (audio_.isStreamOpen() && audio_.isStreamRunning()) {
            return;
        }

        if (!audio_.isStreamOpen()) {
            openStream();
        }

        audio_.startStream();
    }

    
    void stop() {
        if (audio_.isStreamOpen() && audio_.isStreamRunning()) {
            audio_.stopStream();
        }
    }

    bool isRunning() const {
        return audio_.isStreamOpen() && audio_.isStreamRunning();
    }

    void setAmplitudeLimiter(float value) {
        amplitudeLimiter_ = std::clamp(value, 0.0f, 1.0f);
    }

    float amplitudeLimiter() const {
        return amplitudeLimiter_;
    }

private:
    SoundCard() = default;

    ~SoundCard() {
        try {
            stop();
            if (audio_.isStreamOpen()) {
                audio_.closeStream();
            }
        } catch (...) {
            // 析构函数里不抛异常。
        }
    }

    void openStream() {
        if (audio_.getDeviceCount() == 0) {
            throw std::runtime_error("No audio output device found.");
        }

        RtAudio::StreamParameters outputParams;
        outputParams.deviceId = audio_.getDefaultOutputDevice();
        outputParams.nChannels = channelCount_;
        outputParams.firstChannel = 0;

        unsigned int bufferFrames = bufferFrames_;

        audio_.openStream(
            &outputParams,
            nullptr,
            RTAUDIO_FLOAT32,
            sampleRate_,
            &bufferFrames,
            &SoundCard::renderCallback,
            this
        );
    }

    static int renderCallback(
        void* outputBuffer,
        void* /* inputBuffer */,
        unsigned int nFrames,
        double /* streamTime */,
        RtAudioStreamStatus status,
        void* userData
    ) {
        auto* self = static_cast<SoundCard*>(userData);
        auto* out = static_cast<float*>(outputBuffer);

        if (status) {
            // 这里不做复杂日志；audio callback 应尽量轻。
        }

        if (self == nullptr || out == nullptr) {
            return 0;
        }

        get_next_buffer(
            out,
            static_cast<int>(nFrames),
            static_cast<double>(self->amplitudeLimiter_)
        );

        return 0;
    }

private:
    RtAudio audio_;

    static constexpr unsigned int sampleRate_ = 44'100;

    static constexpr unsigned int channelCount_ = 1;

    // 小 buffer 低延迟，但 CPU 压力更高。
    // 256 对实时乐器通常是一个比较稳妥的起点。
    static constexpr unsigned int bufferFrames_ = 256;

    float amplitudeLimiter_ = 0.3f;
};

#endif /* soundCard_hpp */
