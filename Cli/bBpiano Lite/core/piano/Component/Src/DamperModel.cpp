//
//  DamperModel.cpp
//  bBpiano
//
//  Created by opus arc on 2026/4/7.
//
//  This document is not AI-assisted.
//

#include "DamperModel.hpp"

DamperModel::DamperModel(double sampleRateHz) {
    prepare(sampleRateHz);
}

void DamperModel::prepare(double sampleRateHz) {
    sampleRate = sampleRateHz;
    reset();
    updateCoefficients();
}

void DamperModel::reset() {
    state = 0.0;
}

void DamperModel::setPressure(double value01) {
    pressure = clamp01(value01);
    updateCoefficients();
}

void DamperModel::setFeltHardness(double value01) {
    feltHardness = clamp01(value01);
    updateCoefficients();
}

void DamperModel::setFromSustainPedal(double sustainPedal01) {
    setPressure(1.0 - clamp01(sustainPedal01));
}

double DamperModel::process(double inputSample) {
    state = coefficientB * inputSample + coefficientA * state;
    return state;
}

void DamperModel::updateCoefficients() {
    // Temporary placeholder implementation.
    // More pressure -> lower cutoff -> stronger damping.
    const double alpha = 0.98 - 0.93 * pressure;
    coefficientA = std::clamp(alpha, 0.02, 0.999);
    coefficientB = 1.0 - coefficientA;
}

double DamperModel::clamp01(double value) {
    return std::clamp(value, 0.0, 1.0);
}
