//
//  KeyModel.cpp
//  bBpiano
//
//  Created by opus arc on 2026/4/7.
//
//  This document is not AI-assisted.
//

#include "KeyModel.hpp"


KeyModel::KeyModel(int _midi_n) :

    midi_n(_midi_n)

{
    
    hammer = new HammerModel(midi_n);
    
}

void KeyModel::keyMovement(){
    
    hammer->hammerMovement();
    
    
}

float KeyModel::getSample(){
    
    return hammer->getSample();
    
}


