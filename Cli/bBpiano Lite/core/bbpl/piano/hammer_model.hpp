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

#ifndef hammer_model_hpp
#define hammer_model_hpp

#include <iostream>
#include <cmath>

class HammerModel {
    
    static constexpr double k_a = 6000;
    static constexpr double k_b = 6000;
    
    static constexpr double c_a = 2;
    static constexpr double c_b = 3;
    
    static constexpr double p1 = 2.0;
    static constexpr double p2 = 2.0;
    static constexpr double p3 = 2.0;
    static constexpr double p4 = 2.0;
    
    static constexpr double hammer_m_ = 9e-3; // 9g
    static constexpr double release_threshold = -5e-3; // 5mm
    
    // --------------------------------------
    

    
    double ts_ = 0.0;
    
    double w_a_1_ = 0.0;
    double w_b_1_ = 0.0;
    
    double hammer_v_ = 0.0;
    double middle_v_ = 0.0;
    double release_distance = 0.0;
    
    bool is_contacting_ = false;
    

    
public:
    
    HammerModel(double sample_rate) : ts_(1.0 / sample_rate) {}
    
    inline double hammer_movement(double string_v) {
        if(!is_contacting_)
            return 0.0;
        
        // 算 f (解出 middle_v)
        double hammer_force = solve_f(string_v);
        // 击锤的空间移动
        if(hammer_v_ < 0 || release_distance < 0.0) {
            release_distance += hammer_v_ * ts_;
            if(release_distance > 0.0) {
                release_distance = 0.0;
            }
        }
        // 检查是否接触结束
        if(release_distance < release_threshold) {
            system_reset();
            return 0.0;
        }
        
        // 更新压缩量
        w_a_1_ = w_a_1_ + (middle_v_ - string_v) * ts_;
        w_b_1_ = w_b_1_ + (hammer_v_ - middle_v_) * ts_;
        
        // 更新击锤的速度
        hammer_v_ -= (hammer_force / hammer_m_) * ts_;
        
        return hammer_force;
    }
    
    inline void trigger(double hammer_v) {
        if(is_contacting_)
            return;
        
        system_reset();
        hammer_v_ = hammer_v;
        is_contacting_ = true;
    }

private:
    
    inline void system_reset() {
        w_a_1_ = 0.0;
        w_b_1_ = 0.0;
        hammer_v_ = 0.0;
        middle_v_ = 0.0;
        release_distance = 0.0;
        is_contacting_ = false;
    }

    inline double solve_f(double string_v) {
        double upper_limit = 20;
        double lower_limit = -20;
        double middle_v_suppose = (upper_limit + lower_limit) / 2.0;
        for(int i = 0; i < 20; i++) {
            if((k_a * std::pow(w_a_1_, p1) + c_a *  signed_pow(middle_v_suppose - string_v, p2) -
                (k_b * std::pow(w_b_1_, p3) + c_b * signed_pow(hammer_v_ - middle_v_suppose, p4))) > 0) {
                upper_limit = middle_v_suppose;
            } else {
                lower_limit = middle_v_suppose;
            }
            middle_v_suppose = (upper_limit + lower_limit) / 2.0;
        }
        middle_v_ = middle_v_suppose;
        return k_a * std::pow(w_a_1_, p1) + c_a * signed_pow(middle_v_ - string_v, p2);
    }
    
    inline double signed_pow(double x, double p) {
        return x >= 0 ? std::pow(x, p) : -std::pow(-x, p);
    }
};

#endif /* hammer_model_hpp */

