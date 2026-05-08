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

        
        // 先移动，再采样
//        bBpiano.pianoMovement();
//        out[i] = bBpiano.pianoKeys[68 - 21].hammer->pairedString_a->velocityAt(0.7);
        
//        int activeCount = 0;
//        float sum = 0.0f;
//
//        for(int j = 0; j < bBpiano.pianoKeys.size(); j++){
//            if(!bBpiano.activePianoKeys[j]) continue;
//
//            auto& key = bBpiano.pianoKeys[j];
//
//            float v = 0.0f;
//
//            if (key.hammer->pairedString_a)
//                v += key.hammer->pairedString_a->velocityAt(0.7);
//
//            if (key.hammer->pairedString_b)
//                v += key.hammer->pairedString_b->velocityAt(0.7);
//
//            if (key.hammer->pairedString_c)
//                v += key.hammer->pairedString_c->velocityAt(0.7);
//
//            sum += v;
//            activeCount++;
//        }
//
//        if (activeCount > 0){
//            sum /= std::sqrt((float)activeCount);
//            
//        }
//
//        // 先做一个简单的归一化吧
//        // 双曲正切函数 在音频处理中非常常见 平滑压缩函数 输出永远在 (-1, 1) 之间
//        out[i] = tanh(sum);;
        
//        int __midi_n = 93; // A6
        int __midi_n = 69; // A4
        
//        bBpiano.pianoKeys[69 - 21].hammer->hammerMovement();
        bBpiano.pianoKeys[__midi_n - 21].hammer->hammerMovement();
        out[i] = tanh(bBpiano.pianoKeys[__midi_n - 21].hammer->getSample());

        
        
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






