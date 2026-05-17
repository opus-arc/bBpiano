//
//  BankLossFilterDesigner.cpp
//  bBpiano
//
//  Created by opus arc on 2026/5/16.
//

#include "BankLossFilterDesigner.hpp"
#include "../../Utils/MyFFT.h"
#include "../../Utils/MyDrWav.h"

#include <atomic>
#include <iostream>


void lossFilterDesinger(){
    std::string wavPath = "/Users/opusarc/XCodeProjects/bBpiano/AcousticLab/LossFilterLab/Samples/Pianoteq 9/SingleNoteSamples/Split/A4_take03_v80.wav";
    
    // TODO: 这里临时关掉了沙盒 为了不用复制这些采样音频进入 resource，以后还是要打开的吧
    const std::vector<float> pcm = MyDrWav::loadWav(wavPath, 44100.0, 2);
    
    const std::vector<std::vector<float>> spectrogram = MyFFT::computeSpectrogram(pcm, 44100.0, 32768, 512);
    
    
    

}
