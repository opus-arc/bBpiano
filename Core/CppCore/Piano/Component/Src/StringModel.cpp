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

#include "../../../../../AcousticLab/StringFilterLab/Utils/MyCSVReader.hpp"

#include <array>
#include <fstream>
#include <sstream>
#include <algorithm>

StringModel::StringModel(HammerModel *_pairedHammer, int _midi_n, int _stringNum) :

    pairedHammer(_pairedHammer),
    midi_n(_midi_n),
    string_index(_stringNum)

{
    
    Ts = 1.0 / static_cast<double>(sampleRate);
    

//    // 计算波导长度
    delay = double(sampleRate) / double(2 * get_f0());
//
    
//    // 取不大于波导长度的最大整数作为数组长度
    delay_int = std::floor(delay - 0.5);
    if(delay_int <= 0) delay_int = 2;
    delay_index = delay_int - 1;
    delay_frac = delay - static_cast<double>(delay_int);
    fractional_a1 = double(1 - delay_frac) / double(1 + delay_frac);
    

    // 计算力和速度的比例常数
    Z = std::sqrt(T * rho);

    // 初始化 N_int 个 0.0f 的离散位置
    right.assign(delay_int, 0.0f);
    left.assign(delay_int, 0.0f);
    leftHead = 0;
    rightHead = 0;
    
    // loss init
    lossConstants = MyCSVReader::getLossConstant();
    for (const auto& c : lossConstants) {
        if (c.midi_n == midi_n) {
            loss_g = c.g;
            loss_a1 = c.a_1;
            break;
        }
    }
    
    // dispersion init
    auto dispersion = MyCSVReader::getDispersionConstantByMidi(midi_n);
    dispersion_a1 = static_cast<float>(dispersion.a1);
    dispersion_order = static_cast<float>(dispersion.order);
    
    
    if(midi_n == 69) {
        std::cout << loss_g << ", " << loss_a1 << "\n";
    }
    
    if (midi_n == 69) {
        std::cout
            << "midi_n: " << midi_n
            << ", string_index: " << string_index
            << ", f0: " << get_f0()
//            << ", loopDelayTarget: " << loopDelayTarget
//            << ", halfDelayTarget: " << halfDelayTarget
            << ", Delay: " << delay
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

void StringModel::setMode(StringMode _mode) {
    mode = _mode;
}

void StringModel::stringMovement() {
    
    propagate();
    
    activityCounter++;
    if(activityCounter >= 10000) {
//        std::cout << activityProbe() << "\n";
//        std::cout << "pairedHammer->pairedKey->key_active: " << pairedHammer->pairedKey->key_active << "\n";
//        std::cout << "&pairedHammer->pairedKey->key_active: " << &pairedHammer->pairedKey->key_active << "\n";
//        if((activityProbe() < 0.01) && pairedHammer->pairedKey->key_active) {
//            pairedHammer->setInactive();
//            std::cout<<"!!!!!!!"<<"\n";
//        }
        activityCounter = 0;
    }
}

void StringModel::injectForce(double p, float F) const {
    // 边界条件
    p = std::clamp(p, 0.0, 1.0);

    int relative_i = std::floor(p * delay_index);
    
    injectForce(relative_i, F);
    
}

void StringModel::injectForce(int relative_i, float F) const {
    
    // 边界条件
    relative_i = std::clamp(relative_i, 1, delay_index - 1);
    int absolute_i_l = rToAIndex_l(relative_i);
    int absolute_i_r = rToAIndex_r(relative_i);

    // 增量计算公式
    // TODO: 这里应该还能凹
    float delta = F / (2.0f * static_cast<float>(Z));

    // 注入弦
    right[absolute_i_r] += delta;
    left[absolute_i_l] += delta;
}

void StringModel::injectHammerFStaggeredForce(int relative_i, float F) const {
    
    // 边界条件：要访问 right[M + 1]，所以 M 不能太靠右
    const int M = std::clamp(relative_i, 1, delay_index - 2);
    
    const int absolute_i_l = rToAIndex_l(M);
    const int absolute_i_r = rToAIndex_r(M + 1);
    
    const float delta = F / (2.0f * static_cast<float>(Z));
    
    // Bank 错位注入：left[M] 和 right[M + 1]
    left[absolute_i_l] += delta;
    right[absolute_i_r] += delta;
}

void StringModel::propagate() {
    
    // 先读边界
    float r_r_boundary_value = right[rToAIndex_r(delay_index)];
    float l_l_boundary_value = left[rToAIndex_l(0)];
    
    // 边界与滤波器
    // Normal 模式走原来的 fractional / dispersion / loss。
    // HammerFTest 模式跳过这些滤波器，得到整数格点、无损耗、无色散的理想 DWG。
    if(mode == StringMode::Normal){
        BoundaryFilter(l_l_boundary_value, true);
        BoundaryFilter(r_r_boundary_value, false);
    }
    // 边界传播
    rightHead = (rightHead - 1 + delay_int) % delay_int; // 右边界向左移动 则波向右传播
    leftHead  = (leftHead + 1) % delay_int; // 左边界向右移动 则波向左传播
    
    // 写入新边界
    right[rToAIndex_r(0)] = -l_l_boundary_value;
    left[rToAIndex_l(delay_index)] = -r_r_boundary_value;

}






float StringModel::velocityAt(double p) const {
    // 边界条件
    p = std::clamp(p, 0.0, 1.0);

    int relative_i = std::floor(p * delay_index);
    
    int absolute_i_l = (relative_i + delay_int + leftHead) % delay_int;
    int absolute_i_r = (relative_i + delay_int + rightHead) % delay_int;
    
    

    // 拾音点以数据为参考系位置不发生改变
    return left[absolute_i_l] + right[absolute_i_r];
}

float StringModel::velocityAtGrid(int relative_i) const{
    //注意是只读函数（专门给编程学到牛肚皮里去的fancy准备的注释）
    relative_i = std::clamp(relative_i, 0, delay_index);
    
    return left[rToAIndex_l(relative_i)] + right[rToAIndex_r(relative_i)];
}

float StringModel::nextVelocityAt(double p) {
    // 边界条件
    p = std::clamp(p, 0.0, 1.0);

    int relative_i = std::floor(p * delay_index);
    relative_i = std::clamp(relative_i, 0, delay_index);
    
    int relative_i_l_next = std::clamp(relative_i + 1, 0, delay_index); // 左波拾音点右侧的点下一帧就在拾音点上
    int relative_i_r_next = std::clamp(relative_i - 1, 0, delay_index); // 右波拾音点左侧的点下一帧就在拾音点上

    return left[rToAIndex_l(relative_i_l_next)]
         + right[rToAIndex_r(relative_i_r_next)];
    
}

float StringModel::velocityAtHalfSample(int relative_i) const{
    //边界条件
    relative_i = std::clamp(relative_i, 1, delay_index - 1);
    
    
    //y+(n+1, M) = y+(n, M - 1)
    float yPlus = 0.5f * (
            right[rToAIndex_r(relative_i)]
          + right[rToAIndex_r(relative_i-1)]
        );
    
    
    //y-(n+1, M) = y-(n, M + 1)
    float yMinus = 0.5f * (
            left[rToAIndex_l(relative_i)]
          + left[rToAIndex_l(relative_i + 1)]
        );
    
    return yPlus + yMinus;
}
    
float StringModel::activityProbe() const {
    int points[5] = {
        delay_int / 5,
        delay_int / 3,
        delay_int / 2,
        (delay_int * 2) / 3,
        (delay_int * 4) / 5
    };

    float p = 0.0f;

    for (int relative_idx : points) {
        relative_idx = std::clamp(relative_idx, 0, delay_index);
        
        int absolute_idx_l = rToAIndex_l(relative_idx);
        int absolute_idx_r = rToAIndex_r(relative_idx);
        

        float l = left[absolute_idx_l];
        float r = right[absolute_idx_r];

        p = std::max(p, std::abs(l + r)); // physical velocity proxy
        p = std::max(p, std::abs(l));     // travelling wave proxy
        p = std::max(p, std::abs(r));
    }

    p = std::max(p, std::abs(loss_y1_l));
    p = std::max(p, std::abs(loss_y1_r));
    p = std::max(p, std::abs(fractional_y1_l));
    p = std::max(p, std::abs(fractional_y1_r));
//    p = std::max(p, std::abs(dispersion_y1_l));
//    p = std::max(p, std::abs(dispersion_y1_r));

    return p;
}




float StringModel::BoundaryFilter_virtual(float boundary_value, bool isLeft) {
    if (isLeft) {
        float fx1 = fractional_x1_l;
        float fy1 = fractional_y1_l;
        float ly1 = loss_y1_l;
        std::array<float, 20> dx1 = dispersion_x1_l;
        std::array<float, 20> dy1 = dispersion_y1_l;

        fractionalFilter(boundary_value, fx1, fy1);
        dispersionFilter(boundary_value, dx1, dy1);
        lossFilter(boundary_value, ly1);

        return -boundary_value;
    } else {
        float fx1 = fractional_x1_r;
        float fy1 = fractional_y1_r;
        float ly1 = loss_y1_r;
        std::array<float, 20> dx1 = dispersion_x1_r;
        std::array<float, 20> dy1 = dispersion_y1_r;
        
        fractionalFilter(boundary_value, fx1, fy1);
        dispersionFilter(boundary_value, dx1, dy1);
        lossFilter(boundary_value, ly1);

        return -boundary_value;
    }
}
