//
//  PartialSpectrumAnalyzer.hpp
//  bBpiano
//
//  Created by opus arc on 2026/5/19.
//

#ifndef PartialSpectrumAnalyzer_hpp
#define PartialSpectrumAnalyzer_hpp

#include <iostream>


struct PartialBeforeMerge {
    std::string pitchName = "A4";
    double f0 = 440.0;
    int midi_n = 69;
    std::string velocity;
    std::vector<double> partials_part;
};

class PartialSpectrumAnalyzer {
public:
    static void analyzer();
    
};

#endif /* PartialSpectrumAnalyzer_hpp */
