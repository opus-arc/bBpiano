//
//  StringModel.cpp
//  bBpiano
//
//  Created by opus arc on 2026/4/6.
//

#include "StringModel.hpp"

StringModel::StringModel(double _f0) {
    
    Ts = 1 / sampleRate;
    
    if (_f0 <= 0.0) f0 = 1.0;
    else f0 = _f0;
    

    // 音域

    // 计算波导长度
    N = double(sampleRate) / double(2 * f0);

    // 取不大于波导长度的最大整数作为数组长度
    N_int = std::floor(N);
    if(N_int <= 0) N_int = 2;
    N_index = N_int - 1;

    // 计算 力 ↔ 速度 的比例常数
    Z = std::sqrt(T * rho);

    // 初始化 N_int 个 0.0f 的离散位置
    right.assign(N_int, 0.0f);
    left.assign(N_int, 0.0f);
}

void StringModel::stringMovement(){
    // 传播
    propagate();
}


void StringModel::injectForce(double p, float F) {
    // 边界条件
    if (p > 1)
        p = 1;
    if (p < 0)
        p = 0;

    int m = std::floor(p * N_index);

    // 增量计算公式
    float delta = F / (2.0f * static_cast<float>(Z));

    // 注入弦
    right[m] += delta;
    left[m] += delta;
}

void StringModel::propagate() {
    std::vector<float> newLeft(N_int);
    std::vector<float> newRight(N_int);

    // 内部传播
    for (int i = 1; i <= N_index; ++i) {
        newLeft[i - 1] = left[i];
    }

    for (int i = 0; i <= N_index - 1; ++i) {
        newRight[i + 1] = right[i];
    }

    // 边界反射
    newRight[0] = -g * left[0];
    newLeft[N_index] = -g * right[N_index];

    left = std::move(newLeft);
    right = std::move(newRight);
}

float StringModel::velocityAt(double p) {
    // 边界条件
    if (p > 1)
        p = 1;
    if (p < 0)
        p = 0;

    int m = std::floor(p * N_index);

    return left[m] + right[m];
}

