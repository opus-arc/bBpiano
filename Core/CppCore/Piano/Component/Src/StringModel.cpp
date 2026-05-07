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
    Delay = double(sampleRate) / double(2 * get_f0());
//
    
//    // 取不大于波导长度的最大整数作为数组长度
    Delay_Int = std::floor(Delay);
    if(Delay_Int <= 0) Delay_Int = 2;
    Delay_Index = Delay_Int - 1;
    Delay_Frac = Delay - static_cast<double>(Delay_Int);
    

    // 计算力和速度的比例常数
    Z = std::sqrt(T * rho);

    // 初始化 N_int 个 0.0f 的离散位置
    right.assign(Delay_Int, 0.0f);
    left.assign(Delay_Int, 0.0f);
    rightNext.assign(Delay_Int, 0.0f);
    leftNext.assign(Delay_Int, 0.0f);
    
    if (midi_n == 69) {
        std::cout
            << "midi_n: " << midi_n
            << ", string_index: " << string_index
            << ", f0: " << get_f0()
//            << ", loopDelayTarget: " << loopDelayTarget
//            << ", halfDelayTarget: " << halfDelayTarget
            << ", Delay: " << Delay
//            << ", halfDelayFractional: " << halfDelayFractional
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
//    if (std::abs(F) > 0.0f) active = true;
    
    // 边界条件
    p = std::clamp(p, 0.0, 1.0);

    int m = std::floor(p * Delay_Index);
    
    // 不允许端点
    m = std::clamp(m, 1, Delay_Index - 1);

    // 增量计算公式
    float delta = F / (2.0f * static_cast<float>(Z));

    // 注入弦
    right[m] += delta;
    left[m] += delta;
}

void StringModel::injectForce(int m, float F) const {
//    if (std::abs(F) > 0.0f) active = true;
    
    // 边界条件
    if (m > Delay_Index) m = Delay_Index - 1;
    if (m < 0) m = 1;

    // 增量计算公式
    float delta = F / (2.0f * static_cast<float>(Z));

    // 注入弦
    right[m] += delta;
    left[m] += delta;
}

void StringModel::propagate() const {

    // 内部传播
    for (int i = 1; i <= Delay_Index; ++i) {
        leftNext[i - 1] = left[i];
    }

    for (int i = 0; i <= Delay_Index - 1; ++i) {
        rightNext[i + 1] = right[i];
    }

    // 边界反射
    rightNext[0] = -g * left[0];
    leftNext[Delay_Index] = -g * right[Delay_Index];
    

    // 当 N == 50.1136 时, 到边界时才规定它少移动了 frac 帧 （一秒几万帧，十几、百把帧的误差应该听不出来的）
    // 所以就要去用插值估计在这一帧的前 0.1136 帧的值是多少
    // 使用线性插值
    // y[N - frac] = (1 - frac) x [N - 1] + frac x [N]
    
    

    std::swap(left, leftNext);
    std::swap(right, rightNext);
}

float StringModel::velocityAt(double p) const {
    // 边界条件
    p = std::clamp(p, 0.0, 1.0);

    int m = std::floor(p * Delay_Index);

    return left[m] + right[m];
}

float StringModel::nextVelocityAt(double p) const {
    // 边界条件
    p = std::clamp(p, 0.0, 1.0);

    int m = std::floor(p * Delay_Index);

    // 左边的右边那一帧下回就到拾音点
    int ml = m + 1;
    int mr = m - 1;

    if (ml > Delay_Index) ml = Delay_Index;
    if (mr < 0) mr = 0;

    return left[ml] + right[mr];
}
    


