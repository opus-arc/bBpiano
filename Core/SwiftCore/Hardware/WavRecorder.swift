//
//  WavRecorder.swift
//  bBpiano
//
//  Created by opus arc on 2026/4/4.
//
//  AI Assisted
//

import Foundation
import AVFoundation


final class WavRecorder {
    
    // 麦克风也用全局唯一单例
    static let shared: WavRecorder = WavRecorder(sampleRate: 44100.0, channels: 1)

    // 这就是被写入的文件
    // 因为 start/stop 生命周期中可能为空
    private var file: AVAudioFile?
    
    // 用于描述
    private let format: AVAudioFormat
    
    // 避免在音频 render 线程做 IO
    // 串行化写文件操作（AVAudioFile 不是线程安全的）
    private let queue = DispatchQueue(label: "wav.writer.queue")

    
    
    // 总共录制了多少帧
    // 用于计算 duration = totalFrames / sampleRate
    private var totalFramesWritten: Int64 = 0
    
    // 需要录制的帧数
    private var maxRecordFrames: Int64?
    
    private var isRecording = false

    private init(sampleRate: Double, channels: AVAudioChannelCount) {
        self.format = AVAudioFormat(
            commonFormat: .pcmFormatFloat32, // DSP 友好， 无需归一化
            sampleRate: sampleRate,
            channels: channels,
            interleaved: false // 是否是交错结构
        )!
    }

    // MARK: - Start

    // 可能抛异常
    func start(
        url: URL, // 录制文件存储的地方
        maxDuration: Int? // 可以允许填写最大录制时长
    ) throws {
        let settings: [String: Any] = [
            AVFormatIDKey: kAudioFormatLinearPCM, // 也就是 WAV 格式
            AVSampleRateKey: format.sampleRate, // 与 format 保持一致
            AVNumberOfChannelsKey: format.channelCount, // 与 format 保持一致
            AVLinearPCMBitDepthKey: 32, // 录制精度 32 bit
            AVLinearPCMIsFloatKey: true,    // float PCM（而不是 int16）
            AVLinearPCMIsNonInterleaved: true  // 与 format 对齐
        ]
        
        // 如果规定了录制的时长，这里来计算需要录制的帧数
        if let sec = maxDuration {
            maxRecordFrames = Int64(sec * Int(format.sampleRate))
        } else {
            maxRecordFrames = nil
        }
        
        // 打开 WAV 文件
        file = try AVAudioFile(forWriting: url, settings: settings)
        
        // 开始录音
        isRecording = true
    }

    // MARK: - Append (核心函数)

    func virtualMicrophone(
        samples: UnsafePointer<Float>,
        frameCount: Int
    ) {
        
        // 入口
        guard isRecording else { return }

        // render线程不做 IO → 这里 copy
        let copied = Array(UnsafeBufferPointer(start: samples, count: frameCount))

        queue.async { [weak self] in
            guard let self,
                  let file = self.file,
                  self.isRecording else { return }

            guard let buffer = AVAudioPCMBuffer(
                pcmFormat: self.format,
                frameCapacity: AVAudioFrameCount(frameCount)
            ) else { return }

            buffer.frameLength = AVAudioFrameCount(frameCount)

            let dst = buffer.floatChannelData![0]

            copied.withUnsafeBufferPointer { src in
                dst.assign(from: src.baseAddress!, count: frameCount)
            }

            do {
                try file.write(from: buffer)

                // 累计帧数
                self.totalFramesWritten += Int64(frameCount)

                // 如果规定了时长，判断是否达到时间
                if let maxFrames = self.maxRecordFrames,
                   self.totalFramesWritten >= maxFrames {

                    print("Auto stop triggered")

                    self.isRecording = false
                    self.file = nil
                }

            } catch {
                print("WAV write error:", error)
            }
        }
    }

    // MARK: - Stop

    func stop() {
        queue.sync {
            isRecording = false
            file = nil
        }
    }
}
