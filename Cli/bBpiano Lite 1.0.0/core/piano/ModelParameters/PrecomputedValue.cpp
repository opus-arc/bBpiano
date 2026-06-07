//
//  PrecomputedValue.cpp
//  bBpiano
//
//  Created by opus arc on 2026/5/5.
//

#include "PrecomputedValue.hpp"
#include "ModelParameters.hpp"

bool PrecomputedValue::f0s_is_latest = false;
std::vector<StringFrequencies> PrecomputedValue::f0s;


PrecomputedValue::PrecomputedValue() {

}

double PrecomputedValue::get_f0(int midi_n, int string_index) {
    
    if(!f0s_is_latest) compute_f0();
    
    midi_n = std::clamp(midi_n, 21, 108);
    string_index = std::clamp(string_index, 1, 3);

    auto it = std::find_if(
        f0s.begin(),
        f0s.end(),
        [midi_n](const StringFrequencies& x) {
            return x.midi_n == midi_n;
        }
    );

    if (it == f0s.end() || it->unison.empty()) {
        return 440.0f;
    }

    int idx = string_index - 1;

    if (idx >= static_cast<int>(it->unison.size())) {
        idx = static_cast<int>(it->unison.size()) - 1;
    }

    return static_cast<double>(it->unison[idx]);
}

void PrecomputedValue::compute_f0() {
    
    std::vector<StringFrequencies> _f0s;
    
    for(int midi_n = 21; midi_n <= 108; midi_n++) {
        if(midi_n <= 52) {
            std::vector<double> unison;
            unison.push_back(compute_f0(midi_n, 1));
            unison.push_back(compute_f0(midi_n, 2));
            _f0s.push_back(StringFrequencies(midi_n, unison));
        } else {
            std::vector<double> unison;
            unison.push_back(compute_f0(midi_n, 1));
            unison.push_back(compute_f0(midi_n, 2));
            unison.push_back(compute_f0(midi_n, 3));
            _f0s.push_back(StringFrequencies(midi_n, unison));
        }
        
    }
    
    f0s = std::move(_f0s);
    
}

double PrecomputedValue::compute_f0(int midi_n, int string_index) {
    const auto& tuning = ModelParameters::instance().tuning;;

    const float f_ref = tuning->reference_tone; // e.g. 440.0
    const int n_ref = 69; // A4
    const int n = midi_n;

    // Equal temperament base frequency
    double f_equal = f_ref * std::pow(2.0f, (n - n_ref) / 12.0f);

    // 获取当前音在八度内的位置
    int degree = (n % 12 + 12) % 12;

    double cent_offset = 0.0f;

    switch (tuning->temperament) {
    case ModelParameters::Tuning::Temperament::equal:
//        return f_equal;
            break;

    case ModelParameters::Tuning::Temperament::pythagore: {
        static const float cents[12] = {
            0, 113.7f, 203.9f, 317.6f, 407.8f, 521.5f,
            611.7f, 701.9f, 815.6f, 905.9f, 1019.5f, 1109.8f
        };
        cent_offset = cents[degree] - degree * 100.0f;
        break;
    }

    case ModelParameters::Tuning::Temperament::zarlino: {
        static const float cents[12] = {
            0, 112.0f, 204.0f, 316.0f, 386.0f, 498.0f,
            590.0f, 702.0f, 814.0f, 884.0f, 996.0f, 1088.0f
        };
        cent_offset = cents[degree] - degree * 100.0f;
        break;
    }

    case ModelParameters::Tuning::Temperament::meantone: {
        static const float cents[12] = {
            0, 76.0f, 193.2f, 310.3f, 386.3f, 503.4f,
            579.5f, 696.6f, 772.6f, 889.7f, 1006.8f, 1082.9f
        };
        cent_offset = cents[degree] - degree * 100.0f;
        break;
    }

    case ModelParameters::Tuning::Temperament::werckmeister: {
        static const float cents[12] = {
            0, 90.2f, 192.2f, 294.1f, 390.2f, 498.0f,
            588.3f, 696.6f, 792.2f, 888.3f, 996.1f, 1088.3f
        };
        cent_offset = cents[degree] - degree * 100.0f;
        break;
    }

    case ModelParameters::Tuning::Temperament::well:
        // 简化处理（可扩展为具体 well temperament）
//        return f_equal;
            break;
    }

    float total_cent = cent_offset;

    if (midi_n <= 52) {
        if (string_index == 1) {
            total_cent += tuning->unison_width / 2.0f;
        } else if (string_index == 2) {
            total_cent -= tuning->unison_width / 2.0f;
        }
    } else {
        if (string_index == 1) {
            total_cent += tuning->unison_width / 2.0f;
        } else if (string_index == 3) {
            total_cent -= tuning->unison_width / 2.0f;
        }
    }

    double f = f_equal * std::pow(2.0f, total_cent / 1200.0f);

    return f;
}
