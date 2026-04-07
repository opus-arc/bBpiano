//
//  main.hpp
//  bBpiano
//
//  Created by opus arc on 2026/4/4.
//

#ifndef main_hpp
#define main_hpp

#include "Piano/Component/KeyModel.hpp"

#ifdef __cplusplus
extern "C" {
#endif

KeyModel key = KeyModel();

void say_hello();

void get_next_buffer(float* buffer, int frameCount, double amplitudeLimiter);

#ifdef __cplusplus

}
#endif

#endif /* main_hpp */
