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

#ifndef string_model_hpp
#define string_model_hpp

#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <cmath>
#include <numbers>

#include "./damper_model.hpp"
#include "./fractional_filter.hpp"
#include "./loss_filter.hpp"

#include "../configuration/configuration.hpp"

class StringModel {
    
public:
    
    struct SpatialPort {
        int size = 0;
        int max_index = 0;
        double position = 0.0;
        double accurate_index = 0.0;;
        
        int index_a = 0;
        int index_b = 0;
        float weight_a = 1.0f;
        float weight_b = 0.0f;
        
        SpatialPort(int size, double position) : size(size), position(position) {
            if(size < 2)
                throw std::runtime_error("string_model: size is too small, size: " + std::to_string(size));
            if(position > 1.0 || position <= 0.0)
                throw std::runtime_error("string_model: position isn't good, position: " + std::to_string(position));
            max_index = size - 1;
            accurate_index = max_index * position;
            
            index_a = std::ceil(accurate_index);
            index_b = index_a - 1;
            
            weight_a = accurate_index - index_b;
            weight_b = 1 - weight_a;
        }
    };
    
public:
    // ======================== ========================
    // Configuration
    // 配置
    // ======================== ========================
    const Configuration *configuration_ = nullptr;
    
    // ======================== ========================
    // Basic member value
    // 基础成员变量
    // ======================== ========================
    int midi_n_ = 69;
    double f0 = 440.0;
    double samplerate = 44100.0;
    
    // ======================== ========================
    // Filters
    // 滤波器 (为了计算 group delay 放在前面)
    // ======================== ========================
    LossFilter loss_filter;
    double loss_phase_delay = 0.0;
    
    // ======================== ========================
    // Delay data
    // 延迟数据
    // ======================== ========================
    double delay = 0.0;
    int delay_int = 0;
    double delay_frac = 0.0;
    int traveling_wave_max_index = 0;

    // ======================== ========================
    // Waveguide
    // 波导
    // ======================== ========================
    std::vector<float> left;
    std::vector<float> right;
    int left_head = 0;
    int right_head = 0;
    float* left_boundary_point = nullptr;
    float* right_boundary_point = nullptr;
    
    // ======================== ========================
    // Frac position
    // 分数格点
    // ======================== ========================
    double strike_point = 1.0 / 9.4;
    SpatialPort strike_port;
    
    // ======================== ========================
    // Fine-tuning coefficient
    // 微调系数
    // 弦特性阻抗
    // ======================== ========================
    double z_ = 0.0;
    
    // ======================== ========================
    // Damper
    // 制音器
    // ======================== ========================
    Damper damper;
    bool damper_active = false;
    
    // ======================== ========================
    // Filters
    // 滤波器
    // ======================== ========================
    FractionalFilter fractional_filter;
    
    // ======================== ========================
    // State
    // 状态
    // ======================== ========================
    bool is_active = false;

    
public:
    
    StringModel(double sample_rate,
                int midi_n,
                TunningPresets::Temperament temperament,
                TunningPresets::StringIndex string_index,
                const Configuration* configuration) :
        configuration_(configuration),
        midi_n_(midi_n),
        f0(configuration->tuning_presets
           .get_frequency(midi_n_,
                          temperament,
                          string_index)),
        samplerate(sample_rate),
        loss_filter(midi_n_),
        loss_phase_delay(loss_filter.get_phase_delay(sample_rate, f0)),
        delay((sample_rate / f0 - loss_phase_delay) / 2.0),
        // delay_int 是数组节点数；真实单程整数延迟为 delay_int - 1。
        delay_int(static_cast<int>(std::floor(delay)) + 1),
        // fractional filter 每圈一次，因此承担完整 round-trip residual。
        delay_frac(2.0 * (delay - std::floor(delay))),
        traveling_wave_max_index(delay_int - 1),
        strike_port(delay_int, strike_point),
        fractional_filter(delay_frac, 2.0 * std::numbers::pi_v<double> * f0 / sample_rate)
    {
        
        if(delay_int < 4)
            throw std::runtime_error("string_model: delay_int is too small: " + std::to_string(delay_int));
        
        left.resize(delay_int, 0.0f);
        right.resize(delay_int, 0.0f);
            
        left_head = 0;
        right_head = 0;
        
        left_boundary_point = &left[left_head];
        right_boundary_point = &right[get_i(traveling_wave_max_index, right_head)];
        
        z_ = configuration->string_impedance_presets.get_characteristic_impedance(midi_n_);
    }
    
    inline void propagate() {
        // ======================== ========================
        // Reverse moving boundary index
        // 反向移动边界下标
        // ======================== ========================
        if(left_head == traveling_wave_max_index)
            left_head = 0;
        else
            left_head++;
        if(right_head == 0)
            right_head = traveling_wave_max_index;
        else
            right_head--;
        
        // ======================== ========================
        // Boundary reflection
        // 边界反射
        // ======================== ========================
        right[get_i(0, right_head)] = -left[get_i(0, left_head)];
        left[get_i(traveling_wave_max_index, left_head)] =
            -right[get_i(traveling_wave_max_index, right_head)];
        
        // ======================== ========================
        // Update boundary point
        // 更新边界指针
        // ======================== ========================
        left_boundary_point = &right[get_i(0, right_head)];
        right_boundary_point = &left[get_i(traveling_wave_max_index, left_head)];
    }
    
    inline int get_i(int real_index, int head) {
        return real_index + head <= traveling_wave_max_index ?
               real_index + head :
               real_index + head - delay_int;
    }

    inline void string_movement(double hammer_force) {

        const bool excited = hammer_force > 0.0;

        if (excited) {
            inject(hammer_force / (2 * z_));
        }

        propagate();
        filter();

        check_active();

        if (excited) {
            is_active = true;
        }
    }
    
    inline void inject(double inject_v) {
        left[get_i(strike_port.index_a, left_head)] += inject_v * strike_port.weight_a;
        left[get_i(strike_port.index_b, left_head)] += inject_v * strike_port.weight_b;
        right[get_i(strike_port.index_a, right_head)] += inject_v * strike_port.weight_a;
        right[get_i(strike_port.index_b, right_head)] += inject_v * strike_port.weight_b;
    }
    
    inline double get_bridge_force() {
        return -2.0 * z_ * static_cast<double>(*right_boundary_point);
    }
    
    inline double get_string_vs() {
        return strike_port.weight_a * left[get_i(strike_port.index_a, left_head)] + strike_port.weight_b * left[get_i(strike_port.index_b, left_head)] + strike_port.weight_a * right[get_i(strike_port.index_a, right_head)] + strike_port.weight_b * right[get_i(strike_port.index_b, right_head)];
    }

    inline void system_reset() {
        left_head = 0;
        right_head = 0;

        is_active = false;
        damper_active = false;

        inactive_probe_count_ = 0;
        activity_probe_counter_ = 0;

        std::fill(left.begin(), left.end(), 0.0f);
        std::fill(right.begin(), right.end(), 0.0f);

        left_boundary_point = &left[left_head];
        right_boundary_point = &right[get_i(traveling_wave_max_index, right_head)];

        loss_filter.system_reset();
        fractional_filter.system_reset();
        damper.system_reset();
    }
    

private:
    inline void filter() {
        fractional_filter.process(*left_boundary_point);
        loss_filter.process(*left_boundary_point);
        if(damper_active) {
            damper.process(*left_boundary_point);
        }
    }
    
    int inactive_probe_count_ = 0;
    int activity_probe_counter_ = 0;
    inline void check_active() {
        constexpr int kProbeInterval = 64;
        constexpr int kInactiveProbeCount = 8;

        if (++activity_probe_counter_ < kProbeInterval) {
            return;
        }

        activity_probe_counter_ = 0;

        constexpr float kVelocityThreshold = 1.0e-6f;
        constexpr float kEnergyThreshold = kVelocityThreshold * kVelocityThreshold;

        if (activity_probe() < kEnergyThreshold) {
            if (++inactive_probe_count_ >= kInactiveProbeCount) {
                is_active = false;
                system_reset();
            }
        } else {
            inactive_probe_count_ = 0;
            is_active = true;
        }
    }

    inline float activity_probe() {
        constexpr int kProbeCount = 8;

        float energy = 0.0f;

        for (int i = 1; i <= kProbeCount; ++i) {
            const int index =
                (traveling_wave_max_index * i) / (kProbeCount + 1);

            const float& l = left[get_i(index, left_head)];
            const float& r = right[get_i(index, right_head)];

            energy += l * l + r * r;
        }

        return energy / static_cast<float>(kProbeCount);
    }
    
};

#endif /* string_model_hpp */
