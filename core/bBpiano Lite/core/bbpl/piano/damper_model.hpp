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
    
    float z1 = 0.0f;
    float z2 = 0.0f;

public:
    
    inline void process(float &x) {
        // ==========================
        // Temporary Damper Controls
        // ==========================
        constexpr float lowLoss   = 0.020f; // 低频耗散：0.020~0.070
        constexpr float highLoss  = 0.25f;  // 高频抓取：0.25~0.70
        constexpr float damperMix = 0.38f;  // 毛毡低通占比：0.30~0.70

        constexpr float loopGain = 1.0f - lowLoss;
        constexpr float wet = damperMix * highLoss;
        constexpr float dry = 1.0f - wet;

        constexpr float dryGain = loopGain * dry;
        constexpr float wetGain = loopGain * wet;

        // Gentle second-order low-pass damper color.
        constexpr float b0 = 0.292893218813f;
        constexpr float b1 = 0.585786437627f;
        constexpr float b2 = 0.292893218813f;
        constexpr float a2 = 0.171572875254f;

        const float y = b0 * x + z1;

        z1 = b1 * x + z2;
        z2 = b2 * x - a2 * y;

        x = dryGain * x + wetGain * y;
    }
    
    inline void system_reset() noexcept {
        z1 = 0.0f;
        z2 = 0.0f;
    }
    
    inline float state_energy() const noexcept {
        return 0.5f * (z1 * z1 + z2 * z2);
    }
};




#endif /* damper_model_hpp */
