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


double frequency = 440.0;
double sampleRate = 44100.0;

void get_next_buffer(float* buffer, int frameCount, double amplitudeLimiter) {
    
    for(int i = 0; i < frameCount; i++) {
        
        key.keyMovement();
        
        // 音频代码需要显示转换
        buffer[i] = static_cast<float>(key.getSample());
            
    }
    
}
