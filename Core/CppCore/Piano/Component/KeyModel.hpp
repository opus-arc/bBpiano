//
//  KeyModel.hpp
//  bBpiano
//
//  Created by opus arc on 2026/4/7.
//

#ifndef Key_hpp
#define Key_hpp

#include <iostream>
#include "HammerModel.hpp"
#include "StringModel.hpp"

class KeyModel{
    
public:
    
    // --------------------------------------------
    // MARK: 常量
    
    StringModel string = StringModel(440.0);
    HammerModel hammer = HammerModel(2.0);
    
    
    // --------------------------------------------
    // MARK: 传入值
    
    
    
    
    // --------------------------------------------
    // MARK: 计算值
    
    
    
    
    // --------------------------------------------
    // MARK: 函数
    
    KeyModel();
    
    // 弦锤配对
    void pairedWith(StringModel& string);
    void pairedWith(HammerModel& hammer);
    
    // 键的运动回合，每帧的调用接口
    void keyMovement();
    
    // 获取这一帧的振幅数据
    float getSample();

    
};



#endif /* Key_hpp */
