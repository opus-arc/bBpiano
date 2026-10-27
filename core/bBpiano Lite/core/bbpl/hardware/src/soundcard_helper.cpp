#include <cstring>
#include <limits>

#include "../soundcard_helper.hpp"
#include "../../piano_controller.hpp"

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
            0.90);
        
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
