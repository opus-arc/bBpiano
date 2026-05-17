//
//  main.cpp
//  bBpiano
//
//  Created by opus arc on 2026/4/4.
//


#include "main.hpp"
#include <iostream>
#include <atomic>
#include <time.h>

#define PI 3.1415926535897932384



static PianoModel bBpiano;

float sampleRate = 44100.0;
float frequency = 440.0;
double phase = 0.2;

static std::atomic<double> engineRate {0.0};

void get_next_buffer(float* out, int frameCount, double amplitudeLimiter) {
    
    const uint64_t start = clock_gettime_nsec_np(CLOCK_UPTIME_RAW);
    
    for (int i = 0; i < frameCount; ++i) {
        
//        int __midi_n = 93; // A6
        int __midi_n = 69; // A4
//        int __midi_n = 25; // A2
        
//        bBpiano.pianoKeys[69 - 21].hammer->hammerMovement();
        if(bBpiano.pianoKeys.size() < 87) return;
        bBpiano.pianoKeys[__midi_n - 21].hammer->hammerMovement();
        out[i] = tanh(bBpiano.pianoKeys[__midi_n - 21].hammer->getSample());
    }
    
    const uint64_t end = clock_gettime_nsec_np(CLOCK_UPTIME_RAW);
    
    const double actualNs = double(end - start);
    const double bufferNs = 1'000'000'000.0 * double(frameCount) / double(sampleRate);

    const double instantRate = actualNs / bufferNs;
    const double oldRate = engineRate.load(std::memory_order_relaxed);
    const double smoothRate = oldRate * 0.90 + instantRate * 0.10;
    engineRate.store(smoothRate, std::memory_order_relaxed);
    
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

double get_engineRate() {
    return engineRate.load(std::memory_order_relaxed);
}






