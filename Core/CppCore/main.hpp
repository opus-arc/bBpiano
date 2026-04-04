//
//  main.hpp
//  bBpiano
//
//  Created by opus arc on 2026/4/4.
//

#ifndef main_hpp
#define main_hpp

#ifdef __cplusplus
extern "C" {
#endif

void say_hello();

void get_next_buffer(float* buffer, int frameCount, double amplitudeLimiter);

#ifdef __cplusplus

}
#endif

#endif /* main_hpp */
