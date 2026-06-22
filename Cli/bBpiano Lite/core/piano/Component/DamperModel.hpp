//
//  DamperModel.hpp
//  bBpiano
//
//  Created by opus arc on 2026/4/7.
//
//  This document is not AI-assisted.
//

#ifndef Damper_hpp
#define Damper_hpp


#include <iostream>
#include <algorithm>
#include <cmath>

class DamperModel{
public:
    DamperModel() = default;
    explicit DamperModel(double sampleRateHz);

    void prepare(double sampleRateHz);
    void reset();

    // value01: 0.0 = damper fully lifted, 1.0 = damper fully pressed on the string.
    void setPressure(double value01);

    // value01: 0.0 = soft felt, 1.0 = hard felt.
    void setFeltHardness(double value01);

    // Convenience MIDI-style pedal mapping.
    // sustainPedal01: 0.0 = pedal up / damper down, 1.0 = pedal down / damper lifted.
    void setFromSustainPedal(double sustainPedal01);

    double getPressure() const { return pressure; }
    double getFeltHardness() const { return feltHardness; }
    bool isActive() const { return pressure > activeThreshold; }

    double process(double inputSample);

private:
    void updateCoefficients();

    static double clamp01(double value);

private:
    double sampleRate = 44100.0;

    double pressure = 0.0;
    double feltHardness = 0.5;
    double activeThreshold = 1.0e-4;

    // One-pole low-pass damper section.
    // This is intended as a first physical approximation:
    // high frequencies are dissipated quickly, low frequencies survive longer.
    double coefficientA = 0.0;
    double coefficientB = 1.0;
    double state = 0.0;
};

#endif /* Damper_hpp */
