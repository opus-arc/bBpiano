//
//  MyCSVReader.hpp
//  bBpiano
//
//  Created by opus arc on 2026/5/17.
//

#ifndef MyCSVReader_hpp
#define MyCSVReader_hpp

#include <iostream>
#include <vector>

struct LossConstant {
    int midi_n = 69;
    
    double g = 0.999293;
    double a_1 = -0.01;

};

//struct DispersionConstant {
//    double f = 0.0;
//    double a1 = 0.0;
//    int order = 1;
//};

struct RT425StringParameter {
    int key = 60;
    double f0_hz = 262.22;
    double length_m = 0.657;
    double diameter_m = 0.001006;
    double volume_density_kg_m3 = 7850.0;
    double linear_density_kg_m = 0.00623958204318;
    double tension_n = 741.0;
    double strike_position_m = 0.079;
    double strike_ratio = 0.120243531202;
};

struct RT425HammerParameter {
    int key = 60;
    int note_index = 40;
    double mass_kg = 0.00870608;
    double K = 5.84001775414e9;
    double P = 2.41808;
    double R = 5.32115980739e5;
};

class MyCSVReader {
public:
    static std::vector<LossConstant> getLossConstant();
//    static DispersionConstant getDispersionConstantByMidi(int midi_n);
    static RT425StringParameter getRT425WrappedStringParameterByMidi(int midi_n);
    static RT425HammerParameter getRT425HammerParameterByMidi(int midi_n);
    static double estimateRT425WrappedB(double frequencyHz);
    
};

#endif /* MyCSVReader_hpp */
