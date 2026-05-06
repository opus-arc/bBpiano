//
//  StringModel.cpp
//  bBpiano
//
//  Created by opus arc on 2026/4/6.
//
//  This document is not AI-assisted.
//

#include "StringModel.hpp"
#include "../HammerModel.hpp"
#include "../KeyModel.hpp"
#include "../../../ModelParameters/PrecomputedValue.hpp"

StringModel::StringModel(HammerModel *_pairedHammer, int _midi_n, int _stringNum) :

    pairedHammer(_pairedHammer),
    midi_n(_midi_n),
    string_index(_stringNum)

{
    
    Ts = 1 / sampleRate;
    

//    // 计算波导长度
//    N = double(sampleRate) / double(2 * get_f0());
//
    
//    // 取不大于波导长度的最大整数作为数组长度
//    N_int_length = std::floor(N);
//    if(N_int_length <= 0) N_int_length = 2;
//    N_index = N_int_length - 1;
//    N_frac = N - static_cast<double>(N_int_length);
    
    
    // 目标 round-trip delay
    const double loopDelayTarget = sampleRate / static_cast<double>(get_f0());

    // 目标 half delay
    halfDelayTarget = 0.5 * loopDelayTarget;

    // Bank 的思想是：整数 delay 取 floor(D - 0.5)，让 fractional delay 落在 [0.5, 1.5)
    // 这里对 half delay 使用同样思想。
    const int halfDelayInt = static_cast<int>(std::floor(halfDelayTarget - 0.5));

    N_int_length = std::max(2, halfDelayInt);
    N_index = N_int_length - 1;

    // 每个半程需要补偿的小数延迟
    halfDelayFractional = halfDelayTarget - static_cast<double>(N_int_length);

    // 防御：理论上应为 [0.5, 1.5)
    if (halfDelayFractional < 0.5) {
        N_int_length -= 1;
        if (N_int_length < 2) N_int_length = 2;
        halfDelayFractional = halfDelayTarget - static_cast<double>(N_int_length);
    }

    if (halfDelayFractional >= 1.5) {
        N_int_length += 1;
        halfDelayFractional = halfDelayTarget - static_cast<double>(N_int_length);
    }

    N = halfDelayTarget;
    N_frac = halfDelayFractional;
    N_index = N_int_length - 1;

    // 每个边界各补偿 halfDelayFractional 的相位延迟
    leftBoundaryFracDelay.setDelay(halfDelayFractional);
    rightBoundaryFracDelay.setDelay(halfDelayFractional);
    

    // 计算力和速度的比例常数
    Z = std::sqrt(T * rho);

    // 初始化 N_int 个 0.0f 的离散位置
    right.assign(N_int_length, 0.0f);
    left.assign(N_int_length, 0.0f);
    rightNext.assign(N_int_length, 0.0f);
    leftNext.assign(N_int_length, 0.0f);
    
    if (midi_n == 69) {
        std::cout
            << "midi_n: " << midi_n
            << ", string_index: " << string_index
            << ", f0: " << get_f0()
            << ", loopDelayTarget: " << loopDelayTarget
            << ", halfDelayTarget: " << halfDelayTarget
            << ", N_int_length: " << N_int_length
            << ", halfDelayFractional: " << halfDelayFractional
            << "\n";
    }

}

// --------------------------------------------
// MARK: 实时值函数

// 根据 midi_n, reference_tone, temperament 计算 f0
// TODO: 这里能使用查表的方式降低加计算成本
float StringModel::get_f0() const {
    return PrecomputedValue::get_f0(midi_n, string_index);
}


// --------------------------------------------
// MARK: 计算函数


void StringModel::stringMovement() const {
    
    propagate();
    
}

void StringModel::injectForce(double p, float F) const {
    if (std::abs(F) > 0.0f) active = true;
    
    // 边界条件
    p = std::clamp(p, 0.0, 1.0);

    int m = std::floor(p * N_index);
    
    // 不允许端点
    m = std::clamp(m, 1, N_index - 1);

    // 增量计算公式
    float delta = F / (2.0f * static_cast<float>(Z));

    // 注入弦
    right[m] += delta;
    left[m] += delta;
}

void StringModel::injectForce(int m, float F) const {
    if (std::abs(F) > 0.0f) active = true;
    
    // 边界条件
    if (m > N_index) m = N_index - 1;
    if (m < 0) m = 1;

    // 增量计算公式
    float delta = F / (2.0f * static_cast<float>(Z));

    // 注入弦
    right[m] += delta;
    left[m] += delta;
}

void StringModel::propagate() const {

    // 内部传播：left 向左移动
    for (int i = 1; i <= N_index; ++i) {
        leftNext[i - 1] = left[i];
    }

    // 内部传播：right 向右移动
    for (int i = 0; i <= N_index - 1; ++i) {
        rightNext[i + 1] = right[i];
    }

    // 边界反射 + 一阶 allpass fractional delay
    const float reflectedFromLeft  = static_cast<float>(-g) * left[0];
    const float reflectedFromRight = static_cast<float>(-g) * right[N_index];

    rightNext[0] = leftBoundaryFracDelay.process(reflectedFromLeft);
    leftNext[N_index] = rightBoundaryFracDelay.process(reflectedFromRight);

    std::swap(left, leftNext);
    std::swap(right, rightNext);
}

float StringModel::velocityAt(double p) const {
    // 边界条件
    p = std::clamp(p, 0.0, 1.0);

    int m = std::floor(p * N_index);

    return left[m] + right[m];
}

float StringModel::nextVelocityAt(double p) const {
    // 边界条件
    p = std::clamp(p, 0.0, 1.0);

    int m = std::floor(p * N_index);

    // 左边的右边那一帧下回就到拾音点
    int ml = m + 1;
    int mr = m - 1;

    if (ml > N_index) ml = N_index;
    if (mr < 0) mr = 0;

    return left[ml] + right[mr];
}
    


