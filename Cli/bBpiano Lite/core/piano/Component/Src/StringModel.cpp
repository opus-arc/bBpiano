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
#include "../../PianoModel.hpp"


#include "../../Utils/MyCSVReader.hpp"
#include "../../ModelParameters/constants/RT425DispersionPresets.hpp"
#include "../../ModelParameters/constants/D274LossPresets.hpp"
#include "../../ModelParameters/constants/TunningPresets.hpp"




#include <array>
#include <fstream>
#include <sstream>
#include <algorithm>



StringModel::StringModel(HammerModel *_pairedHammer, int _midi_n, int _stringNum) :

    pairedHammer(_pairedHammer),
    midi_n(_midi_n),
    string_index(_stringNum)

{
    // 先加载所有常数
    
    // loss init
    lossPreset =
        Parameters::Tuning::D274LossPresets::getD274LossPreset(midi_n, string_index);
    
    // dispersion init
    dispersionPreset =
        Parameters::Tuning::RT425DispersionPresets::getRT425DispersionPreset(get_f0());

    // 步长
    Ts = 1.0 / static_cast<double>(sampleRate);
    
    const auto physicalParameter = MyCSVReader::getRT425WrappedStringParameterByMidi(midi_n);
    
    // 物理常数初始化
    T = physicalParameter.tension_n;
    rho = physicalParameter.linear_density_kg_m;
    physical_f0_hz = physicalParameter.f0_hz;
    physical_length_m = physicalParameter.length_m;
    physical_strike_ratio = physicalParameter.strike_ratio;
    // 计算波导长度
    double halfDispersionPureDelay = 0.5 * dispersionPreset.loopDelaySamples;
    const double lossHalfLoopPhaseDelay =
        lossHalfLoopPhaseDelayCompensation();

    // Dispersion preset 已经给出满足目标模态的纯 delay。
    // Loss 每个完整 round trip 经过一次，所以这里只扣除一半的
    // 基频 phase delay；不要用 group delay，也不要整段从半圈扣除。
    delay = halfDispersionPureDelay - lossHalfLoopPhaseDelay;
//    std::cout<<"midi_n: "<<midi_n<<", halfIdealDelay: "<<halfIdealDelay<<", halfDispersionPureDelay: "<<halfDispersionPureDelay<<", dispersionDelay: "<<dispersionDelay<<
////    ", lossDelay: "<<lossDelay<<
//    ", delay: "<<delay<<"\n";

    
    
//    // 取不大于波导长度的最大整数作为数组长度
    delay_int = std::floor(delay - 0.5);
    if(delay_int <= 0) delay_int = 2;
    delay_index = delay_int - 1;
    delay_frac = delay - static_cast<double>(delay_int);

    
    fractional_a1 = double(1 - delay_frac) / double(1 + delay_frac);
    

    // 计算力和速度的比例常数
    Z = std::sqrt(T * rho);
    
    // 这里直接借鉴最大的 midi_n = 21 的 delay 623.743 来初始化
    // 暂时选择 1000 初始化 N_int 个 0.0f 的离散位置
    // 这样在调琴的过程中不会数组越界
    right.assign(1000, 0.0f);
    left.assign(1000, 0.0f);
    leftHead = 0;
    rightHead = 0;

    
    

    //    std::cout
    //    << "midi_n: " << midi_n
    //    << ", referenceF1: " << dispersionPreset.referenceF1
    //    << ", B: " << dispersionPreset.B
    //    << ", loopDelaySamples: " << dispersionPreset.loopDelaySamples
    //    << ", sectionCount: " << dispersionPreset.sectionCount
    //    << "\n";
//    std::cout << "midi_n: " << midi_n << ", stringCount: " << string_index << "\n";
//    std::cout << "midi_n: " << midi_n << ", delay: " << delay << "\n";
//    std::cout << "midi_n: " << midi_n << ", delay: " << lossPreset.lossDelaySamples << "\n";
//    std::cout << "midi_n: " << midi_n << ", f0: " << get_f0() << "\n";
//    std::cout << "f0: " << get_f0() << "\n";
//    std::cout << "dispersionDelay: " << sampleRate / (2 * get_f0()) - 0.5 * dispersionPreset.loopDelaySamples << "\n";
//    std::cout << "_delay: " << sampleRate / (2 * get_f0()) - (sampleRate / (2 * get_f0()) - 0.5 * dispersionPreset.loopDelaySamples) << "\n";
    

}

// --------------------------------------------
// MARK: 实时值函数

// 根据 midi_n, reference_tone, temperament 计算 f0
// TODO: 这里能使用查表的方式降低加计算成本
float StringModel::get_f0() const {
    return pairedHammer->pairedKey->piano->modelParameters->tunningPresets->getFrequency(midi_n, TunningPresets::Temperament::equal, static_cast<TunningPresets::StringIndex>(string_index));
}


// --------------------------------------------
// MARK: 计算函数

void StringModel::stringMovement() {
    
    propagate();
    
    activityCounter++;
        if(activityCounter >= 10000) {
            // 此处仍然不可低于 0.003
            if((activityProbe() < 0.001) && pairedHammer->pairedKey->key_active) {
                pairedHammer->setInactive();
            }
            activityCounter = 0;
        }

}

void StringModel::injectForce(int relative_i, float F) const {
    
    // 边界条件：要访问 right[M + 1]，所以 M 不能太靠右
    const int maxM = std::max(1, delay_index - 2);
    const int M = std::clamp(relative_i, 1, maxM);
    const int MPlus = std::min(M + 1, delay_index);
    
    // Hammer-P 错位注入：left[M] 和 right[M + 1]
    const int absolute_i_l = rToAIndex_l(M);
    const int absolute_i_r = rToAIndex_r(MPlus);
    
    const float delta = F / (2.0f * static_cast<float>(Z));
    
    left[absolute_i_l] += delta;
    right[absolute_i_r] += delta;
}

void StringModel::propagate() {
    
    // 先读边界
    float r_r_boundary_value = right[rToAIndex_r(delay_index)];
    float l_l_boundary_value = left[rToAIndex_l(0)];
    
    BoundaryFilter(l_l_boundary_value, true);
    BoundaryFilter(r_r_boundary_value, false);
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


void StringModel::readHammerVelocityPair(int relative_i, float& v0, float& vHalf) const {
    // M 是 hammer 的相对击弦格点。为了半采样读速度，需要访问 M - 1 和 M + 1。
    const int maxM = std::max(1, delay_index - 1);
    const int M = std::clamp(relative_i, 1, maxM);
    const int MMinus = std::max(M - 1, 0);
    const int MPlus = std::min(M + 1, delay_index);
    
    const float right_M = right[rToAIndex_r(M)];
    const float left_M = left[rToAIndex_l(M)];
    
    // v(nTs) = y+(n, M) + y-(n, M)
    v0 = right_M + left_M;
    
    // v(nTs + Ts/2) 使用半采样速度：
    // 右行波取当前 M 与半步后会到达 M 的 M - 1 平均；
    // 左行波取当前 M 与半步后会到达 M 的 M + 1 平均。
    vHalf = 0.5f * (right_M + right[rToAIndex_r(MMinus)])
          + 0.5f * (left_M + left[rToAIndex_l(MPlus)]);
}



float StringModel::BoundaryFilter_virtual(float boundary_value, bool isLeft) {
    if (isLeft) {
        float fx1 = fractional_x1_l;
        float fy1 = fractional_y1_l;
//        auto lx1 = loss_x1_l;
//        auto lx2 = loss_x2_l;
//        auto ly1 = loss_y1_l;
//        auto ly2 = loss_y2_l;

        fractionalFilter(boundary_value, fx1, fy1);
//        dispersionFilter(boundary_value, dx1, dx2, dy1, dy2);
//        lossFilter(boundary_value, lx1, lx2, ly1, ly2);

        return -boundary_value;
    } else {
        float fx1 = fractional_x1_r;
        float fy1 = fractional_y1_r;
        auto lx1 = loss_x1_r;
        auto lx2 = loss_x2_r;
        auto ly1 = loss_y1_r;
        auto ly2 = loss_y2_r;
        std::array<float, Parameters::Tuning::RT425DispersionPresets::kRT425DispersionSectionCount> dx1 = dispersion_x1_r;
        std::array<float, Parameters::Tuning::RT425DispersionPresets::kRT425DispersionSectionCount> dx2 = dispersion_x2_r;
        std::array<float, Parameters::Tuning::RT425DispersionPresets::kRT425DispersionSectionCount> dy1 = dispersion_y1_r;
        std::array<float, Parameters::Tuning::RT425DispersionPresets::kRT425DispersionSectionCount> dy2 = dispersion_y2_r;
        
        fractionalFilter(boundary_value, fx1, fy1);
        dispersionFilter(boundary_value, dx1, dx2, dy1, dy2);
        lossFilter(boundary_value, lx1, lx2, ly1, ly2);

        return -boundary_value;
    }
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

    for (float y : loss_y1_l) p = std::max(p, std::abs(y));
    for (float y : loss_y1_r) p = std::max(p, std::abs(y));
    for (float y : loss_y2_l) p = std::max(p, std::abs(y));
    for (float y : loss_y2_r) p = std::max(p, std::abs(y));
    p = std::max(p, std::abs(fractional_y1_l));
    p = std::max(p, std::abs(fractional_y1_r));
//    p = std::max(p, std::abs(dispersion_y1_l));
//    p = std::max(p, std::abs(dispersion_y1_r));

    return p;
}
