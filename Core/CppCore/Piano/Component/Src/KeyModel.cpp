//
//  KeyModel.cpp
//  bBpiano
//
//  Created by opus arc on 2026/4/7.
//

#include "KeyModel.hpp"


KeyModel::KeyModel(){
    
    pairedWith(string);
    pairedWith(hammer);
    
}

void KeyModel::pairedWith(StringModel &string){
    hammer.pairedString = &string;
}

void KeyModel::pairedWith(HammerModel &hammer){
    string.pairedHammer = &hammer;
}

void KeyModel::keyMovement(){
    string.stringMovement();
    hammer.hammerMovement();
}

float KeyModel::getSample(){
    return string.velocityAt(0.24);
}


