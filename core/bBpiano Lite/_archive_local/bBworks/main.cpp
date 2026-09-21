// --------------------------------------------------------------------
// MARK: 库与函数签名

#include <iostream>
#include <thread>
#include <chrono>

#include "./core/PianoModel.hpp"
#include "./core/HammerModel.hpp"
#include "./hardware/soundCard.hpp"

// MARK: MAIN

int main() {
    

    PianoModel pianoModel;
    
    
//    pianoModel.hammerModel->letsStrikeTheString();
    
    
    init(pianoModel);
    
    SoundCard::shared().start();
    std::this_thread::sleep_for(std::chrono::seconds(1));
    pianoModel.hammerModel->setVIn(1.6266395205247568);
    std::this_thread::sleep_for(std::chrono::seconds(3));
    
    SoundCard::shared().stop();
    
    return EXIT_SUCCESS;
}




//
//  main.cpp
//  bBworks
//
//  Created by opus arc on 2026/7/27.
//
