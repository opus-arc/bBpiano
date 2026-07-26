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
#include "../../ModelParameters/constants/D274DispersionPresets.hpp"
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
        Parameters::Tuning::D274DispersionPresets::getRT425DispersionPreset(get_f0());

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


    // 取不大于波导长度的最大整数作为数组长度
    delay_int = std::floor(delay - 0.5);
    if(delay_int <= 0) delay_int = 2;
    delay_index = delay_int - 1;
    delay_frac = delay - static_cast<double>(delay_int);

    // pickup 可落在 0...delay_index 的任意分数位置。
    pickupPort = makeSpatialPort(0.7 * delay_index, delay_index);

    bridgePort = makeSpatialPort(static_cast<double>(delay_index), delay_index);

    // 一个 Hammer-P junction j 对应 left[j] / right[j + 1]。
    // 因而有效 junction 范围是 0...delay_index-1。
    const int maxStrikeJunction = std::max(0, delay_index - 1);
    strikePort = makeSpatialPort(
        physical_strike_ratio * delay_index,
        maxStrikeJunction
    );

    fractional_a1 = double(1 - delay_frac) / double(1 + delay_frac);

    dispersionSectionCount = std::min<std::size_t>(
        static_cast<std::size_t>(dispersionPreset.sectionCount),
        dispersionPreset.sections.size()
    );
    for (std::size_t i = 0; i < dispersionSectionCount; ++i) {
        const auto& source = dispersionPreset.sections[i];
        dispersionCoefficients[i] = {
            static_cast<float>(source.b0),
            static_cast<float>(source.b1),
            static_cast<float>(source.b2),
            static_cast<float>(source.a1),
            static_cast<float>(source.a2)
        };
    }
    

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
//              << ", pickupPort: " << pickupPort.index0 << "+"
//              << pickupPort.weight1 << "\n";
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
            // 此处不再起到降低计算量的作用，而是削除噪声
            const KeyModel* key = pairedHammer->pairedKey;
            const bool damperLifted =
                key->key_down || key->piano->test_sustainPedal_active;

            if(!damperLifted && (activityProbe() < 0.001) && key->key_active) {
                pairedHammer->setInactive();
            }
            activityCounter = 0;
        }

}

StringModel::SpatialPort StringModel::makeSpatialPort(
    double position,
    int maxIndex
) {
    maxIndex = std::max(0, maxIndex);
    position = std::clamp(position, 0.0, static_cast<double>(maxIndex));

    const int index0 = static_cast<int>(std::floor(position));
    const int index1 = std::min(index0 + 1, maxIndex);
    const float weight1 = index1 == index0
        ? 0.0f
        : static_cast<float>(position - index0);

    return SpatialPort{
        index0,
        index1,
        1.0f - weight1,
        weight1
    };
}

void StringModel::injectForceAtJunction(int junctionIndex, float F) const {
    // Hammer-P 错位 junction：left[j] 和 right[j + 1]。
    const int absolute_i_l = rToAIndex_l(junctionIndex);
    const int absolute_i_r = rToAIndex_r(junctionIndex + 1);
    const float delta = F / (2.0f * static_cast<float>(Z));

    left[absolute_i_l] += delta;
    right[absolute_i_r] += delta;
}

void StringModel::injectForce(float F) const {
    injectForceAtJunction(strikePort.index0, F * strikePort.weight0);

    if (strikePort.index1 != strikePort.index0) {
        injectForceAtJunction(strikePort.index1, F * strikePort.weight1);
    }
}


void StringModel::propagate() {
    
    // 先读边界
    float r_r_boundary_value = right[rToAIndex_r(delay_index)];
    float l_l_boundary_value = left[rToAIndex_l(0)];
    
    BoundaryFilter(l_l_boundary_value, true);
    BoundaryFilter(r_r_boundary_value, false);

    // 边界传播
    if (rightHead == 0) {
        rightHead = delay_int - 1;
    } else {
        --rightHead;
    }

    ++leftHead;
    if (leftHead == delay_int) {
        leftHead = 0;
    }
    
    // 写入新边界
    right[rToAIndex_r(0)] = -l_l_boundary_value;
    left[rToAIndex_l(delay_index)] = bridgeVelocity - r_r_boundary_value;

}

bool StringModel::isActive() const {
    return pairedHammer
        && pairedHammer->pairedKey
        && pairedHammer->pairedKey->key_active;
}

float StringModel::velocityAtPort(const SpatialPort& port) const {
    const float velocity0 =
        left[rToAIndex_l(port.index0)]
        + right[rToAIndex_r(port.index0)];

    if (port.index1 == port.index0) {
        return velocity0;
    }

    const float velocity1 =
        left[rToAIndex_l(port.index1)]
        + right[rToAIndex_r(port.index1)];

    return port.weight0 * velocity0 + port.weight1 * velocity1;
}

//float StringModel::getSamples() const {
//    // 右行波到达右端/桥端的入射速度波 v+
//    float vPlus = right[rToAIndex_r(delay_index)];
//    // 预览右端会经过的 fractional / dispersion / loss / damper
//    // 注意 BoundaryFilter_virtual 返回的是 -processed，所以这里取负号还原。
//    float reflectedPreview =
//        const_cast<StringModel*>(this)->BoundaryFilter_virtual(vPlus, false);
//    float processedVPlus = -reflectedPreview;
//    // 暂时假设桥速度 vb = 0
//    // F_bridge = 2 * Z * v+
//    return static_cast<float>(2.0 * Z * processedVPlus);
//}


float StringModel::BoundaryFilter_virtual(float boundary_value, bool isLeft) {

    if (isLeft) {
        float fx1 = fractional_x1_l;
        float fy1 = fractional_y1_l;

        fractionalFilter(boundary_value, fx1, fy1);

        boundary_value = -boundary_value;

        return boundary_value;
    } else {
        float fx1 = fractional_x1_r;
        float fy1 = fractional_y1_r;
        auto lx1 = loss_x1_r;
        auto lx2 = loss_x2_r;
        auto ly1 = loss_y1_r;
        auto ly2 = loss_y2_r;
        std::array<float, Parameters::Tuning::D274DispersionPresets::kRT425DispersionSectionCount> dx1 = dispersion_x1_r;
        std::array<float, Parameters::Tuning::D274DispersionPresets::kRT425DispersionSectionCount> dx2 = dispersion_x2_r;
        std::array<float, Parameters::Tuning::D274DispersionPresets::kRT425DispersionSectionCount> dy1 = dispersion_y1_r;
        std::array<float, Parameters::Tuning::D274DispersionPresets::kRT425DispersionSectionCount> dy2 = dispersion_y2_r;
        
        fractionalFilter(boundary_value, fx1, fy1);
        dispersionFilter(boundary_value, dx1, dx2, dy1, dy2);
        lossFilter(boundary_value, lx1, lx2, ly1, ly2);
        
        double dz1 = damper_z1;
        double dz2 = damper_z2;
        boundary_value = static_cast<float>(
            testDamper(boundary_value, dz1, dz2)
        );

        boundary_value = bridgeVelocity - boundary_value;

        return boundary_value;
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

double StringModel::testDamper(double x, double &z1, double &z2) {
    const KeyModel* key = pairedHammer->pairedKey;
    const bool damperLiftedByKey = key->key_down;
    const bool damperLiftedByPedal =
        key->piano->test_sustainPedal_active;

    // 真实钢琴中，按住某键会单独抬起该键制音器；延音踏板则抬起
    // 全部制音器。只有键已松开且踏板未踩下时，制音器才接触弦。
    if(damperLiftedByKey || damperLiftedByPedal) {
        z1 = 0.0;
        z2 = 0.0;
        return x;
    }

    // ==========================
    // Temporary Damper Controls
    // ==========================
    constexpr double lowLoss   = 0.020; // 低频耗散：0.020~0.070
    constexpr double highLoss  = 0.25;  // 高频抓取：0.25~0.70
    constexpr double damperMix = 0.38;  // 毛毡低通占比：0.30~0.70

    const double loopGain = 1.0 - lowLoss;
    const double wet = damperMix * highLoss;
    const double dry = 1.0 - wet;

    // Gentle second-order low-pass damper color.
    constexpr double b0 = 0.292893218813;
    constexpr double b1 = 0.585786437627;
    constexpr double b2 = 0.292893218813;

    constexpr double a1 = 0.000000000000;
    constexpr double a2 = 0.171572875254;

    const double y = b0 * x + z1;

    z1 = b1 * x - a1 * y + z2;
    z2 = b2 * x - a2 * y;

    return loopGain * (dry * x + wet * y);
}

void StringModel::readIntegerHammerVelocityPair(
    int relative_i,
    float& v0,
    float& vHalf
) const {
    // M 是 hammer 的相对击弦格点。为了半采样读速度，需要访问 M - 1 和 M + 1。
    const int M = std::clamp(relative_i, 0, delay_index);
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

void StringModel::readHammerVelocityPair(float& v0, float& vHalf) const {
    float v0AtIndex0 = 0.0f;
    float vHalfAtIndex0 = 0.0f;
    readIntegerHammerVelocityPair(
        strikePort.index0,
        v0AtIndex0,
        vHalfAtIndex0
    );

    if (strikePort.index1 == strikePort.index0) {
        v0 = v0AtIndex0;
        vHalf = vHalfAtIndex0;
        return;
    }

    float v0AtIndex1 = 0.0f;
    float vHalfAtIndex1 = 0.0f;
    readIntegerHammerVelocityPair(
        strikePort.index1,
        v0AtIndex1,
        vHalfAtIndex1
    );

    v0 = strikePort.weight0 * v0AtIndex0
       + strikePort.weight1 * v0AtIndex1;
    vHalf = strikePort.weight0 * vHalfAtIndex0
          + strikePort.weight1 * vHalfAtIndex1;
}
