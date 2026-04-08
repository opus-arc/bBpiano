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
    // MARK: 传入值
    
    // 88个键
    KeyModel *key;
    
    // Soft pedal
    PedalModel *softPedal;
    
    // Harmonic pedal
    PedalModel *harmonicPedal;
    
    // Sostenuto pedal
    PedalModel *sostenutoPedal;
    
    // Sustain pedal
    PedalModel *sustainPedal;
    
    // --------------------------------------------
    // MARK: 计算值
    
    
    
    // --------------------------------------------
    // MARK: 运动帧
    
    void pianoMovement();
    
    // --------------------------------------------
    // MARK: 初始化
    
    
    PianoModel();
    
    
    // --------------------------------------------
    // MARK: 函数
    
    void note_on(int midi_n, double velocity);
    void note_off(int midi_n, double velocity);
    void note_afterTouch(int midi_n, double pressure);

};








#endif /* Piano_hpp */
