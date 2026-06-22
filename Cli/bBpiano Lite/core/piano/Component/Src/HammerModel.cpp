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
    midi_n(_midi_n)
{
    const auto hammerParameter = MyCSVReader::getRT425HammerParameterByMidi(midi_n);
    K = hammerParameter.K;
    P = hammerParameter.P;
    m = hammerParameter.mass_kg;
    RH = hammerParameter.R;
    
    const auto stringParameter = MyCSVReader::getRT425WrappedStringParameterByMidi(midi_n);
    strikePoint = stringParameter.strike_ratio;
    
    if(midi_n <= 30) {
        pairedString_a = new StringModel(this, midi_n, 1);
        string_count = 1;
    } else if (midi_n <= 37) {
        pairedString_a = new StringModel(this, midi_n, 1);
        pairedString_b = new StringModel(this, midi_n, 2);
        string_count = 2;
    } else {
        pairedString_a = new StringModel(this, midi_n, 1);
        pairedString_b = new StringModel(this, midi_n, 2);
        pairedString_c = new StringModel(this, midi_n, 3);
        string_count = 3;
    }

    
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
    
    if(string_count == 1) {
        return pairedString_a->pickupVelocity();
    } else if(string_count == 2) {
        return pairedString_a->pickupVelocity() + pairedString_b->pickupVelocity();
    } else if(string_count == 3) {
        return pairedString_a->pickupVelocity() + pairedString_b->pickupVelocity() + pairedString_c->pickupVelocity();
    }
    
    return pairedString_a->pickupVelocity();
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
    
    if(string_count == 1) {
        const float forcePerString = static_cast<float>(_F);
        pairedString_a->injectForce(M, forcePerString);
    } else if(string_count == 2) {
        const float forcePerString = static_cast<float>(_F / 2.0);
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
    
    if(string_count == 1) {
        pairedString_a->stringMovement();
    } else if (string_count == 2) {
        pairedString_a->stringMovement();
        pairedString_b->stringMovement();
    } else if (string_count == 3) {
        pairedString_a->stringMovement();
        pairedString_b->stringMovement();
        pairedString_c->stringMovement();
    }
}

void HammerModel::setVIn(double _v_in){
    // _v_in 此处语义为 MIDI velocity 0~128，
    // HammerModel 内部转换为物理 hammer impact velocity，单位 m/s。
    v_in = midiVelocityToHammerVelocity(_v_in);

    dy = 0.0;
    dv = 0.0;
    F = 0.0;
    F_Last = 0.0;
}

void HammerModel::setInactive(){
    pairedKey->key_active = false;
}



double HammerModel::midiVelocityToHammerVelocity(double midiVelocity) const {
    // `_v_in` from PianoModel is currently MIDI velocity, not a physical speed.
    // This function maps MIDI velocity to hammer impact velocity in m/s through
    // the action travel-time model reported by Goebl et al. (2005).
    //
    // Empirical anchors used here:
    // - very soft tones: about 0.18 m/s
    // - typical expressive MIDI 40~60: about 0.7~1.25 m/s
    // - loud/fortissimo actions: about 6.8~7.8 m/s
    //
    // Goebl travel-time model:
    //     tt = a * HV^b
    // so:
    //     HV = pow(tt / a, 1 / b)
    //
    // This is an initial ActionModel approximation. HammerModel itself still
    // receives a physical hammer impact velocity after this conversion.
    const double midi = std::clamp(midiVelocity, 0.0, 127.0);
    if (midi <= 0.0) {
        return 0.0;
    }

    // MIDI velocity is treated as a linear control of target action travel time.
    // The endpoints follow the measured range: very soft attacks can take around
    // 200 ms, while very loud struck attacks approach about 20 ms.
    constexpr double slowTravelTimeMs = 200.0;
    constexpr double fastTravelTimeMs = 20.0;
    const double u = midi / 127.0;
    const double travelTimeMs = slowTravelTimeMs + (fastTravelTimeMs - slowTravelTimeMs) * u;

    // Average Bösendorfer SE290 / Goebl 2001 travel-time approximation,
    // verified in Goebl et al. 2005 as close to the pressed-touch data:
    //     tt = 89.16 * HV^-0.570
    constexpr double travelA = 89.16;
    constexpr double travelB = -0.570;

    double hammerVelocity = std::pow(travelTimeMs / travelA, 1.0 / travelB);

    // Keep the excitation inside the experimentally reported grand-piano range.
    // The upper bound is intentionally below 8.0 m/s to avoid excessive high-note
    // brittleness while still allowing fortissimo struck tones.
    hammerVelocity = std::clamp(hammerVelocity, 0.18, 7.5);

    return hammerVelocity;
}
