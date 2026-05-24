//
//  DispersionDesigner.cpp
//  bBpiano
//
//  Created by opus arc on 2026/5/19.
//

#include "DispersionDesigner.hpp"
#include "../../Utils/MyPitch.hpp"

#include <filesystem>
#include <fstream>


std::array<double, 109> makeAccurateBByMidi();

void writeDispersionFilterConstantsCSV() {
    
    const std::filesystem::path outputPath =
        "/Users/opusarc/XCodeProjects/bBpiano/AcousticLab/StringFilterLab/Generated/dispersion_filter_constants.csv";
    
    std::filesystem::create_directories(outputPath.parent_path());
    
    std::ofstream out(outputPath, std::ios::trunc);
    
    if (!out.is_open()) {
        std::cerr << "Failed to open: " << outputPath << "\n";
        return;
    }
    
    out << "key,f0,B,order,a,fitError,pointCount\n";
    
    auto accurateB = makeAccurateBByMidi();
    
    constexpr double sampleRate = 44100.0;
    
    auto firstOrderAllpassGroupDelay = [](double omega, double a) {
        return (1.0 - a * a) /
               (1.0 + a * a + 2.0 * a * std::cos(omega));
    };
    
    for (int midi = 21; midi <= 108; ++midi) {
        
        const double f0 = MyPitch::midiToFrequency(midi);
        const double B = accurateB[midi];
        
        const int maxPartial =
            std::max(2, std::min(12, static_cast<int>((sampleRate * 0.45) / f0)));
        
        double bestA = 0.0;
        double bestError = std::numeric_limits<double>::max();
        
        for (double a = -0.95; a <= 0.95; a += 0.0005) {
            
            const double omega1 = 2.0 * M_PI * f0 / sampleRate;
            const double tau1 = firstOrderAllpassGroupDelay(omega1, a);
            
            double error = 0.0;
            int pointCount = 0;
            
            for (int p = 2; p <= maxPartial; ++p) {
                
                const double fp =
                    p * f0 * std::sqrt(1.0 + B * p * p);
                
                if (fp >= sampleRate * 0.45)
                    continue;
                
                const double omega = 2.0 * M_PI * fp / sampleRate;
                
                const double targetDelay =
                    (static_cast<double>(p) * sampleRate / (2.0 * fp)) -
                    (sampleRate / (2.0 * f0));
                
                const double fittedDelay =
                    firstOrderAllpassGroupDelay(omega, a) - tau1;
                
                const double w = 1.0 / static_cast<double>(p);
                const double e = fittedDelay - targetDelay;
                
                error += w * e * e;
                ++pointCount;
            }
            
            if (pointCount <= 0)
                continue;
            
            error /= static_cast<double>(pointCount);
            
            if (error < bestError) {
                bestError = error;
                bestA = a;
            }
        }
        
        out
            << midi << ","
            << f0 << ","
            << B << ","
            << 1 << ","
            << bestA << ","
            << bestError << ","
            << maxPartial - 1
            << "\n";
    }
    
    out.close();
    
    std::cout << "dispersion filter constants written to: "
              << outputPath << "\n";
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
