//
//  compute_loss.cpp
//  bBpiano
//
//  Created by opus arc on 2026/6/20.
//

#include <iostream>
#include <algorithm>
#include <cmath>

void compute_loss_constants(){
    
}

inline double getSharedPartialDecaySigma(double modalFrequencyHz) {
//    const double f = std::clamp(modalFrequencyHz, 400.0, 6000.0);
    const double f = modalFrequencyHz;
    const double x = std::log(f);
    constexpr double level = 1.8212175098933927;
    constexpr double lowSlope = 0.81788163231787625;
    constexpr double slopeChange = 4.1765125495083133;
    constexpr double transitionLogF = 8.2884972265386025;
    constexpr double sharpness = 7.9999999998560565;
    constexpr double centerLogF = 7.9448881184941751;
    const double u = sharpness * (x - transitionLogF);
    const double softplus = std::max(u, 0.0) + std::log1p(std::exp(-std::abs(u)));
    const double logSigma = level
                          + lowSlope * (x - centerLogF)
                          + slopeChange * softplus / sharpness;
    return std::exp(logSigma);
}

inline double computeFittedB(double f0_hz) {
    constexpr double k1 = -1.3333333333333333;
    constexpr double k2 =  1.3333333333333333;

    constexpr double c0 = -6.5760753629956712;
    constexpr double c1 =  1.9137732290077178;
    constexpr double c2 =  0.27614763151078042;
    constexpr double c3 = -0.28572738557770405;
    constexpr double c4 = -0.30709648310494486;

    const double x = std::log2(f0_hz / 440.0);

    double log_b =
          c0
        + c1 * x
        + c2 * x * x
        + c3 * std::pow(std::max(x - k1, 0.0), 2.0)
        + c4 * std::pow(std::max(x - k2, 0.0), 2.0);

    log_b = std::clamp(log_b,
                       std::log(5e-6),
                       std::log(2e-2));

    return std::exp(log_b);
}
