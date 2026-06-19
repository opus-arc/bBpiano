//
//  ModelParameters.hpp
//  bBpiano
//
//  Created by opus arc on 2026/4/8.
//

#ifndef ModelParameters_hpp
#define ModelParameters_hpp

#include <iostream>

#include "./constants/TunningPresets.hpp"


class ModelParameters {
    
public:
    
    TunningPresets* tunningPresets;
    
    ModelParameters() {
        tunningPresets = new TunningPresets();
    }
    
private:
    // 禁止拷贝构造和赋值（单例规范）
    ModelParameters(const ModelParameters&) = delete;
    ModelParameters& operator=(const ModelParameters&) = delete;
};









#endif /* ModelParameters_hpp */
