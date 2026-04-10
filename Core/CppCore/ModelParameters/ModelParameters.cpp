//
//  ModelParameters.cpp
//  bBpiano
//
//  Created by opus arc on 2026/4/8.
//

#include "ModelParameters.hpp"



ModelParameters& ModelParameters::instance() {
    static ModelParameters instance;
    return instance;
}

ModelParameters::ModelParameters(){
    tuning = new Tuning;
    tuning->reference_tone = 440.0;
    tuning->temperament = Tuning::Temperament::equal;
    tuning->unsion_width = 0.0;
    tuning->direct_sound_duration = 0.0;
}
