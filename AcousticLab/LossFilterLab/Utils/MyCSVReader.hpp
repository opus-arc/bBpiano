//
//  MyCSVReader.hpp
//  bBpiano
//
//  Created by opus arc on 2026/5/17.
//

#ifndef MyCSVReader_hpp
#define MyCSVReader_hpp

#include <iostream>
#include <vector>

struct LossConstant {
    int midi_n = 69;
    
    double g = 0.999293;
    double a_1 = -0.01;
    
    LossConstant() = default;
};

class MyCSVReader {
public:
    static std::vector<LossConstant> getLossConstant();
    
};

#endif /* MyCSVReader_hpp */
