// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
// [ZERO AI-GENERATED CODE]
// Every line in this file is written and understood by its author. Every result
// is reproducible, every assumption is open to inspection, and every
// implementation stands open to criticism and challenge. AI may be used for
// non-core, replaceable, engineering work; this file, however, contains core
// logic that the author considers necessary to understand firsthand, explain
// line by line, and take full responsibility for, and is therefore implemented
// entirely by hand.
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

#include <cmath>
#include <numbers>
#include <stdexcept>
#include <string>
#include <vector>

#include "./damper_model.hpp"
#include "./dispersion_filter.hpp"
#include "./fractional_filter.hpp"

#include "./bridge_model.hpp"

#include "../configuration/configuration.hpp"

class StringModel {

public:
    struct SpatialPort {
        int size = 0;
        int max_index = 0;
        double position = 0.0;
        double accurate_index = 0.0;

        int index_a = 0;
        int index_b = 0;
        float weight_a = 1.0f;
        float weight_b = 0.0f;

        SpatialPort(int size, double position) : size(size), position(position) {
            if (size < 2)
                throw std::runtime_error("string_model: size is too small, size: " +
                                         std::to_string(size));
            if (position > 1.0 || position <= 0.0)
                throw std::runtime_error("string_model: position isn't good, position: " +
                                         std::to_string(position));
            max_index = size - 1;
            accurate_index = max_index * position;

            index_a = std::ceil(accurate_index);
            index_b = index_a - 1;

            weight_a = accurate_index - index_b;
            weight_b = 1 - weight_a;
        }
    };
    
    struct Waveguide2D {
        float v_x;
        float v_y;
        
        Waveguide2D operator-() const {
            return {-v_x, -v_y};
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
    double sample_rate_ = 44100.0;
    double internal_sample_rate_ = 44100.0;
    
    // ======================== ========================
    // Bridge
    // 音桥
    // ======================== ========================
    BridgeModel* bridge_;

    // ======================== ========================
    // Filters
    // 滤波器 (为了计算 group delay 放在前面)
    // ======================== ========================
    DispersionFilter dispersion_filter_x;
    DispersionFilter dispersion_filter_y;

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
    std::vector<Waveguide2D> left;
    std::vector<Waveguide2D> right;
    int left_head = 0;
    int right_head = 0;
    Waveguide2D *left_boundary_point = nullptr;
    Waveguide2D *right_boundary_point = nullptr;

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
    double z_s_ = 0.0;

    // ======================== ========================
    // Damper
    // 制音器
    // ======================== ========================
    Damper damper_x;
    Damper damper_y;
    bool damper_active = false;

    // ======================== ========================
    // Filters
    // 滤波器
    // ======================== ========================
    FractionalFilter fractional_filter_x;
    FractionalFilter fractional_filter_y;

    // ======================== ========================
    // State
    // 状态
    // ======================== ========================
    bool is_active = false;

    // ======================== ========================
    // Pre-compute
    // 预计算
    // ======================== ========================
    double c_z = 0.0;

public:
    StringModel(double sample_rate, int midi_n,
                TunningPresets::Temperament temperament,
                TunningPresets::StringIndex string_index,
                const Configuration *configuration,
                BridgeModel* bridge)
    : configuration_(configuration), midi_n_(midi_n), bridge_(bridge),
    f0(configuration->tuning_presets.get_frequency(midi_n_, temperament,
                                                   string_index)),
    sample_rate_(sample_rate),
    internal_sample_rate_(midi_n_ >= 88 ? 2.0 * sample_rate_
                          : sample_rate_),
    dispersion_filter_x(f0),
    dispersion_filter_y(f0),
    delay(dispersion_filter_x.get_loop_delay_samples() / 2.0 - configuration->bridge_transmission_presets.get_one_way_delay_compensation_internal_samples()),
    // delay_int 是数组节点数；真实单程整数延迟为 delay_int - 1。
    delay_int(static_cast<int>(std::floor(delay)) + 1),
    // fractional filter 每圈一次，因此承担完整 round-trip residual。
    delay_frac(2.0 * (delay - std::floor(delay))),
    traveling_wave_max_index(delay_int - 1),
    strike_port(delay_int, strike_point),
    fractional_filter_x(delay_frac, 2.0 * std::numbers::pi_v<double> * f0 / internal_sample_rate_),
    fractional_filter_y(delay_frac, 2.0 * std::numbers::pi_v<double> * f0 / internal_sample_rate_)
    {

        if (delay_int < 4)
            throw std::runtime_error("string_model: delay_int is too small: " +
                                     std::to_string(delay_int) + ", midi_n: " + std::to_string(midi_n_));
        if (strike_port.index_a + 1 > traveling_wave_max_index ||
            strike_port.index_a - 1 < 0) {
            throw std::runtime_error("string_model: next_index doesn't exist: " +
                                     std::to_string(delay_int));
        }
        if (std::abs(dispersion_filter_x.design_sample_rate() -
                     internal_sample_rate_) > 0.5) {
            throw std::runtime_error(
                                     "string_model: dispersion preset sample rate mismatch");
        }

        std::cout
                << "midi_n: " << midi_n_
            << ", delay_int: " << delay_int
                << ", delay: " << delay << '\n';

        left.resize(delay_int, {0.0f, 0.0f});
        right.resize(delay_int, {0.0f, 0.0f});

        left_head = 0;
        right_head = 0;

        left_boundary_point = &left[left_head];
        right_boundary_point = &right[get_i(traveling_wave_max_index,
                                            right_head)];

        z_s_ = configuration->string_impedance_presets.get_characteristic_impedance(
                                                                                  midi_n_);
        c_z = 1.0 / (2.0 * z_s_);
    }

    inline void propagate() {
        // ======================== ========================
        // Reverse moving boundary index
        // 反向移动边界下标
        // ======================== ========================
        if (left_head == traveling_wave_max_index)
            left_head = 0;
        else
            left_head++;
        if (right_head == 0)
            right_head = traveling_wave_max_index;
        else
            right_head--;

        // ======================== ========================
        // Boundary reflection
        // 边界反射
        // ======================== ========================
        // 左侧刚性反射
        right[get_i(0, right_head)] = -left[get_i(0, left_head)];
        // 右侧桥端反射
        bridge_->coupling_process(&right[get_i(traveling_wave_max_index, right_head)].v_x,
                                  &right[get_i(traveling_wave_max_index, right_head)].v_y);
        left[get_i(traveling_wave_max_index, left_head)] =
            right[get_i(traveling_wave_max_index, right_head)]; // 去掉负号，似乎bridge输出的已经是带符号的


        // ======================== ========================
        // Update boundary point
        // 更新边界指针
        // ======================== ========================
        left_boundary_point = &right[get_i(0, right_head)];
        right_boundary_point = &left[get_i(traveling_wave_max_index,
                                           left_head)];
    }

    inline int get_i(int real_index, int head) {
        return real_index + head <= traveling_wave_max_index
        ? real_index + head
        : real_index + head - delay_int;
    }

    inline void string_movement(double hammer_force) {

        const bool excited = hammer_force > 0.0;

        if (excited) {
            inject(hammer_force * c_z); // inject(hammer_force / (2 * z_));
        }

        propagate();
        filter();

        check_active();

        if (excited) {
            is_active = true;
        }
    }

    inline void inject(double inject_v) {
        left[get_i(strike_port.index_a, left_head)].v_y +=
        inject_v * strike_port.weight_a;
        left[get_i(strike_port.index_b, left_head)].v_y +=
        inject_v * strike_port.weight_b;
        right[get_i(strike_port.index_a, right_head)].v_y +=
        inject_v * strike_port.weight_a;
        right[get_i(strike_port.index_b, right_head)].v_y +=
        inject_v * strike_port.weight_b;
    }

//    inline double get_bridge_force(float vb) {
//        // vb = [2 * Zs / (Zs + Zb(ω))] * vi
//        // vb = vi + vo
//        return z_s_ * (vb - 2 * right_boundary_point->v_y);
//    }
    
    
    inline double get_string_vs() {
        return strike_port.weight_a * left[get_i(strike_port.index_a, left_head)].v_y +
        strike_port.weight_b * left[get_i(strike_port.index_b, left_head)].v_y +
        strike_port.weight_a * right[get_i(strike_port.index_a, right_head)].v_y +
        strike_port.weight_b * right[get_i(strike_port.index_b, right_head)].v_y;
    }

    inline void system_reset() {
        left_head = 0;
        right_head = 0;

        is_active = false;
        damper_active = false;

        inactive_probe_count_ = 0;
        activity_probe_counter_ = 0;

        std::fill(left.begin(), left.end(), Waveguide2D{0.0f, 0.0f});
        std::fill(right.begin(), right.end(), Waveguide2D{0.0f, 0.0f});

        left_boundary_point = &left[left_head];
        right_boundary_point = &right[get_i(traveling_wave_max_index,
                                            right_head)];

        fractional_filter_x.system_reset();
        fractional_filter_y.system_reset();
        damper_x.system_reset();
        damper_y.system_reset();
        dispersion_filter_x.system_reset();
        dispersion_filter_y.system_reset();
    }

private:
    inline void filter() {
        fractional_filter_x.process(left_boundary_point->v_x);
        fractional_filter_y.process(left_boundary_point->v_y);
        dispersion_filter_x.process(left_boundary_point->v_x);
        dispersion_filter_y.process(left_boundary_point->v_y);
        if (damper_active) {
            damper_x.process(left_boundary_point->v_x);
            damper_y.process(left_boundary_point->v_y);
        }
    }

    int inactive_probe_count_ = 0;
    int activity_probe_counter_ = 0;
    inline void check_active() {
        const int probe_interval = midi_n_ >= 88 ? 128 : 64;
        constexpr int k_inactive_probe_count = 8;

        if (++activity_probe_counter_ < probe_interval) {
            return;
        }

        activity_probe_counter_ = 0;

        constexpr float kVelocityThreshold = 1.0e-5f;
        constexpr float kEnergyThreshold = kVelocityThreshold * kVelocityThreshold;

        if (activity_probe() < kEnergyThreshold) {
            if (++inactive_probe_count_ >= k_inactive_probe_count) {
                is_active = false;
                system_reset();
            }
        } else {
            inactive_probe_count_ = 0;
            is_active = true;
        }
    }

    inline float activity_probe() {
        float rail_energy = 0.0f;
        for (int index = 0; index < delay_int; ++index) {
            rail_energy += left[static_cast<std::size_t>(index)].v_x *
                            left[static_cast<std::size_t>(index)].v_x +
                            right[static_cast<std::size_t>(index)].v_x *
                            right[static_cast<std::size_t>(index)].v_x +
                            left[static_cast<std::size_t>(index)].v_y *
                            left[static_cast<std::size_t>(index)].v_y +
                            right[static_cast<std::size_t>(index)].v_y *
                            right[static_cast<std::size_t>(index)].v_y;
        }
        rail_energy /= static_cast<float>(2 * delay_int);

        float state_energy_x = std::max(fractional_filter_x.state_energy(),
                                      static_cast<float>(dispersion_filter_x.state_energy()));
        float state_energy_y = std::max(fractional_filter_y.state_energy(),
                                      static_cast<float>(dispersion_filter_y.state_energy()));
        float state_energy = std::max(state_energy_x, state_energy_y);
      
        if (damper_active) {
            state_energy = std::max(state_energy, damper_x.state_energy());
            state_energy = std::max(state_energy, damper_y.state_energy());
        }

        return std::max(rail_energy, state_energy);
    }
};

#endif /* string_model_hpp */
