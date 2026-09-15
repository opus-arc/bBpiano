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
