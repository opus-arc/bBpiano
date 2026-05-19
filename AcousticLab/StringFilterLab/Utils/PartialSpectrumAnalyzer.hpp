//
//  PartialSpectrumAnalyzer.hpp
//  bBpiano
//
//  Created by opus arc on 2026/5/19.
//

#ifndef PartialSpectrumAnalyzer_hpp
#define PartialSpectrumAnalyzer_hpp

#include <iostream>

struct Partial {
    int partialIndex = 0;
    double f = 440.0;
    std::vector<float> envelope;
    
};

class PartialSpectrumAnalyzer {
public:
    static void analyzer();
    
};

#endif /* PartialSpectrumAnalyzer_hpp */
