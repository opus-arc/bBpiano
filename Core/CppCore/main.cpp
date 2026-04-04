//
//  main.cpp
//  bBpiano
//
//  Created by opus arc on 2026/4/4.
//


#include "main.hpp"
#include <iostream>
#define PI 3.1415926535897932384


void say_hello() {
    std::cout << "Hello World from C++" << std::endl;
}

double phase = 0.0;
double frequency = 440.0;
double sampleRate = 44100.0;

void get_next_buffer(float* buffer, int frameCount, double amplitudeLimiter) {
    
    
//    for(int i = 0; i < frameCount; i++) {
//        
//        // 音频代码需要显示转换
//        if (phase < 0.5)
//            buffer[i] = static_cast<float>(amplitudeLimiter);
//        else
//            buffer[i] = static_cast<float>(-amplitudeLimiter);
//            
//        
//        phase += frequency / sampleRate;
//        
//        if(phase >= 1.0) phase -= 1.0;
//        
//    }
    
    for(int i = 0; i < frameCount; i++) {

        // 音频代码需要显示转换
        buffer[i] = static_cast<float>(
                                       sin(2 * PI * phase)
                                       );
            
        
        phase += frequency / sampleRate;
        
        if(phase >= 1.0) phase -= 1.0;
        
    }
    
}
