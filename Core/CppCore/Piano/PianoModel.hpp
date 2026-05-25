//
//  PianoModel.hpp
//  bBpiano
//
//  Created by opus arc on 2026/4/4.
//

#ifndef Piano_hpp
#define Piano_hpp

#include <iostream>
#include "Component/KeyModel.hpp"
#include "Component/PedalModel.hpp"

class PianoModel{

public:
    
    // --------------------------------------------
    // MARK: 常量
    
    
    
    // --------------------------------------------
    // MARK: 组件与基本信息
    //  初始化后固定的内容
    
    PianoModel();
    
    // 88个键
    std::vector<std::unique_ptr<KeyModel>> pianoKeys;
    
    // Soft pedal
    PedalModel *softPedal;
    
    // Harmonic pedal
    PedalModel *harmonicPedal;
    
    // Sostenuto pedal
    PedalModel *sostenutoPedal;
    
    // Sustain pedal
    PedalModel *sustainPedal;
    
    // --------------------------------------------
    // MARK: 实时值与其函数
    //  Derived Value（派生量） + Lazy Evaluation（惰性计算） + Cache（缓存）
    

    
    // --------------------------------------------
    // MARK: 状态值
    
    std::vector<bool> activePianoKeys;
    
    // active 弦检测剪枝计数器
    mutable int activityCounter = 0;
    
    // 当前全局 hammer 模式，只影响之后按下的音
    HammerMode hammerMode = HammerMode::Normal;

    
    // --------------------------------------------
    // MARK: 函数
    
    void setHammerMode(HammerMode _mode);
    void note_on(int midi_n, double velocity);
    void note_off(int midi_n, double velocity);
    void note_afterTouch(int midi_n, double pressure);
    

    
    
    // --------------------------------------------
    // MARK: 运动帧
    
    void pianoMovement();
    
    // 从单根弦上传来的信息
    void updateActivity();
    
    float getSample();

};








#endif /* Piano_hpp */
