//
//  Piano.cpp
//  bBpiano
//
//  Created by opus arc on 2026/4/4.
//

#include "PianoModel.hpp"



void PianoModel::note_on(int midi_n, double velocity) {
    std::cout << "note_on: " << midi_n << ", " << velocity << std::endl;
    
//     这里太重要了
//    pianoKeys[midi_n - 21].key_active = true;
//    
//     还是封一个口吧
    midi_n = std::clamp(midi_n, 21, 108);
    
    pianoKeys[midi_n - 21].hammer->setVIn(velocity * 2.0);
}

void PianoModel::note_off(int midi_n, double velocity) {
    std::cout << "note_off: " << midi_n << ", " << velocity << std::endl;
}

void PianoModel::note_afterTouch(int midi_n, double pressure) {
    std::cout << "after_touch: " << midi_n << ", " << pressure << std::endl;
}

PianoModel::PianoModel(){
    for(int i = 21; i <= 108; i++)
        pianoKeys.push_back(KeyModel(this, i));
    
    activePianoKeys.assign(88, false);
}


void PianoModel::pianoMovement(){
    
    
    for(auto key : pianoKeys){
//        if(key.key_active)
            key.keyMovement();
    }
    
//    updateActivity();
}

float PianoModel::getSample(){
    return pianoKeys[69 - 21].hammer->pairedString_a->velocityAt(0.7);
}
