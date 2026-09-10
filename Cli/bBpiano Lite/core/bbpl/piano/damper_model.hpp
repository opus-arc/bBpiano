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


#ifndef damper_model_hpp
#define damper_model_hpp

#include <iostream>


double Damper(double x, double &z1, double &z2) {
    // ==========================
    // Temporary Damper Controls
    // ==========================
    constexpr double lowLoss   = 0.020; // 低频耗散：0.020~0.070
    constexpr double highLoss  = 0.25;  // 高频抓取：0.25~0.70
    constexpr double damperMix = 0.38;  // 毛毡低通占比：0.30~0.70

    const double loopGain = 1.0 - lowLoss;
    const double wet = damperMix * highLoss;
    const double dry = 1.0 - wet;

    // Gentle second-order low-pass damper color.
    constexpr double b0 = 0.292893218813;
    constexpr double b1 = 0.585786437627;
    constexpr double b2 = 0.292893218813;

    constexpr double a1 = 0.000000000000;
    constexpr double a2 = 0.171572875254;

    const double y = b0 * x + z1;

    z1 = b1 * x - a1 * y + z2;
    z2 = b2 * x - a2 * y;

    return loopGain * (dry * x + wet * y);
}

#endif /* damper_model_hpp */
