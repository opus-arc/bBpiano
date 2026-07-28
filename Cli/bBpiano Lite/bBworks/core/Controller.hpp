//
//  Controller.hpp
//  bBworks
//
//  Created by opus arc on 2026/7/27.
//

#ifndef Controller_hpp
#define Controller_hpp

#include <iostream>
#include <atomic>
#include <time.h>

#include "PianoModel.hpp"

inline PianoModel* pianoModel = nullptr;

inline void init(PianoModel& p) {
    pianoModel = &p;
}

inline void get_next_buffer(float* out, int frameCount, double amplitudeLimiter) {
    
    for (int i = 0; i < frameCount; ++i) {
        float sample = 0.0f;

        if(pianoModel) {

            pianoModel->pianoMovement();
            sample = pianoModel->getSamples();

        }

        out[i] = sample * amplitudeLimiter;
        

    }
    
}

#endif /* Controller_hpp */
