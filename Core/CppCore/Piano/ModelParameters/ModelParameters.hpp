//
//  ModelParameters.hpp
//  bBpiano
//
//  Created by opus arc on 2026/4/8.
//

#ifndef ModelParameters_hpp
#define ModelParameters_hpp

#include <iostream>



class ModelParameters{
    
public:
        
    static ModelParameters& instance();
    
    struct Tuning;
    Tuning *tuning;
    
    ModelParameters();
    
private:
    // 禁止拷贝构造和赋值（单例规范）
    ModelParameters(const ModelParameters&) = delete;
    ModelParameters& operator=(const ModelParameters&) = delete;
};




// 结构体一类的定义不要放在 cpp 中，否则 -> 符号不生效且被检查为不完整类型

struct ModelParameters::Tuning {
    // 这个类的所有版本号都必须大于 0
    uint32_t version = 1;
    
    // 基准频率 A4
    double reference_tone;

    // 调律
    enum class Temperament : int;
    Temperament temperament; // 枚举本身就是数字

    // 单音宽度
    double unison_width;

    // 直接音的持续时间
    double direct_sound_duration;

};

enum class ModelParameters::Tuning::Temperament {
    equal,
    pythagore,
    zarlino,
    meantone,
    well,
    werckmeister
};






#endif /* ModelParameters_hpp */
