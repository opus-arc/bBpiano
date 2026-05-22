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
    // MARK: 组件与基本信息
    //  初始化后固定的内容
    
    KeyModel(PianoModel * piano, int _midi_n);
    
    PianoModel * piano;
        
    HammerModel *hammer;
        
    // midi 序列
    int midi_n;
    
    
    // --------------------------------------------
    // MARK: 状态值
    
    mutable bool key_active = false;
    mutable bool lastStatus = false;
    
    // active 弦检测剪枝计数器
    mutable int activityCounter = 0;
    
    // --------------------------------------------
    // MARK: 函数
    
    // 键的运动回合，每帧的调用接口
    void keyMovement();
    
    // 获取这一帧的振幅数据
    float getSample();

};



#endif /* Key_hpp */
