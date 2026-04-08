//
//  KeyModel.hpp
//  bBpiano
//
//  Created by opus arc on 2026/4/7.
//
//  This document is not AI-assisted.
//

#ifndef Key_hpp
#define Key_hpp

#include <iostream>

#include "HammerModel.hpp"
#include "StringModel.hpp"

// 需要反向连接大类节点时为了避免反复引用文件
// 在小集合中使用签名方式引用
// 然后在逻辑中使用指针
class PianoModel;

class KeyModel{
    
public:
    
    // --------------------------------------------
    // MARK: 常量
    
    // 律制
    // 现在统一假设为平均律
    // 要通过这个和 midi_n 来算基频
    // Temperament
    
    

    
    // 击中速度
    // 这个是已经从按下速度换算好之后的速度
    double v_in;
    
    StringModel string_a = StringModel(438.0);
    StringModel string_b = StringModel(440.0);
    StringModel string_c = StringModel(442.0);
    HammerModel hammer = HammerModel(0.5);
    
    
    // --------------------------------------------
    // MARK: 传入值
    
    // 所属钢琴
    PianoModel *piano;
    
    // midi 序列
    int midi_n;
    
    // 基准频率 A4
    double reference_tone;
    
    // 调律
    
    
    // 单音宽度
    double unsion_width;
    
    // 直接音的持续时间
    double direct_sound_duration;
    
    
    // --------------------------------------------
    // MARK: 更新值
    
    

    
    
    // --------------------------------------------
    // MARK: 计算值
    
    // 基频
    double f0;
    
    
    // --------------------------------------------
    // MARK: 函数
    
    KeyModel();
    
    // 弦锤配对
    void pairedWith(StringModel& string_a, StringModel& string_b, StringModel& string_c);
    void pairedWith(HammerModel& hammer);
    
    // 键的运动回合，每帧的调用接口
    void keyMovement();
    
    // 获取这一帧的振幅数据
    float getSample();

    
};



#endif /* Key_hpp */
