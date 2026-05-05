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
    

    // 计算波导长度
    N = double(sampleRate) / double(2 * get_f0());

    // 取不大于波导长度的最大整数作为数组长度
    N_int = std::floor(N);
    if(N_int <= 0) N_int = 2;
    N_index = N_int - 1;

    // 计算力和速度的比例常数
    Z = std::sqrt(T * rho);

    // 初始化 N_int 个 0.0f 的离散位置
    right.assign(N_int, 0.0f);
    left.assign(N_int, 0.0f);
    rightNext.assign(N_int, 0.0f);
    leftNext.assign(N_int, 0.0f);
    
    if(midi_n == 69)
    std::cout << "midi_n: " << midi_n << ", string_index: " << string_index << ", f0: " << get_f0() << ", 波导长度: " << N << "\n";
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

    // 我这里只扫描第一根弦，我假设第一根弦能量不够了，那其他一到两根也差不多结束了
    // 再加上 128 帧扫描一次的剪枝
//        if (!active) return;
//        if (string_index == 1 && ++activityCounter >= 128) {
//            activityCounter = 0;
//            updateActivity();
//
//        }
    
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

    // 内部传播
    for (int i = 1; i <= N_index; ++i) {
        leftNext[i - 1] = left[i];
    }

    for (int i = 0; i <= N_index - 1; ++i) {
        rightNext[i + 1] = right[i];
    }

    // 边界反射
    rightNext[0] = -g * left[0];
    leftNext[N_index] = -g * right[N_index];

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



// TODO: 这里算能量还可以用包络近似，直接看该处的能量怎么样，简化计算
float StringModel::energy() const {
    double e = 0.0;
    for (int i = 0; i < N_int; ++i) {
        e += double(left[i]) * double(left[i]);
        e += double(right[i]) * double(right[i]);
    }
    return static_cast<float>(e);
}
bool StringModel::isActive() const {
    constexpr float energyThreshold = 1e-8f;
    return energy() > energyThreshold;
}
void StringModel::updateActivity() const {
    constexpr float energyThreshold = 1e-8f;

    double e = 0.0;
    for (int i = 0; i < N_int; ++i) {
        e += double(left[i]) * double(left[i]);
        e += double(right[i]) * double(right[i]);
    }

    active = (e > energyThreshold);
    
//    std::cout<<"midi_n: "<<midi_n<<", key_active: "<<active<<std::endl;
    
}
