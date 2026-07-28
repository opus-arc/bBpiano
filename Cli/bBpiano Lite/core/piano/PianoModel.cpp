//
//  Piano.cpp
//  bBpiano
//
//  Created by opus arc on 2026/4/4.
//

#include "PianoModel.hpp"

#include "Component/BridgeModel.hpp"

void PianoModel::note_on(int midi_n, double velocity) {
    std::cout << "note_on: " << midi_n << ", " << velocity << std::endl;
    
    midi_n = std::clamp(midi_n, 21, 108);
    pianoKeys[midi_n - 21]->key_down = true;
    pianoKeys[midi_n - 21]->key_active = true;

    pianoKeys[midi_n - 21]->hammer->setVIn(velocity);
}

void PianoModel::note_off(int midi_n, double velocity) {
    std::cout << "note_off: " << midi_n << ", " << velocity << std::endl;

    midi_n = std::clamp(midi_n, 21, 108);

    // 松键只允许制音器重新接触弦，不能立刻停掉弦的运行。
    // 若延音踏板仍踩下，StringModel::testDamper() 会继续保持制音器抬起。
    pianoKeys[midi_n - 21]->key_down = false;
}

void PianoModel::note_afterTouch(int midi_n, double pressure) {
    std::cout << "after_touch: " << midi_n << ", " << pressure << std::endl;
}

void PianoModel::softPedal_control(double depth) {
    if (depth > 0.1) {
        std::cout << "PedalPressed: Soft pedal (una corda) has been pressed." << "\n";
        std::cout << "Pedal depth: " + std::to_string(depth)  << "\n";
    } else {
        std::cout << "PedalReleased: Soft pedal (una corda) has been released."  << "\n";
    }
}

void PianoModel::harmonicPedal_control(double depth) {
    if (depth > 0.1) {
        std::cout << "PedalPressed: Harmonic pedal has been pressed." << "\n";
        std::cout << "Pedal depth: " + std::to_string(depth)  << "\n";
    } else {
        std::cout << "PedalReleased: Harmonic pedal has been released." << "\n";
    }
}

void PianoModel::sostenutoPedal_control(double depth) {
    if (depth > 0.1) {
        std::cout << "PedalPressed: Sostenuto pedal has been pressed." << "\n";
        std::cout << "Pedal depth: " + std::to_string(depth)  << "\n";
    } else {
        std::cout << "PedalReleased: Sostenuto pedal has been released."  << "\n";
    }
}

void PianoModel::sustainPedal_control(double depth) {
    if (depth > 0.1) {
        std::cout << "PedalPressed: Sustain pedal (damper) has been pressed." << "\n";
        std::cout << "Pedal depth: " + std::to_string(depth)  << "\n";
        
        test_sustainPedal_active = true;
        
    } else {
        std::cout << "PedalReleased: Sustain pedal (damper) has been released." << "\n";
        
        test_sustainPedal_active = false;
    }
}

PianoModel::PianoModel(){
    
    PianoModel::modelParameters = new ModelParameters();
    
    pianoKeys.reserve(88);

    for (int midi_n = 21; midi_n <= 108; ++midi_n) {
        pianoKeys.push_back(std::make_unique<KeyModel>(this, midi_n));
    }
    
    // test 5 * 11 + 3 * 11
    bridges.reserve(22);
    int k = 21;
    for (int i = 1; i <= 11; i++) {
        bridges.push_back(std::make_unique<BridgeModel>(i));
        BridgeModel* bridge = bridges.back().get();
        for(int j = 1; j <= 5; j++) {
            pianoKeys[k - 21]->bridge = bridge;
            if(pianoKeys[k - 21]->hammer->pairedString_a)
                bridge->strings.push_back(pianoKeys[k - 21]->hammer->pairedString_a);
            if(pianoKeys[k - 21]->hammer->pairedString_b)
                bridge->strings.push_back(pianoKeys[k - 21]->hammer->pairedString_b);
            if(pianoKeys[k - 21]->hammer->pairedString_c)
                bridge->strings.push_back(pianoKeys[k - 21]->hammer->pairedString_c);
            k++;
        }
    }
    for (int i = 1; i <= 11; i++) {
        bridges.push_back(std::make_unique<BridgeModel>(i));
        BridgeModel* bridge = bridges.back().get();
        for(int j = 1; j <= 3; j++) {
            if (k > 108) break;
            pianoKeys[k - 21]->bridge = bridge;
            if(pianoKeys[k - 21]->hammer->pairedString_a)
                bridge->strings.push_back(pianoKeys[k - 21]->hammer->pairedString_a);
            if(pianoKeys[k - 21]->hammer->pairedString_b)
                bridge->strings.push_back(pianoKeys[k - 21]->hammer->pairedString_b);
            if(pianoKeys[k - 21]->hammer->pairedString_c)
                bridge->strings.push_back(pianoKeys[k - 21]->hammer->pairedString_c);
            k++;
        }
    }

//    for(int midi_n = 21; midi_n <= 108; midi_n++) {
//        std::cout << "midi_n: " << midi_n << ", bridgeNum: " << std::to_string(pianoKeys[midi_n - 21]->bridge->test) << "\n";
//    }



//    activePianoKeys.assign(88, false);

    damper = new DamperModel();


}


void PianoModel::pianoMovement(){

    for(auto& bridge : bridges){
        bridge->processJunction();
    }

    for(auto& key : pianoKeys){
        if(key->key_active)
            key->keyMovement();
    }
    
}
