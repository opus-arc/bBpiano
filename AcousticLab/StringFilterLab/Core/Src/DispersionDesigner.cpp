//
//  DispersionDesigner.cpp
//  bBpiano
//
//  Created by opus arc on 2026/5/19.
//
//  No AI-assist
//

#include "DispersionDesigner.hpp"
#include "../../Utils/MyPitch.hpp"


#include <filesystem>
#include <fstream>
#include <cmath>


std::array<double, 109> makeAccurateBByMidi();


int getOrder(int midi_n);
double fitDispersionA1ForMidi(int midi_n, double sampleRate, const std::array<double, 109>& accurateB, int order, double& best_error, int& used_partial_count);

double angular_to_normal(double angular_f, double sampleRate);
double normal_to_angular(double f, double sampleRate);

double calculate_group_delay(double a1, double omega);
double calculate_constant_a1(double frac);
void first_order_allpass(float a1, float &x, float &x1, float &y1);

void writeDispersionFilterConstantsCSV() {
    using std::cout;
    
    double sampleRate = 44100.0;
    std::array<double, 109> accurateB = makeAccurateBByMidi();
    std::ofstream csv("/Users/opusarc/Projects/XCodeProjects/bBpiano/AcousticLab/StringFilterLab/Generated/dispersion_filter_constants.csv");
    csv << "key,f0,a1,order,usedPartialCount,error\n";
    
    for(int midi_n_i = 21; midi_n_i <= 108; midi_n_i++) {
        double f0 = MyPitch::midiToFrequency(midi_n_i);
        // 先计算 loop delay
        
        int order = getOrder(midi_n_i);

        double best_error = 0.0;
        int used_partial_count = 0;
        double best_a1 = fitDispersionA1ForMidi(midi_n_i, sampleRate, accurateB, order, best_error, used_partial_count);
        cout << "midi: " << midi_n_i
             << ", best_a1: " << best_a1
             << ", order: " << order
             << ", used_partial_count: " << used_partial_count
             << ", error: " << best_error
             << "\n";
        csv << midi_n_i << ","
            << f0 << ","
            << best_a1 << ","
            << order << ","
            << used_partial_count << ","
            << best_error << "\n";
    }
    
    
    
    csv.close();
}

double fitDispersionA1ForMidi(int midi_n, double sampleRate, const std::array<double, 109>& accurateB, int order, double& best_error, int& used_partial_count) {
    double f0 = MyPitch::midiToFrequency(midi_n);
    double safeHalfSampleRate = sampleRate * 0.48;
    int nyquistLimit = static_cast<int>(safeHalfSampleRate / f0);
    
    int fitPartialLimit = 0;
    double last_best_partial_a1 = 1.0;
    bool has_last_best_partial_a1 = false;
    
    for(int p = 1; p <= nyquistLimit; p++) {
        double dispersionF = p * f0 * std::sqrt(1 + accurateB[midi_n] * p * p);
        if (dispersionF >= safeHalfSampleRate) break;
        
        double dispersion_delay = sampleRate / dispersionF;
        double normalF = p * f0;
        double normal_delay = sampleRate / normalF;
        double omega = normal_to_angular(dispersionF, sampleRate);
        double delayX = normal_delay - dispersion_delay;
        
        double best_partial_a1 = 0.0;
        double min_parital_delta = 1e9;
        for(double a1 = 0.0; a1 < 0.999; a1 += 0.0001) {
            double group_delay = order * calculate_group_delay(a1, omega);
            double delta_delay = std::abs(group_delay - delayX);
            if(min_parital_delta > delta_delay) {
                min_parital_delta = delta_delay;
                best_partial_a1 = a1;
            }
        }
        
        if(has_last_best_partial_a1 && best_partial_a1 > last_best_partial_a1) break;
        
        last_best_partial_a1 = best_partial_a1;
        has_last_best_partial_a1 = true;
        fitPartialLimit = p;
    }
    
    if(fitPartialLimit <= 0) fitPartialLimit = 1;
    
    double best_a1 = 0.0;
    best_error = 1e18;
    used_partial_count = fitPartialLimit;
    
    for(double a1 = 0.0; a1 < 0.999; a1 += 0.0001) {
        double total_error = 0.0;
        
        for(int p = 1; p <= fitPartialLimit; p++) {
            double dispersionF = p * f0 * std::sqrt(1 + accurateB[midi_n] * p * p);
            if (dispersionF >= safeHalfSampleRate) break;
            
            double dispersion_delay = sampleRate / dispersionF;
            double normalF = p * f0;
            double normal_delay = sampleRate / normalF;
            double omega = normal_to_angular(dispersionF, sampleRate);
            double delayX = normal_delay - dispersion_delay;
            
            double group_delay = order * calculate_group_delay(a1, omega);
            double diff = group_delay - delayX;
            double weight = 1.0 / double(p * p);
            
            total_error += weight * diff * diff;
        }
        
        if(best_error > total_error) {
            best_error = total_error;
            best_a1 = a1;
        }
    }
    
    return best_a1;
}

int getOrder(int midi_n) {
    if(midi_n < 33) return 20;
    else if(midi_n < 51) return 16;
    else if(midi_n < 75) return 8;
    else if(midi_n < 96) return 4;
    else return 2;
}

double angular_to_normal(double angular_f, double sampleRate) {
    return angular_f * 2 * M_PI / double(sampleRate);
}
double normal_to_angular(double f, double sampleRate) {
    return 2.0 * M_PI * f / sampleRate;
}

double calculate_group_delay(double a1, double omega) {
    double denominator = 1 + a1 * a1 + 2 * a1 * cos(omega);
    if(denominator < 1e-6) throw std::runtime_error("calculate_partial_delay: 分母太小！");
    return (1 - a1 * a1) / (1 + a1 * a1 + 2 * a1 * cos(omega));
}




void first_order_allpass(double a1, double &x, double &x1, double &y1)  {
    // y = a1 * x + x1 - a1 * y1;
    double y = a1 * x + (x1) - a1 * y1;
    y1 = y;
    x1 = x;
    x = y;
}




std::array<double, 109> makeAccurateBByMidi() {
    
    std::array<double, 109> accurateB{};
    
    accurateB[21] = 6.87669e-06;
    accurateB[22] = 7.57327e-06;
    accurateB[23] = 8.34042e-06;
    accurateB[24] = 9.18528e-06;
    accurateB[25] = 1.01157e-05;
    accurateB[26] = 1.11404e-05;
    accurateB[27] = 1.22689e-05;
    accurateB[28] = 1.35117e-05;
    accurateB[29] = 1.48804e-05;
    accurateB[30] = 1.63877e-05;
    accurateB[31] = 1.80477e-05;
    accurateB[32] = 1.98759e-05;
    accurateB[33] = 2.18892e-05;
    accurateB[34] = 2.41065e-05;
    accurateB[35] = 2.65484e-05;
    accurateB[36] = 2.92377e-05;
    accurateB[37] = 3.21994e-05;
    accurateB[38] = 3.5461e-05;
    accurateB[39] = 3.90531e-05;
    accurateB[40] = 4.30091e-05;
    accurateB[41] = 4.73657e-05;
    accurateB[42] = 5.21637e-05;
    accurateB[43] = 5.74477e-05;
    accurateB[44] = 6.3267e-05;
    accurateB[45] = 6.96757e-05;
    accurateB[46] = 7.67336e-05;
    accurateB[47] = 8.45064e-05;
    accurateB[48] = 9.30666e-05;
    accurateB[49] = 0.000102494;
    accurateB[50] = 0.000112876;
    accurateB[51] = 0.00012431;
    accurateB[52] = 0.000136902;
    accurateB[53] = 0.00015077;
    accurateB[54] = 0.000166042;
    accurateB[55] = 0.000182862;
    accurateB[56] = 0.000201385;
    accurateB[57] = 0.000221785;
    accurateB[58] = 0.000244251;
    accurateB[59] = 0.000268993;
    accurateB[60] = 0.000296241;
    accurateB[61] = 0.000326249;
    accurateB[62] = 0.000359297;
    accurateB[63] = 0.000395692;
    accurateB[64] = 0.000435774;
    accurateB[65] = 0.000479917;
    accurateB[66] = 0.000528531;
    accurateB[67] = 0.000582069;
    accurateB[68] = 0.00064103;
    accurateB[69] = 0.000705964;
    accurateB[70] = 0.000777476;
    accurateB[71] = 0.000856232;
    accurateB[72] = 0.000942965;
    accurateB[73] = 0.00103848;
    accurateB[74] = 0.00114368;
    accurateB[75] = 0.00125953;
    accurateB[76] = 0.00138711;
    accurateB[77] = 0.00152762;
    accurateB[78] = 0.00168237;
    accurateB[79] = 0.00185279;
    accurateB[80] = 0.00204047;
    accurateB[81] = 0.00224716;
    accurateB[82] = 0.00247479;
    accurateB[83] = 0.00272547;
    accurateB[84] = 0.00300156;
    accurateB[85] = 0.0033056;
    accurateB[86] = 0.00364045;
    accurateB[87] = 0.00400921;
    accurateB[88] = 0.00441533;
    accurateB[89] = 0.00486259;
    accurateB[90] = 0.00535515;
    accurateB[91] = 0.00589761;
    accurateB[92] = 0.00649502;
    accurateB[93] = 0.00715294;
    accurateB[94] = 0.0078775;
    accurateB[95] = 0.00867547;
    accurateB[96] = 0.00955426;
    accurateB[97] = 0.0105221;
    accurateB[98] = 0.0115879;
    accurateB[99] = 0.0127617;
    accurateB[100] = 0.0140545;
    accurateB[101] = 0.0154781;
    accurateB[102] = 0.017046;
    accurateB[103] = 0.0187727;
    accurateB[104] = 0.0206743;
    accurateB[105] = 0.0227685;
    accurateB[106] = 0.0250749;
    accurateB[107] = 0.0276149;
    accurateB[108] = 0.0304122;
    
    return accurateB;
}
