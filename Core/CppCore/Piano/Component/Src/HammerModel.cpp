//
//  HammerModel.cpp
//  bBpiano
//
//  Created by opus arc on 2026/4/6.
//
//  This document is not AI-assisted.
//


#include "HammerModel.hpp"
#include "../KeyModel.hpp"
#include "../../Utils/MyCSVReader.hpp"

#include <algorithm>
#include <cmath>

// ------------------------------------------------------------------------------------------
// MARK: 初始化

HammerModel::HammerModel(KeyModel *_pairedKey, int _midi_n) :
    pairedKey(_pairedKey),
    midi_n(_midi_n),
    string_count(midi_n <= 52 ? 2 : 3)
{
    const auto hammerParameter = MyCSVReader::getRT425HammerParameterByMidi(midi_n);
    K = hammerParameter.K;
    P = hammerParameter.P;
    m = hammerParameter.mass_kg;
    RH = hammerParameter.R;
    
    const auto stringParameter = MyCSVReader::getRT425WrappedStringParameterByMidi(midi_n);
    strikePoint = stringParameter.strike_ratio;
    
    pairedString_a = new StringModel(this, midi_n, 1);
    pairedString_b = new StringModel(this, midi_n, 2);
    pairedString_c = new StringModel(this, midi_n, 3);
    
    if (pairedString_a) {
        hammerTs = pairedString_a->Ts * 0.5;
        
        // strikeM 只在初始化时从比例位置量化到格点。
        // 实时音频循环里不再做 strikePoint * delay_index。
        const int maxM = std::max(1, pairedString_a->delay_index - 2);
        strikeM = std::clamp(
            static_cast<int>(std::floor(strikePoint * pairedString_a->delay_index)),
            1,
            maxM
        );
    }
}

// ------------------------------------------------------------------------------------------
// MARK: 整合 Sample
float HammerModel::getSample(){
    
    if(string_count == 2) {
        return pairedString_a->velocityAt(0.7) + pairedString_b->velocityAt(0.7);
    } else {
        return pairedString_a->velocityAt(0.7) + pairedString_b->velocityAt(0.7) + pairedString_c->velocityAt(0.7);
    }
    
    return pairedString_a->velocityAt(0.7);
}

// ------------------------------------------------------------------------------------------
// MARK: 运动帧

void HammerModel::hammerMovement() {
    if (!pairedString_a) return;
    
    // Hammer-P runtime：
    // 1. 用同一个击弦格点 strikeM 读取当前速度与半采样速度。
    // 2. hammer 以 2 倍采样率做两个子步。
    // 3. 两个子步力平均后，以原弦采样率注入完整 normal 弦。
    
    float v0_a = 0.0f;
    float vHalf_a = 0.0f;
    pairedString_a->readHammerVelocityPair(strikeM, v0_a, vHalf_a);
    
    double string_v0 = v0_a;
    double string_vHalf = vHalf_a;
    
    if(string_count >= 2 && pairedString_b) {
        float v0_b = 0.0f;
        float vHalf_b = 0.0f;
        pairedString_b->readHammerVelocityPair(strikeM, v0_b, vHalf_b);
        string_v0 += v0_b;
        string_vHalf += vHalf_b;
    }
    
    if(string_count >= 3 && pairedString_c) {
        float v0_c = 0.0f;
        float vHalf_c = 0.0f;
        pairedString_c->readHammerVelocityPair(strikeM, v0_c, vHalf_c);
        string_v0 += v0_c;
        string_vHalf += vHalf_c;
    }
    
    const double F1 = hammerPHalfStepForce(string_v0, hammerTs);
    const double F2 = hammerPHalfStepForce(string_vHalf, hammerTs);
    
    F = 0.5 * (F1 + F2);
    
    distributeHammerForce(strikeM, F);
    moveStrings();
}

double HammerModel::hammerPHalfStepForce(double _string_v, double _dt) {
    
    const double Z0 = pairedString_a->Z;
    
    // Hammer-P 的延迟力反馈：上一 hammer 子步的力通过 2Z0 转成速度修正。
    const double delayedForceVelocity = F_Last / (2.0 * Z0);
    
    // 锤毡压缩速度 = 锤速度 - 弦反馈速度 - 延迟力反馈速度。
    dv = (v_in - _string_v) - delayedForceVelocity;
    
    // 更新锤毡压缩量。
    dy += dv * _dt;
    
    // 锤子离开弦时，压缩量不能为负。
    if (dy < 0.0) {
        dy = 0.0;
    }
    
    // 非线性锤毡力：F = K * dy^P。
    double F_new = 0.0;
    if (dy > 0.0) {
        F_new = K * std::pow(dy, P);
    }
    
    // 接触力反作用到锤子，降低锤子速度。
    v_in -= (F_new / m) * _dt;
    
    // 保留 hammer-rate 的最后一个子步力。
    // 下一次 half step 会把它作为 Bank 结构里的延迟反馈。
    F_Last = F_new;
    
    return F_new;
}

void HammerModel::distributeHammerForce(int M, double _F) {
    
    // _F 是 hammer 看到的总接触力。
    // 多弦时先读多弦总反馈速度，再把同一个总力平均注入到每根弦。
    if(string_count == 2) {
        const float forcePerString = static_cast<float>(_F * 0.5);
        pairedString_a->injectForce(M, forcePerString);
        pairedString_b->injectForce(M, forcePerString);
    } else if(string_count == 3) {
        const float forcePerString = static_cast<float>(_F / 3.0);
        pairedString_a->injectForce(M, forcePerString);
        pairedString_b->injectForce(M, forcePerString);
        pairedString_c->injectForce(M, forcePerString);
    }
}

void HammerModel::moveStrings() {
    
    if(string_count == 2) {
        pairedString_a->stringMovement();
        pairedString_b->stringMovement();
    } else if (string_count == 3) {
        pairedString_a->stringMovement();
        pairedString_b->stringMovement();
        pairedString_c->stringMovement();
    }
}

void HammerModel::setVIn(double _v_in){
    // 每次新的 note_on 都是一轮新的 hammer-string 接触。
    // 弦本身不清空，保留正在振动的能量；这里只重置锤子的接触状态。
    v_in = _v_in;
    dy = 0.0;
    dv = 0.0;
    F = 0.0;
    F_Last = 0.0;
}

void HammerModel::setInactive(){
    pairedKey->key_active = false;
}
