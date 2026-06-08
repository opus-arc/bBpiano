//
//  PrecomputedValue.hpp
//  bBpiano
//
//  Created by opus arc on 2026/5/5.
//

#ifndef PrecomputedValue_hpp
#define PrecomputedValue_hpp

#include <iostream>

struct StringFrequencies {
    int midi_n;
    std::vector<double> unison;
    StringFrequencies(int _midi_n, std::vector<double> _unison) : midi_n(_midi_n), unison(_unison) {
    }
};

class PrecomputedValue {
    
    static std::vector<StringFrequencies> f0s;
    static bool f0s_is_latest;
    
public:
    
    PrecomputedValue();
    ~PrecomputedValue();
    
    static void compute_f0();
    static double compute_f0(const int midi_n, const int string_index);
    static double get_f0(int midi_n, int string_index) ;
    
};

#endif /* PrecomputedValue_hpp */
