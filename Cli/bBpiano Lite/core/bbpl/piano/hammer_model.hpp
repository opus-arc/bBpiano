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

#include "../configuration/configuration.hpp"

class HammerModel {
    
public: // 暂时 public
    
    // ======================== ======================== ========================
    // 双层 Kelvin–Voigt
    // ======================== ======================== ========================
    
    // ======================== ========================
    // Basic parameters
    // 基本参数
    // ======================== ========================
    int midi_n_ = 69;
    
    // ======================== ========================
    // Degrees of freedom of parameters
    // 参数自由度
    // ======================== ========================
    const Parameters::Hammer::LegacyFit::HammerPreset* hammer_presets = nullptr;
    
    // ======================== ========================
    // Cross-function update volume
    // 跨函数更新量
    // ======================== ========================
    
    
    double ts_ = 0.0;
    
    double w_a_1_ = 0.0;
    double w_b_1_ = 0.0;
    
    double hammer_v_ = 0.0;
    double middle_v_ = 0.0;
    double release_distance = 0.0;
    
    bool is_contacting_ = false;

    
public:
    
    HammerModel(double sample_rate,
                int midi_n,
                const Configuration* configuration) :
        ts_(1.0 / sample_rate),
        midi_n_(midi_n),
        hammer_presets(configuration->hammer_presets.find_preset(midi_n_))
    {}
    
    inline double hammer_movement(double string_v) {
        if(!is_contacting_)
            return 0.0;

        if(w_a_1_ < 0.0 || w_b_1_ < 0.0) {
            system_reset();
            return 0.0;
        }
        
        // 算 f (解出 middle_v)
        double hammer_force = solve_f(string_v);
        
        // 无根、NaN、零力或负力：接触结束。
        if (!std::isfinite(hammer_force) ||
            hammer_force <= 0.0) {
            system_reset();
            return 0.0;
        }
        
        // 击锤的空间移动
        if(hammer_v_ < 0 || release_distance < 0.0) {
            release_distance += hammer_v_ * ts_;
            if(release_distance > 0.0) {
                release_distance = 0.0;
            }
        }
        // 检查是否接触结束
        if(release_distance < hammer_presets->release_threshold_m) {
            system_reset();
            return 0.0;
        }
        
        // 更新压缩量
        w_a_1_ = w_a_1_ + (middle_v_ - string_v) * ts_;
        w_b_1_ = w_b_1_ + (hammer_v_ - middle_v_) * ts_;
        
        constexpr double compression_tolerance = 1.0e-12;

        // 任意一层将进入拉伸状态，说明单边接触已经结束。
        if (!std::isfinite(w_a_1_) ||
            !std::isfinite(w_b_1_) ||
            w_a_1_ < -compression_tolerance ||
            w_b_1_ < -compression_tolerance) {

            system_reset();
            return 0.0;
        }

        // 只消除舍入产生的极小负数。
        w_a_1_ = std::max(0.0, w_a_1_);
        w_b_1_ = std::max(0.0, w_b_1_);
        
        // 更新击锤的速度
        // 负力不更新！
        if(hammer_force > 0.0)
            hammer_v_ -= (hammer_force / hammer_presets->mass_kg) * ts_;
       
        
        return hammer_force;
    }
    
    inline void trigger(double hammer_v) {
        if(is_contacting_)
            return;
        
        system_reset();
        hammer_v_ = hammer_v;
        is_contacting_ = true;
    }
    
    inline void system_reset() {
        w_a_1_ = 0.0;
        w_b_1_ = 0.0;
        hammer_v_ = 0.0;
        middle_v_ = 0.0;
        release_distance = 0.0;
        is_contacting_ = false;
    }

private:
    
    inline double signed_pow(double x, double p) {
        return x >= 0 ? std::pow(x, p) : -std::pow(-x, p);
    }
    inline double scope_pow(double w, double exponent) {
        return std::pow(w > 0.0 ? w : 0.0, exponent);
    }

    inline double solve_f(double string_v) {
        double lower_limit = string_v - w_a_1_ / ts_;
        double upper_limit = hammer_v_ + w_b_1_ / ts_;
        double middle_v_suppose = (upper_limit + lower_limit) / 2.0;
        
        // 与 middle_v 无关的弹簧提出来
        const double spring_a = hammer_presets->k_a * scope_pow(w_a_1_, hammer_presets->p1);
        const double spring_b = hammer_presets->k_b * scope_pow(w_b_1_, hammer_presets->p3);
        
        const auto residual = [&](double middle_v) {
            const double f_a =
                spring_a + hammer_presets->c_a * signed_pow(middle_v - string_v, hammer_presets->p2);

            const double f_b =
                spring_b + hammer_presets->c_b * signed_pow(hammer_v_ - middle_v, hammer_presets->p4);

            return f_a - f_b;
        };
        
        if (!(lower_limit <= upper_limit)) {
            return 0.0;
        }
        
        const double residual_lower = residual(lower_limit);
        const double residual_upper = residual(upper_limit);

        if (!std::isfinite(residual_lower) ||
            !std::isfinite(residual_upper) ||
            residual_lower > 0.0 ||
            residual_upper < 0.0) {
            return 0.0;
        }
        
        for(int i = 0; i < 20; i++) {
            if(residual(middle_v_suppose) > 0) {
                upper_limit = middle_v_suppose;
            } else {
                lower_limit = middle_v_suppose;
            }
            middle_v_suppose = (upper_limit + lower_limit) / 2.0;
        }
        
        middle_v_ = middle_v_suppose;
        
        return hammer_presets->k_a * scope_pow(w_a_1_, hammer_presets->p1) +
            hammer_presets->c_a *  signed_pow(middle_v_suppose - string_v, hammer_presets->p2);
            
        
    }
};

#endif /* hammer_model_hpp */

