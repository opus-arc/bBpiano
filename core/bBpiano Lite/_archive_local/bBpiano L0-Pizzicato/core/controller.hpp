//
//  controller.hpp
//  bBpiano Lite
//
//  Created by opus arc on 2026/6/7.
//

#ifndef controller_hpp
#define controller_hpp

void bBpiano_init();
void bBpiano_shutdown();

void get_next_buffer(float* out, int frameCount, double amplitudeLimiter);
void note_on(int midi_n, double velocity);
void note_off(int midi_n, double velocity);
void note_afterTouch(int midi_n, double pressure);

void softPedal_control(double depth);
void harmonicPedal_control(double depth);
void sostenutoPedal_control(double depth);
void sustainPedal_control(double depth);

double get_engineRate();

void all_silence();


#endif /* controller_hpp */
