//
//  fractional_filter.hpp
//  bbpl
//
//  Created by opus arc on 2026/9/10.
//

#ifndef fractional_filter_hpp
#define fractional_filter_hpp

#include <iostream>

class FractionalFilter {
    float a1 = 0.0;
    
    float x1 = 0.0;
    float y1 = 0.0;
    
public:
    
    FractionalFilter(double frac) {
        a1 = (1 - frac) / (1 + frac);
    }
    
    inline void process(float &x) {
         // y = a1 * x + x1 - a1 * y1;
         float y = static_cast<float>(a1 * x)
             + static_cast<float>(x1)
             - static_cast<float>(a1 * y1);
         x1 = x;
         y1 = y;
         x = y;
     }
};




#endif /* fractional_filter_hpp */
