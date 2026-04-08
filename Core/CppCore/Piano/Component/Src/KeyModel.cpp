//
//  KeyModel.cpp
//  bBpiano
//
//  Created by opus arc on 2026/4/7.
//
//  This document is not AI-assisted.
//

#include "KeyModel.hpp"


KeyModel::KeyModel(){
    
    pairedWith(string_a, string_b, string_c);
    pairedWith(hammer);
    
}

void KeyModel::pairedWith(StringModel &string_a, StringModel& string_b, StringModel& string_c){
    hammer.pairedString_a = &string_a;
    hammer.pairedString_b = &string_b;
    hammer.pairedString_c = &string_c;
}

void KeyModel::pairedWith(HammerModel &hammer){
    string_a.pairedHammer = &hammer;
}

void KeyModel::keyMovement(){
    string_a.stringMovement();
    hammer.hammerMovement();
}

float KeyModel::getSample(){
    return string_a.velocityAt(0.24) + string_b.velocityAt(0.24) + string_c.velocityAt(0.24);
}


