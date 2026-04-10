//
//  Piano.cpp
//  bBpiano
//
//  Created by opus arc on 2026/4/4.
//

#include "PianoModel.hpp"



void PianoModel::note_on(int midi_n, double velocity) {
    std::cout << "note_on: " << midi_n << ", " << velocity << std::endl;
    pianoKeys[midi_n - 21].hammer->setVIn(velocity / 120.0);
}

void PianoModel::note_off(int midi_n, double velocity) {
    std::cout << "note_off: " << midi_n << ", " << velocity << std::endl;
}

void PianoModel::note_afterTouch(int midi_n, double pressure) {
    std::cout << "after_touch: " << midi_n << ", " << pressure << std::endl;
}


PianoModel::PianoModel(){
    for(int i = 21; i <= 108; i++)
        pianoKeys.push_back(KeyModel(i));
}


void PianoModel::pianoMovement(){
    for(auto key : pianoKeys){
        key.hammer->hammerMovement();
    }
}

float PianoModel::getSample(){
    return pianoKeys[69 - 21].hammer->pairedString_a->velocityAt(0.7);
}
