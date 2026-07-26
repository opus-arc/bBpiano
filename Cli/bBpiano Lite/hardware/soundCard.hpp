//
//  soundCard.hpp
//  bBpiano Lite
//
//  Created by opus arc on 2026/6/7.
//

#ifndef soundCard_hpp
#define soundCard_hpp

#include "../core/controller.hpp"

#include <AudioToolbox/AudioToolbox.h>

#include <atomic>
#include <cstdint>
#include <cstring>
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
        if (running_.load()) {
            return;
        }

        if (queue_ == nullptr) {
            openStream();
        }

        const OSStatus status = AudioQueueStart(queue_, nullptr);
        if (status != noErr) {
            throw std::runtime_error("Failed to start CoreAudio output.");
        }

        running_.store(true);
    }

    void stop() {
        if (!running_.load()) {
            return;
        }

        running_.store(false);

        if (queue_ != nullptr) {
            AudioQueueStop(queue_, true);
        }
    }

    bool isRunning() const {
        return running_.load();
    }

private:
    SoundCard() = default;

    ~SoundCard() {
        try {
            stop();
            if (queue_ != nullptr) {
                AudioQueueDispose(queue_, true);
                queue_ = nullptr;
            }
        } catch (...) {
            // 析构函数里不抛异常。
        }
    }

    void openStream() {
        AudioStreamBasicDescription format {};
        format.mSampleRate = sampleRate_;
        format.mFormatID = kAudioFormatLinearPCM;
        format.mFormatFlags = kAudioFormatFlagIsFloat | kAudioFormatFlagIsPacked | kAudioFormatFlagIsNonInterleaved;
        format.mBytesPerPacket = sizeof(float);
        format.mFramesPerPacket = 1;
        format.mBytesPerFrame = sizeof(float);
        format.mChannelsPerFrame = channelCount_;
        format.mBitsPerChannel = 32;

        OSStatus status = AudioQueueNewOutput(
            &format,
            &SoundCard::renderCallback,
            this,
            nullptr,
            nullptr,
            0,
            &queue_
        );

        if (status != noErr || queue_ == nullptr) {
            queue_ = nullptr;
            throw std::runtime_error("Failed to create CoreAudio output queue.");
        }

        for (AudioQueueBufferRef& buffer : buffers_) {
            status = AudioQueueAllocateBuffer(
                queue_,
                bufferByteSize_,
                &buffer
            );

            if (status != noErr || buffer == nullptr) {
                throw std::runtime_error("Failed to allocate CoreAudio output buffer.");
            }

            fillBuffer(buffer);

            status = AudioQueueEnqueueBuffer(
                queue_,
                buffer,
                0,
                nullptr
            );

            if (status != noErr) {
                throw std::runtime_error("Failed to enqueue CoreAudio output buffer.");
            }
        }
    }

    static void renderCallback(
        void* userData,
        AudioQueueRef queue,
        AudioQueueBufferRef buffer
    ) {
        auto* self = static_cast<SoundCard*>(userData);
        if (self == nullptr || queue == nullptr || buffer == nullptr) {
            return;
        }

        self->fillBuffer(buffer);

        if (self->running_.load()) {
            AudioQueueEnqueueBuffer(queue, buffer, 0, nullptr);
        }
    }

    void fillBuffer(AudioQueueBufferRef buffer) {
        if (buffer == nullptr || buffer->mAudioData == nullptr) {
            return;
        }

        auto* out = static_cast<float*>(buffer->mAudioData);

        get_next_buffer(
            out,
            static_cast<int>(bufferFrames_),
            1.0
        );

        buffer->mAudioDataByteSize = bufferByteSize_;
    }

private:
    AudioQueueRef queue_ = nullptr;

    static constexpr double sampleRate_ = 44'100.0;
    static constexpr unsigned int channelCount_ = 1;
    static constexpr unsigned int bufferFrames_ = 256;
    static constexpr unsigned int bufferCount_ = 3;
    static constexpr UInt32 bufferByteSize_ = static_cast<UInt32>(bufferFrames_ * channelCount_ * sizeof(float));

    AudioQueueBufferRef buffers_[bufferCount_] {};
    std::atomic<bool> running_ { false };
};

#endif /* soundCard_hpp */
