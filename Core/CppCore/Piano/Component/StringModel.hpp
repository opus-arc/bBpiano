//
//  StringModel.hpp
//  bBpiano
//
//  Created by opus arc on 2026/4/6.
//
//  This document is not AI-assisted.
//

#ifndef String_hpp
#define String_hpp

#include <iostream>
#include <algorithm>
#include <vector>
#include <cmath>
#include <array>
#include <complex>

#include "../Utils/RT425DispersionPresets.hpp"

class HammerModel;

struct LossConstant;

class StringModel {

    // --------------------------------------------
    // MARK: 常量
    
private:
    
    // 采样率
    private: static constexpr double sampleRate = 44100.0;
        
    mutable int activityCounter = 0;
    
    // --------------------------------------------
    // MARK: 组件与基本信息
    //  初始化后固定的内容
    
public:
    
    // 初始化
    // explicit 禁止隐式转换带来的语义污染
    explicit StringModel(HammerModel *_pairedHammer, int _midi_n, int _stringNum);
    
    // 配对的击锤
    HammerModel *pairedHammer = nullptr;
    
    // midi 号码
    const int midi_n;
    
    // 弦的编号
    const int string_index;
    
    // RT-425 wrapped string 基础参数。
    // rho 在当前代码里表示线密度 kg/m，不是论文表里的体密度 kg/m^3。
    double T = 0.0;
    double rho = 0.0;
    double physical_f0_hz = 0.0;
    double physical_length_m = 0.0;
    double physical_strike_ratio = 0.0;
    
    // --------------------------------------------
    // MARK: 实时值与其函数
    //  Derived Value（派生量） + Lazy Evaluation（惰性计算） + Cache（缓存）
    
public:
    
    // 依赖 midi_n, reference_tone, temperament 计算 f0
    // 只与 ModelParameters::shared->tuning->version 有关
    float get_f0() const;
    mutable float cache_f0;
    
    
    // --------------------------------------------
    // MARK: 状态值
    //  State 运行时会被反复修改的值
    
public:
    
    // 这根弦的振动能量达到一个 epcilon 以上
//    mutable bool active = false;
    
//    mutable std::vector<float> leftNext;
//    mutable std::vector<float> rightNext;
    
    // 时间步长
    double Ts = 0;
    
    // 波导长度
    double delay = 0.0;
    int delay_int = 0;
    int delay_index = 0;
    double delay_frac = 0.0;// 波导长度小数部分
    double fractional_a1 = 0.0;
    mutable float fractional_x1_r = 0.0; // 上一次传入 allpass 的值
    mutable float fractional_y1_r = 0.0;
    mutable float fractional_x1_l = 0.0; // 上一次传入 allpass 的值
    mutable float fractional_y1_l = 0.0;
    
    // loss filter
    std::vector<LossConstant> lossConstants;
    mutable float loss_x1_r = 0.0; // 上一次传入 allpass 的值
    mutable float loss_y1_r = 0.0;
    mutable float loss_x1_l = 0.0; // 上一次传入 allpass 的值
    mutable float loss_y1_l = 0.0;
    double loss_a1 = -0.01;
    double loss_g = 0.999293;

    // dispersion filter
    MACOS::Piano::Data::DispersionPreset dispersionPreset;
    mutable std::array<float, MACOS::Piano::Data::kRT425DispersionSectionCount> dispersion_x1_r = {}; // 上一次传入 allpass 的值
    mutable std::array<float, MACOS::Piano::Data::kRT425DispersionSectionCount> dispersion_x2_r = {};
    mutable std::array<float, MACOS::Piano::Data::kRT425DispersionSectionCount> dispersion_y1_r = {};
    mutable std::array<float, MACOS::Piano::Data::kRT425DispersionSectionCount> dispersion_y2_r = {};
    mutable std::array<float, MACOS::Piano::Data::kRT425DispersionSectionCount> dispersion_x1_l = {}; // 上一次传入 allpass 的值
    mutable std::array<float, MACOS::Piano::Data::kRT425DispersionSectionCount> dispersion_x2_l = {};
    mutable std::array<float, MACOS::Piano::Data::kRT425DispersionSectionCount> dispersion_y1_l = {};
    mutable std::array<float, MACOS::Piano::Data::kRT425DispersionSectionCount> dispersion_y2_l = {};
    
    
    double dispersion_a0 = 0.5;
    double dispersion_a1 = -1.0;
    int dispersion_order = 0;

    
    // 力 ↔ 速度 的比例常数
    double Z = 0.0;
    
    // 波的实时离散值
    mutable std::vector<float> left;
    mutable std::vector<float> right;
    int leftHead = 0;
    int rightHead = 0;
    
    
    // --------------------------------------------
    // MARK: 计算函数

public:
    
    // Hammer-P 错位注入，将力变成波
    void injectForce(int relative_i, float F) const;
    
    // 传播
    void propagate();
    
    // --------------------------------------------
    // MARK: 运动帧
    
public:
    
    
    // 弦的运动回合，每帧的调用接口
    void stringMovement() ;
    
    // 获取速度的方式
    float velocityAt(double p) const ;
    
    // Hammer-P 读速度接口：一次读出当前格点速度与半采样速度。
    void readHammerVelocityPair(int relative_i, float& v0, float& vHalf) const;
    
    float activityProbe() const;

    
    // --------------------------------------------
    // MARK: inline 小函数
    // 调用函数也是有成本的 能尽量 inline 就尽量这样做
    // 大函数就不用 inline 了
    
    // 边界滤波器
    float BoundaryFilter_virtual(float boundary_value, bool isLeft);

    
    // 0 <= i < Delay_Int !!!
    inline int rToAIndex_l(int i) const {
        int x = leftHead + i;
        if (x >= delay_int)
            x -= delay_int;
        return x;
    }

    inline int rToAIndex_r(int i) const {
        int x = rightHead + i;
        if (x >= delay_int)
            x -= delay_int;
        return x;
    }
    
    
    inline void fractionalFilter(float &x, float &x1, float &y1) const {
        // y = a1 * x + x1 - a1 * y1;
        float y = static_cast<float>(fractional_a1 * x)
            + static_cast<float>(x1)
            - static_cast<float>(fractional_a1 * y1);
        y1 = y;
        x1 = x;
        x = y;
    }

    inline void lossFilter(float &x, float &y1) const {
        // y = g * (1 + a1) * x - a1 * y1
        float y = static_cast<float>(loss_g * (1.0 + loss_a1)) * x
                - static_cast<float>(loss_a1) * y1;
        y1 = y;
        x = y;
    }

    inline void dispersionFilter(
        float& x,
        std::array<float, MACOS::Piano::Data::kRT425DispersionSectionCount>& x1,
        std::array<float, MACOS::Piano::Data::kRT425DispersionSectionCount>& x2,
        std::array<float, MACOS::Piano::Data::kRT425DispersionSectionCount>& y1,
        std::array<float, MACOS::Piano::Data::kRT425DispersionSectionCount>& y2
    ) const {
        const std::size_t sectionCount = std::min<std::size_t>(
            static_cast<std::size_t>(dispersionPreset.sectionCount),
            dispersionPreset.sections.size()
        );

        for (std::size_t i = 0; i < sectionCount; ++i) {
            const auto& c = dispersionPreset.sections[i];

            // H(z) = (b0 + b1 z^-1 + b2 z^-2)
            //      / (1  + a1 z^-1 + a2 z^-2)
            // Difference equation:
            // y[n] = b0*x[n] + b1*x[n-1] + b2*x[n-2]
            //      - a1*y[n-1] - a2*y[n-2]
            float y =
                  static_cast<float>(c.b0) * x
                + static_cast<float>(c.b1) * x1[i]
                + static_cast<float>(c.b2) * x2[i]
                - static_cast<float>(c.a1) * y1[i]
                - static_cast<float>(c.a2) * y2[i];

            x2[i] = x1[i];
            x1[i] = x;

            y2[i] = y1[i];
            y1[i] = y;

            x = y;
        }
    }
    
    inline void BoundaryFilter(float& boundary_value, bool isLeft) {
        if(isLeft) {
            fractionalFilter(boundary_value, fractional_x1_l, fractional_y1_l);
//            dispersionFilter(boundary_value, dispersion_x1_l, dispersion_x2_l, dispersion_y1_l, dispersion_y2_l);
//            lossFilter(boundary_value, loss_y1_l);
        } else {
            fractionalFilter(boundary_value, fractional_x1_r, fractional_y1_r);
            dispersionFilter(boundary_value, dispersion_x1_r, dispersion_x2_r, dispersion_y1_r, dispersion_y2_r);
            lossFilter(boundary_value, loss_y1_r);
        }
    }

};





#endif /* String_hpp */
