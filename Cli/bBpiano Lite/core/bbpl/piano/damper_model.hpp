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
