//
//  PianoModel.cpp
//  bBworks
//
//  Created by opus arc on 2026/7/27.
//

#include "../PianoModel.hpp"
#include "../HammerModel.hpp"

PianoModel::PianoModel() {
    hammerModel = new HammerModel();
}
PianoModel::~PianoModel() {
    delete hammerModel;
}

float PianoModel::getSamples() {
    return hammerModel->string_a->getSamples();
}

void PianoModel::pianoMovement() {
    hammerModel->hammerMovement();
}
