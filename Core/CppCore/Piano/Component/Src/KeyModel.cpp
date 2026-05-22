//
//  KeyModel.cpp
//  bBpiano
//
//  Created by opus arc on 2026/4/7.
//
//  This document is not AI-assisted.
//

#include "KeyModel.hpp"


KeyModel::KeyModel(PianoModel * _piano, int _midi_n) :
    
    piano(_piano),
    midi_n(_midi_n)

{
    
    hammer = new HammerModel(this, midi_n);
    
}



void KeyModel::keyMovement(){
    
    hammer->hammerMovement();
    
//    if((key_active == false) && lastStatus) std::cout << "key_active is turned false!!";
//    
//    lastStatus = key_active;
    
    activityCounter++;
    if(activityCounter >= 10000) {
//        std::cout << "midi_n: " << midi_n << ", key_active: " << key_active << "\n";
//        std::cout << "&key_active: " << &key_active << "\n";
        activityCounter = 0;
    }
    
//    updateActivity();
}

float KeyModel::getSample(){
    
    
    return hammer->getSample();
    
}

