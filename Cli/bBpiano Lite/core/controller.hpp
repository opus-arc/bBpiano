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

double get_engineRate();

void all_silence();


// 临时

void increaseLossG();
void decreaseLossG();

void increaseLossA1();
void decreaseLossA1();

void increaseDispersionA0();
void decreaseDispersionA0();

void increaseDispersionA1();
void decreaseDispersionA1();

void increaseDispersionOrder();
void decreaseDispersionOrder();


#endif /* controller_hpp */
