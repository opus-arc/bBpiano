//
//  PianoModel.hpp
//  bBworks
//
//  Created by opus arc on 2026/7/27.
//

#ifndef PianoModel_hpp
#define PianoModel_hpp

#include <iostream>

class HammerModel;

class PianoModel {
public:
    HammerModel* hammerModel;
    
    PianoModel();
    ~PianoModel();
    
    float getSamples();
    void pianoMovement();
    
private:
    
};

#endif /* PianoModel_hpp */
