//
//  LossFilterDesigner.cpp
//  bBpiano
//
//  Created by opus arc on 2026/5/19.
//

#include "LossDesigner.hpp"

#include "../../Utils/MyPitch.hpp"
#include "../../Utils/LinearRegression.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;
std::array<QuadraticRegressionResult, 109> makeRawLossFitByMidi();

std::array<QuadraticRegressionResult, 109> smoothLossABCByMidi(
        const std::array<QuadraticRegressionResult, 109>& raw,
        int midiBegin = 21,
        int midiEnd = 108
);

void writeLossFilterConstantsCSV() {
    
    const fs::path outputPath =
        "/Users/opusarc/XCodeProjects/bBpiano/AcousticLab/StringFilterLab/Generated/loss_filter_constants.csv";

    fs::create_directories(outputPath.parent_path());

    // std::ios::trunc 会清空上一次数据
    std::ofstream out(outputPath, std::ios::trunc);

    if (!out.is_open()) {
        std::cerr << "Failed to open: " << outputPath << "\n";
        return;
    }

    out << "key,pitch,f0,g,a1,pointCount,fitError\n";

    auto rawLossFit = makeRawLossFitByMidi();
    auto smoothLossFit = smoothLossABCByMidi(rawLossFit);

    constexpr double sampleRate = 44100.0;
    constexpr bool useDoubleSidedLoss = true;

    for (int midi = 21; midi <= 108; ++midi) {
        
        const auto& p = smoothLossFit[midi];

        const double f0 = MyPitch::midiToFrequency(midi);
//        const std::string pitch = MyPitch::midiToName(midi);

        const int nyquistLimit =
            static_cast<int>(std::floor((sampleRate * 0.5) / f0));

        const int maxPartial = std::min(nyquistLimit, 32);

        std::vector<std::array<double, 2>> targets;

        for (int partial = 1; partial <= maxPartial; ++partial) {
            
            const double f = f0 * partial;
            const double x = f / 1000.0;

            double sigma =
                p.a +
                p.b * x +
                0.35 * p.c * x * x;

            const double softLimit = 80.0;
            if (sigma > softLimit) {
                sigma = softLimit + std::sqrt(sigma - softLimit);
            }

            sigma = std::clamp(sigma, 0.05, 180.0);

            // 一次完整 loop 的目标衰减
            double targetGain = std::exp(-sigma / f0);

            // 如果左右边界各滤一次，每个 filter 只承担一半 loss
            if (useDoubleSidedLoss) {
                targetGain = std::sqrt(targetGain);
            }

            targets.push_back({f, targetGain});
        }

        double bestG = 1.0;
        double bestA1 = 0.0;
        double bestError = std::numeric_limits<double>::max();

        for (double a1 = -0.98; a1 <= 0.98; a1 += 0.0005) {
            
            double numerator = 0.0;
            double denominator = 0.0;

            for (const auto& point : targets) {
                
                const double f = point[0];
                const double target = point[1];

                const double omega = 2.0 * M_PI * f / sampleRate;

                const double base =
                    (1.0 + a1) /
                    std::sqrt(1.0 + a1 * a1 + 2.0 * a1 * std::cos(omega));

                numerator += base * target;
                denominator += base * base;
            }

            if (denominator <= 0.0)
                continue;

            const double g = numerator / denominator;

            double error = 0.0;

            for (const auto& point : targets) {
                
                const double f = point[0];
                const double target = point[1];

                const double omega = 2.0 * M_PI * f / sampleRate;

                const double h =
                    g *
                    (1.0 + a1) /
                    std::sqrt(1.0 + a1 * a1 + 2.0 * a1 * std::cos(omega));

                const double e = h - target;
                error += e * e;
            }

            error /= static_cast<double>(targets.size());

            if (error < bestError) {
                bestError = error;
                bestG = g;
                bestA1 = a1;
            }
        }

        out
            << midi << ","
//            << pitch << ","
            << f0 << ","
            << bestG << ","
            << bestA1 << ","
            << targets.size() << ","
            << bestError
            << "\n";
    }

    out.close();

    std::cout << "loss filter constants written to: "
              << outputPath << "\n";
}

std::array<QuadraticRegressionResult, 109> smoothLossABCByMidi(
    const std::array<QuadraticRegressionResult, 109>& raw,
    int midiBegin,
    int midiEnd
) {
    std::array<QuadraticRegressionResult, 109> result{};

    const double sigmaMidi = 7.0;

    auto softQuality = [](double r2) {
        if (!std::isfinite(r2)) return 0.0;
        const double q = std::clamp((r2 - 0.40) / 0.60, 0.0, 1.0);
        return q * q * q;
    };

    auto nWeightOf = [](double n) {
        if (!std::isfinite(n)) return 1.0;
        return std::sqrt(std::max(1.0, n));
    };

    for (int midi = midiBegin; midi <= midiEnd; ++midi) {
        double wa = 0.0, wb = 0.0, wc = 0.0;
        double wr2 = 0.0, wn = 0.0;
        double wsumA = 0.0, wsumB = 0.0, wsumC = 0.0;
        double wsumMeta = 0.0;

        for (int sourceMidi = midiBegin; sourceMidi <= midiEnd; ++sourceMidi) {
            const auto& p = raw[sourceMidi];

            if (!std::isfinite(p.a) || !std::isfinite(p.b) || !std::isfinite(p.c))
                continue;

            const double d = static_cast<double>(midi - sourceMidi);

            const double distanceWeight =
                std::exp(-(d * d) / (2.0 * sigmaMidi * sigmaMidi));

            const double qWeight = softQuality(p.r2);
            const double nWeight = nWeightOf(double(p.n));
            const double baseWeight = distanceWeight * qWeight * nWeight;

            if (baseWeight <= 0.0)
                continue;

            double aWeight = baseWeight;
            double bWeight = baseWeight;
            double cWeight = baseWeight;

            // 高音区 b 容易因为 partial 数量少而病态偏大，温和降权。
            if (sourceMidi >= 80)
                bWeight *= 0.35;

            // 高音区主要由二次项控制高频 loss，略微提高 c 的可信度。
            if (sourceMidi >= 80)
                cWeight *= 1.25;

            wa += aWeight * p.a;
            wb += bWeight * p.b;
            wc += cWeight * p.c;

            wsumA += aWeight;
            wsumB += bWeight;
            wsumC += cWeight;

            wr2 += baseWeight * p.r2;
            wn += baseWeight * p.n;
            wsumMeta += baseWeight;
        }

        if (wsumA <= 0.0 || wsumB <= 0.0 || wsumC <= 0.0)
            continue;

        result[midi].a = wa / wsumA;
        result[midi].b = wb / wsumB;
        result[midi].c = wc / wsumC;

        if (wsumMeta > 0.0) {
            result[midi].r2 = wr2 / wsumMeta;
            result[midi].n = wn / wsumMeta;
        }
    }
    
    for (int midi = midiBegin; midi <= midiEnd; ++midi) {
        const auto& p = result[midi];

        std::cout
            << "midi: " << midi
            << ", a: " << p.a
            << ", b: " << p.b
            << ", c: " << p.c
            << ", r2: " << p.r2
            << ", n: " << p.n
            << "\n";

        const double f0 = MyPitch::midiToFrequency(midi);

        std::cout << "    sigma test:\n";

        for (int partial = 1; partial <= 8; ++partial) {
            const double f = f0 * partial;
            const double x = f / 1000.0;

            double sigma =
                p.a +
                p.b * x +
                0.35 * p.c * x * x;

            const double softLimit = 80.0;
            if (sigma > softLimit) {
                sigma = softLimit + std::sqrt(sigma - softLimit);
            }

            sigma = std::clamp(sigma, 0.05, 180.0);

            std::cout
                << "        partial: " << partial
                << ", f: " << f
                << ", sigma: " << sigma
                << "\n";
        }

        std::cout << "\n";
    }

    return result;

    return result;
}


std::array<QuadraticRegressionResult, 109> makeRawLossFitByMidi() {
    std::array<QuadraticRegressionResult, 109> data{};

    auto set = [&](int midi, double r2, double a, double b, double c, int n) {
        data[midi].r2 = r2;
        data[midi].a = a;
        data[midi].b = b;
        data[midi].c = c;
        data[midi].n = n;
    };

    set(21, 0.841597, 0.000839864, -0.00704851, 0.74194, 7);
    set(22, 0.730765, 0.0167589, -0.103913, 0.50521, 7);
    set(23, 0.792249, 0.000397848, -0.00360043, 0.43319, 8);
    set(24, 0.674653, 0.000225881, -0.00218775, 0.369733, 8);
    set(25, 0.828794, 0.00130816, -0.00991031, 0.407968, 9);
    set(26, 0.719011, 2.77309e-05, -0.000338481, 0.363884, 9);
    set(27, 0.696378, 0.000432582, -0.00375548, 0.289494, 10);
    set(28, 0.932496, 0.000505199, 0.0341535, 0.294346, 10);
    set(29, 0.781587, 0.000170963, 0.00268077, 0.525017, 11);
    set(30, 0.598602, 0.00877207, -0.0880265, 0.888701, 12);
    set(31, 0.905092, 0.00530365, -0.0398318, 0.72032, 12);
    set(32, 0.703191, -6.26454e-05, 0.0386607, 0.627317, 13);
    set(33, 0.710778, -0.0017455, 0.0584114, 0.856519, 14);
    set(34, 0.79946, 0.00299571, -0.00555117, 1.45733, 15);
    set(35, 0.838689, 0.00873248, -0.0945727, 1.64619, 16);
    set(36, 0.966038, 0.0021834, -0.000628879, 1.17528, 17);
    set(37, 0.666068, 0.00657648, -0.091297, 2.03604, 18);
    set(38, 0.889722, 0.00169997, 0.0682128, 1.18104, 19);
    set(39, 0.930781, 0.00182389, 0.0596403, 1.12997, 20);
    set(40, 0.730528, 0.00137592, 0.0183103, 1.73734, 21);
    set(41, 0.837861, 0.00232507, 0.0259511, 1.39311, 22);
    set(42, 0.937013, 0.00220308, 0.0380306, 1.25177, 24);
    set(43, 0.913698, 0.00177396, 0.0366241, 1.55162, 25);
    set(44, 0.88138, 0.000963186, 0.0362638, 1.82587, 27);
    set(45, 0.883249, -0.00248238, 0.166919, 0.745332, 28);
    set(46, 0.96627, -0.00351996, 0.200735, 0.646252, 30);
    set(47, 0.964809, -0.00225755, 0.152759, 0.775125, 32);
    set(48, 0.953203, -0.00280265, 0.184635, 0.769271, 34);
    set(49, 0.978866, -0.0031949, 0.191071, 1.08758, 36);
    set(50, 0.939674, -0.00321806, 0.190138, 1.02358, 38);
    set(51, 0.97081, -0.00301574, 0.194388, 1.04112, 40);
    set(52, 0.923058, -0.00258776, 0.161411, 1.39192, 43);
    set(53, 0.902595, -0.00162848, 0.132046, 1.65393, 45);
    set(54, 0.926154, -0.00194949, 0.132377, 1.49532, 48);
    set(55, 0.961731, -0.00169484, 0.148088, 1.55497, 51);
    set(56, 0.90664, -0.00130397, 0.112381, 2.08825, 54);
    set(57, 0.793296, -0.00134281, 0.102007, 2.17711, 57);
    set(58, 0.813575, -0.00108772, 0.0919178, 2.20889, 61);
    set(59, 0.900706, -0.00140497, 0.137249, 2.2547, 64);
    set(60, 0.85573, -0.00161353, 0.147182, 2.37759, 68);
    set(61, 0.698827, -0.00095187, 0.0879723, 3.15992, 72);
    set(62, 0.659523, -0.00092977, 0.0839801, 3.33337, 70);
    set(63, 0.867099, -0.00153745, 0.137166, 3.16097, 66);
    set(64, 0.641303, -0.00112462, 0.0905314, 3.79524, 63);
    set(65, 0.859014, -0.00184815, 0.145728, 3.42782, 59);
    set(66, 0.900938, -0.00224979, 0.175149, 3.01474, 56);
    set(67, 0.659173, -0.00182022, 0.127062, 3.50153, 53);
    set(68, 0.699468, -0.00238831, 0.153068, 3.83169, 50);
    set(69, 0.880658, -0.00153782, 0.102579, 3.10522, 47);
    set(70, 0.736965, -0.00187258, 0.110232, 3.50977, 44);
    set(71, 0.629882, -0.00239866, 0.12889, 3.29865, 42);
    set(72, 0.594003, -0.0036758, 0.182466, 3.98778, 39);
    set(73, 0.492299, -0.00157186, 0.0738131, 4.24693, 37);
    set(74, 0.69678, -0.00298618, 0.137993, 4.05956, 35);
    set(75, 0.489816, -0.00167135, 0.0706261, 3.84835, 33);
    set(76, 0.0485605, -0.000801011, 0.0275237, 4.86936, 31);
    set(77, 0.155722, -0.00184295, 0.041575, 5.20337, 29);
    set(78, 0.820445, -0.00752278, 0.285607, 3.64068, 28);
    set(79, 0.0578643, -0.000701899, 0.0224608, 4.23547, 26);
    set(80, 0.25839, -0.0016537, 0.0709622, 4.99559, 25);
    set(81, 0.765839, -0.0172386, 0.50162, 2.77722, 23);
    set(82, 0.484545, -0.00411653, 0.145137, 5.47927, 22);
    set(83, 0.933324, -0.0193255, 0.579119, 2.36642, 21);
    set(84, 0.613753, -0.0170093, 0.579309, 5.46619, 19);
    set(85, 0.403642, -0.00258473, 0.106829, 7.74933, 18);
    set(86, 0.036125, -0.00163748, 0.0481713, 8.32339, 17);
    set(87, 0.443488, -0.0111822, 0.420061, 6.71471, 16);
    set(88, 0.555745, -0.0438976, 0.887778, 5.58897, 15);
    set(89, 0.48834, -0.00504414, 0.112786, 8.10654, 14);
    set(90, 0.442373, -0.0194588, 0.346667, 8.59809, 14);
    set(91, 0.707859, -0.0315022, 0.557637, 9.44504, 13);
    set(92, 0.319009, -0.006296, 0.132196, 10.6437, 12);
    set(93, 0.784422, -0.109296, 1.6718, 3.87772, 11);
    set(94, 0.183392, -0.0164087, 0.286607, 9.36501, 11);
    set(95, 0.744149, -0.225496, 3.09048, 2.49796, 10);
    set(96, 0.926585, -0.300102, 4.46368, -0.297974, 9);
    set(97, 0.354715, -0.193954, 2.00892, 5.59655, 9);
    set(98, 0.871293, -0.10377, 1.19316, 4.26826, 8);
    set(99, 0.560351, -0.231044, 2.17584, 3.71048, 8);
    set(100, 0.663161, -0.213757, 2.03066, 5.79332, 7);
    set(101, 0.435292, -0.302343, 2.32382, 3.23109, 7);
    set(102, 0.166553, -0.161761, 1.13345, 3.9676, 7);
    set(103, 0.525047, -0.198177, 1.65256, 6.58921, 6);
    set(104, 0.353991, -0.0103532, 0.245277, 9.63382, 6);
    set(105, 0.905034, -0.621997, 4.68464, 7.95296, 5);
    set(106, 0.911502, 0.00620229, -0.0518092, 9.02553, 5);

    return data;
}
