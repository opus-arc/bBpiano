//
//  SoundCard.swift
//  bBpiano
//
//  Created by opus arc on 2026/4/3.
//
//  AI Assisted
//  But actually I think it's not good at this.
//  So then I do it myself.
//

import AVFoundation

public class SoundCard {
    // AVAudioEngine 启动的时候通常比较慢，此处全局使用唯一的实例
    public static let shared = SoundCard()

    private let engine = AVAudioEngine()
    
    // 这里用下划线只是用于增强可读性，44_100.0 == 44100.0
    private let sampleRate: Double = 44_100.0

    
    // 最大振幅，这里用的比较保守，相当于 gain
    private var amplitudeLimiter: Float = 0.3
    

    // 使用 lazy 方式，指第一次用到这个属性时才创建它，不是创建 SoundCard 时立刻创建。
    // 因为这个节点的初始化比较复杂，而且闭包里要引用 self。
    // 用 lazy 可以避免初始化时机上的问题。
    private lazy var sourceNode: AVAudioSourceNode = {
        AVAudioSourceNode {
            [weak self] // 这里使用弱引用是为了避免循环引用
            _,
            _,
            frameCount, // 多少帧
            audioBufferList // 核心的流输出区域
            -> OSStatus in
            guard let self else { return noErr }
            return self.render(
                frameCount: frameCount,
                audioBufferList: audioBufferList
            )
        }
    }()

    // 单例初始化用 private，不这样做就会破坏单例模式，避免错用
    private init() {
        let format = AVAudioFormat(
            commonFormat: .pcmFormatFloat32, // 音频常用浮点数，因为这样做更自然
            sampleRate: sampleRate,
            channels: 1,
            interleaved: false // 双声道时启用，这里指交错布局
        )!

        // 挂载
        // sourceNode -> mainMixerNode -> outputNode
        engine.attach(sourceNode)
        engine.connect(sourceNode, to: engine.mainMixerNode, format: format)
    }

    private func render(
        frameCount: AVAudioFrameCount,
        audioBufferList: UnsafeMutablePointer<AudioBufferList>
    ) -> OSStatus {
        
        // 复制一份 bufferList
        let bufferList = UnsafeMutableAudioBufferListPointer(audioBufferList)

        // 通常来说只有一个 buffer，但是通过遍历的方式能更通用
        for buffer in bufferList {
            
            // 取出底层数据指针，这是裸内存地址，指向真正的音频采样区。
            // 这里用 guard let 就是为了确保数据不为空
            // 这一步获取了这块内存上的数据
            guard let mData = buffer.mData else { continue }
            
            // 这里 assumingMemoryBound 的用法是：
            // “我知道这块内存其实就是 Float 类型，请按 Float* 来看待它。”
            // 也就是说我们此处我们保证计算机用 float 的格式看待这个数据
            let out = mData.assumingMemoryBound(to: Float.self)

            get_next_buffer(
                out,
                Int32(frameCount),
                Double(amplitudeLimiter)
            )
            
            // 这里录音和音频函数计算用的不是一个线程
            // 所以我把虚拟麦克风直接放在这个位置
//            WavRecorder.shared
//                .virtualMicrophone(
//                    samples: out,
//                    frameCount: Int(frameCount)
//                )

            
        }

        return noErr
    }

    public func start() throws {
        if engine.isRunning { return }
        try engine.start()
    }

    public func stop() {
        engine.stop()
    }

    public func setMaxAmplitude(_ value: Float) {
        amplitudeLimiter = max(0.0, min(value, 1.0))
    }

}
