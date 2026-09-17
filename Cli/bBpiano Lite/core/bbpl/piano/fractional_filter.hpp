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
#ifndef fractional_filter_hpp
#define fractional_filter_hpp

#include <iostream>

class FractionalFilter {
    float a1 = 0.0;
    
    float x1 = 0.0;
    float y1 = 0.0;
    
public:
    
    bool bypass = false;

    FractionalFilter(double phase_delay_samples,
                     double target_omega) {
        if (std::abs(phase_delay_samples) < 1.0e-12) {
            bypass = true;
            a1 = 0.0f;
            return;
        }

        const double q =
            std::tan(0.5 * target_omega * phase_delay_samples) /
            std::tan(0.5 * target_omega);

        const double coefficient = (1.0 - q) / (1.0 + q);

        if (!std::isfinite(coefficient) ||
            std::abs(coefficient) >= 1.0) {
            throw std::runtime_error(
                "fractional_filter: invalid allpass coefficient");
        }

        a1 = static_cast<float>(coefficient);
    }
    
    inline void process(float& x) {
        if (bypass) {
            return;
        }
        // y = a1 * x + x1 - a1 * y1;
        const float y =
            x1 + a1 * (x - y1);

        x1 = x;
        y1 = y;
        x = y;
    }
    
    inline void system_reset() {
        x1 = 0.0;
        y1 = 0.0;
    }
    
    inline float state_energy() const noexcept {
        return 0.5f * (x1 * x1 + y1 * y1);
    }
};




#endif /* fractional_filter_hpp */
