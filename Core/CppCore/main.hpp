//
//  main.hpp
//  bBpiano
//
//  Created by opus arc on 2026/4/4.
//

#ifndef main_hpp
#define main_hpp

#include "Piano/PianoModel.hpp"

#ifdef __cplusplus
extern "C" {
#endif

void get_next_buffer(float* out, int frameCount, double amplitudeLimiter);
void note_on(int midi_n, double velocity);
void note_off(int midi_n, double velocity);
void note_afterTouch(int midi_n, double pressure);

//void get_next_buffer(float* buffer, int frameCount, double amplitudeLimiter);

#ifdef __cplusplus

}
#endif

#endif /* main_hpp */
