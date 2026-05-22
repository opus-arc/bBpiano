//
//  Piano.cpp
//  bBpiano
//
//  Created by opus arc on 2026/4/4.
//

#include "PianoModel.hpp"





void PianoModel::note_on(int midi_n, double velocity) {
    std::cout << "note_on: " << midi_n << ", " << velocity << std::endl;
    
    //     还是封一个口吧
        midi_n = std::clamp(midi_n, 21, 108);
    //     这里太重要了
    pianoKeys[midi_n - 21]->key_active = true;
    std::cout << "pianoKeys[" << midi_n << "].key_active: " << pianoKeys[midi_n - 21]->key_active << "\n";
    pianoKeys[midi_n - 21]->hammer->setVIn(velocity * 2.0);
}

void PianoModel::note_off(int midi_n, double velocity) {
    std::cout << "note_off: " << midi_n << ", " << velocity << std::endl;
}

void PianoModel::note_afterTouch(int midi_n, double pressure) {
    std::cout << "after_touch: " << midi_n << ", " << pressure << std::endl;
}

PianoModel::PianoModel(){
    pianoKeys.reserve(88);

    for (int midi_n = 21; midi_n <= 108; ++midi_n) {
        pianoKeys.push_back(std::make_unique<KeyModel>(this, midi_n));
    }
    
    activePianoKeys.assign(88, false);
}


void PianoModel::pianoMovement(){
    
    for(auto& key : pianoKeys){
        if(key->key_active)
            key->keyMovement();
    }
    
}

float PianoModel::getSample(){
    float sum = 0.0f;
    for(auto& key : pianoKeys) {
        if(key->key_active) {
            sum += key->getSample();
        }
    }
    return sum;
}
