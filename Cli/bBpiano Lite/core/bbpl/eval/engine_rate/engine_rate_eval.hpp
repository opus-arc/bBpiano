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

#ifndef engine_rate_eval_hpp
#define engine_rate_eval_hpp

class EngineEval {

    uint64_t start_ = 0;
    uint64_t end_ = 0;
    uint64_t actualNs_ = 0;
    double bufferNs_ = 0.0;
    double sample_rate_ = 44100.0;
    
    double instant_rate = 0.0;
public:
    
    explicit EngineEval(double sample_rate) : sample_rate_(sample_rate) {}
    
    inline void start_timing() noexcept {
        start_ = clock_gettime_nsec_np(CLOCK_UPTIME_RAW);
    }
    inline void end_timing(int frame_count) noexcept {
        end_ = clock_gettime_nsec_np(CLOCK_UPTIME_RAW);
        actualNs_ = end_ - start_;
        bufferNs_ = 1'000'000'000.0 * double(frame_count) / double(sample_rate_);
        const double current_rate = actualNs_ / bufferNs_;
        
        // 指数平滑
        instant_rate = 0.9 * instant_rate + 0.1 * current_rate;
    }
    inline double engine_rate() const noexcept {
        return instant_rate;
    }
};

#endif /* engine_rate_eval_hpp */
