//
//  MyPitch.hpp
//  bBpiano
//
//  Created by opus arc on 2026/5/17.
//

#ifndef MyPitch_hpp
#define MyPitch_hpp

#include <iostream>

class MyPitch {
public:
    static float getFrequency(const std::string& noteName);
    static float midiToFrequency(const int midi);
    static int nameToMidi(const std::string& noteName);
    static std::string findName(const std::string& fileName);
};

#endif /* MyPitch_hpp */
