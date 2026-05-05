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
    
//    updateActivity();
    
    
}

float KeyModel::getSample(){
    
    return hammer->getSample();
    
}

void KeyModel::updateActivity() const {
    if (++activityCounter >= 128) {
        activityCounter = 0;
        key_active = hammer->pairedString_a->active;
//        std::cout<<"midi_n: "<<midi_n<<", key_active: "<<key_active<<std::endl;
    }

}
