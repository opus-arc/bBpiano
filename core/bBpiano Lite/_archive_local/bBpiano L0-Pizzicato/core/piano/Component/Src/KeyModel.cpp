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

void KeyModel::silence() {
    key_down = false;
    key_active = false;

    auto clearString = [](auto* s) {
        if(!s) return;
        std::fill(s->left.begin(),  s->left.end(),  0.0);
        std::fill(s->right.begin(), s->right.end(), 0.0);
    };
    clearString(hammer->pairedString_a);
    clearString(hammer->pairedString_b);
    clearString(hammer->pairedString_c);
}
