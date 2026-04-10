//
//  main.cpp
//  bBpiano
//
//  Created by opus arc on 2026/4/4.
//


#include "main.hpp"
#include <iostream>
#define PI 3.1415926535897932384



static PianoModel bBpiano;

float sampleRate = 44100.0;
float frequency = 440.0;
double phase = 0.2;

void get_next_buffer(float* out, int frameCount, double amplitudeLimiter) {
    for (int i = 0; i < frameCount; ++i) {
//        
//        if (phase < 0.5)
//            out[i] = static_cast<float>(amplitudeLimiter);
//        else
//            out[i] = static_cast<float>(-amplitudeLimiter);
//        phase += frequency / sampleRate;
//        if(phase >= 1.0) phase -= 1.0;
//        
//        // 1. 先推进整台琴一个 sample
//        bBpiano.pianoMovement();
//
//        // 2. 再读取这一时刻的总输出
//        float sum = 0.0f;
//        int activeStringCount = 0;
//
//        for (auto& key : bBpiano.pianoKeys) {
//            if (!key.hammer || !key.hammer->pairedString_a) continue;
//            if (!key.hammer->pairedString_a->active) continue;
//
//            sum += key.getSample();
//            ++activeStringCount;
//        }
//
//        // 3. 防止除以 0
//        if (activeStringCount > 0) {
//            sum /= std::sqrt(static_cast<float>(activeStringCount));
//        }
//
//        // 4. 总增益
//        sum *= static_cast<float>(amplitudeLimiter);

        // 5. soft clip
//        out[i] = std::tanh(sum);

        out[i] = bBpiano.pianoKeys[69 - 21].hammer->pairedString_a->velocityAt(0.7);
        
//        bBpiano.pianoMovement();
        bBpiano.pianoKeys[69 - 21].hammer->hammerMovement();
        bBpiano.pianoKeys[69 - 21].hammer->pairedString_a->stringMovement();
        
    }
}

void note_on(int midi_n, double velocity) {
    bBpiano.note_on(midi_n, velocity);
}

void note_off(int midi_n, double velocity) {
    bBpiano.note_off(midi_n, velocity);
}

void note_afterTouch(int midi_n, double pressure) {
    bBpiano.note_afterTouch(midi_n, pressure);
}






