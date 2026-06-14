//
//  getDispersionConstants.hpp
//  bBpiano
//
//  Created by opus arc on 2026/6/14.
//

#ifndef getDispersionConstants_hpp
#define getDispersionConstants_hpp

#include <algorithm>
#include <array>
#include <cmath>
#include <complex>
#include <cstdint>
#include <limits>
#include <random>
#include <vector>

struct DispersionBiquad {
    double b0 = 0.0;
    double b1 = 0.0;
    double b2 = 1.0;
    double a1 = 0.0;
    double a2 = 0.0;
};

struct DispersionConstant {
    double f = 0.0;
    int sections = 0;
    std::array<DispersionBiquad, 8> biquads {};
};

namespace {

constexpr double kPi = 3.141592653589793238462643383279502884;
constexpr double kTwoPi = 2.0 * kPi;
constexpr double kMinR = 0.01;
constexpr double kMaxR = 0.999;
constexpr double kMinTheta = 0.001 * kPi;
constexpr double kMaxTheta = 0.999 * kPi;
constexpr std::array<int, 10> kFitPartials = {2, 3, 4, 5, 6, 8, 10, 12, 16, 20};

struct PoleParam {
    double r = 0.5;
    double theta = 0.5 * kPi;
};

struct PartialTarget {
    int n = 0;
    double frequency = 0.0;
    double omega = 0.0;
    double relativeDelay = 0.0;
};

inline int sectionCountForF0(double f0)
{
    if (f0 < 130.0) return 4;
    if (f0 < 440.0) return 6;
    if (f0 < 1800.0) return 8;
    return 6;
}

inline double unitRandom(std::mt19937& rng)
{
    return std::generate_canonical<double, 53>(rng);
}

inline std::uint32_t makeSeed(double f0, double B, double sampleRate)
{
    const auto a = static_cast<std::uint32_t>(std::llround(std::max(0.0, f0) * 1000.0));
    const auto b = static_cast<std::uint32_t>(std::llround(std::max(0.0, B) * 1000000000.0));
    const auto c = static_cast<std::uint32_t>(std::llround(std::max(0.0, sampleRate)));
    return 0x9e3779b9u ^ (a * 1664525u) ^ (b * 1013904223u) ^ (c * 747796405u);
}

void clampParams(std::array<PoleParam, 8>& params, int sections)
{
    for (int i = 0; i < sections; ++i) {
        auto& p = params[static_cast<size_t>(i)];
        p.r = std::clamp(p.r, kMinR, kMaxR);
        p.theta = std::clamp(p.theta, kMinTheta, kMaxTheta);
    }
}

} // namespace

inline DispersionBiquad makeAllpassBiquad(double r, double theta)
{
    r = std::clamp(r, kMinR, kMaxR);
    theta = std::clamp(theta, kMinTheta, kMaxTheta);

    DispersionBiquad bq;
    bq.a1 = -2.0 * r * std::cos(theta);
    bq.a2 = r * r;
    bq.b0 = bq.a2;
    bq.b1 = bq.a1;
    bq.b2 = 1.0;
    return bq;
}

inline double biquadAllpassPhaseDelay(const DispersionBiquad& bq, double omega)
{
    omega = std::clamp(omega, 1.0e-12, kPi - 1.0e-12);

    const std::complex<double> j(0.0, 1.0);
    const std::complex<double> z1 = std::exp(-j * omega);
    const std::complex<double> z2 = std::exp(-j * (2.0 * omega));

    const std::complex<double> numerator = bq.b0 + bq.b1 * z1 + bq.b2 * z2;
    const std::complex<double> denominator = 1.0 + bq.a1 * z1 + bq.a2 * z2;
    const std::complex<double> h = numerator / denominator;

    double phase = std::arg(h);
    if (phase > 0.0)
        phase -= kTwoPi;

    return -phase / omega;
}

inline double allpassBankPhaseDelay(const std::array<PoleParam, 8>& params, int sections, double omega)
{
    double delay = 0.0;

    for (int i = 0; i < sections; ++i) {
        const auto& p = params[static_cast<size_t>(i)];
        delay += biquadAllpassPhaseDelay(makeAllpassBiquad(p.r, p.theta), omega);
    }

    return delay;
}

inline double targetFn(double f0, double B, int n)
{
    const double nd = static_cast<double>(n);
    return nd * f0 * std::sqrt(std::max(0.0, 1.0 + B * nd * nd));
}

inline double targetRelativeDelay(double f0, double B, double sampleRate, int n)
{
    const double f1 = targetFn(f0, B, 1);
    const double fn = targetFn(f0, B, n);

    const double targetTotalDelay1 = sampleRate / f1;
    const double targetTotalDelayN = sampleRate * static_cast<double>(n) / fn;

    return targetTotalDelayN - targetTotalDelay1;
}

namespace {

inline std::vector<PartialTarget> makePartialTargets(double f0, double B, double sampleRate)
{
    std::vector<PartialTarget> targets;
    targets.reserve(kFitPartials.size());

    for (int n : kFitPartials) {
        const double fn = targetFn(f0, B, n);
        if (!(fn > 0.0) || fn >= 0.499 * sampleRate)
            continue;

        PartialTarget t;
        t.n = n;
        t.frequency = fn;
        t.omega = std::clamp(kTwoPi * fn / sampleRate, 1.0e-12, kPi - 1.0e-12);
        t.relativeDelay = targetRelativeDelay(f0, B, sampleRate, n);
        targets.push_back(t);
    }

    return targets;
}

inline double thetaSeparationPenalty(const std::array<PoleParam, 8>& params, int sections)
{
    double penalty = 0.0;

    for (int i = 0; i < sections; ++i) {
        for (int j = i + 1; j < sections; ++j) {
            const double d = std::abs(params[static_cast<size_t>(i)].theta -
                                      params[static_cast<size_t>(j)].theta);
            const double softMin = 0.003 * kPi;

            if (d < softMin) {
                const double x = (softMin - d) / softMin;
                penalty += x * x;
            }
        }
    }

    return penalty;
}

} // namespace

inline double objectiveFunction(const std::array<PoleParam, 8>& params,
                         int sections,
                         double f0,
                         double B,
                         double sampleRate)
{
    const auto targets = makePartialTargets(f0, B, sampleRate);
    if (targets.empty())
        return std::numeric_limits<double>::infinity();

    const double f1 = targetFn(f0, B, 1);
    if (!(f1 > 0.0) || f1 >= 0.499 * sampleRate)
        return std::numeric_limits<double>::infinity();

    const double omega1 = std::clamp(kTwoPi * f1 / sampleRate, 1.0e-12, kPi - 1.0e-12);
    const double modelDelay1 = allpassBankPhaseDelay(params, sections, omega1);

    double sum = 0.0;
    double weightSum = 0.0;

    for (const auto& t : targets) {
        const double modelRelativeDelay =
            allpassBankPhaseDelay(params, sections, t.omega) - modelDelay1;

        const double error = modelRelativeDelay - t.relativeDelay;

        const double nd = static_cast<double>(t.n);
        const double bSensitivity = std::max(B * nd * nd, 1.0e-5);
        const double weight = 1.0 / (bSensitivity * bSensitivity);

        sum += weight * error * error;
        weightSum += weight;
    }

    double regularization = 0.0;

    for (int i = 0; i < sections; ++i) {
        const auto& p = params[static_cast<size_t>(i)];
        const double rHeadroom = std::max(1.0e-6, kMaxR - p.r);
        regularization += 1.0e-7 / rHeadroom;
    }

    regularization += 1.0e-5 * thetaSeparationPenalty(params, sections);

    return sum / std::max(weightSum, 1.0e-30) + regularization;
}

namespace {

inline std::array<PoleParam, 8> deterministicInitialGuess(double f0,
                                                   double B,
                                                   double sampleRate,
                                                   int sections,
                                                   int variant)
{
    std::array<PoleParam, 8> params {};

    const double minHz = std::max(2.0 * f0, 20.0);
    const double maxHz = std::min(0.46 * sampleRate,
                                  std::max(3.0 * f0, targetFn(f0, B, 20)));

    const double thetaLo = std::clamp(kTwoPi * minHz / sampleRate, kMinTheta, kMaxTheta);
    const double thetaHi = std::clamp(kTwoPi * maxHz / sampleRate,
                                      thetaLo + 1.0e-5,
                                      kMaxTheta);

    const std::array<double, 6> radiusBias = {0.34, 0.48, 0.62, 0.76, 0.88, 0.94};
    const double bCurve = std::sqrt(std::clamp(B / (B + 0.002), 0.0, 1.0));

    for (int i = 0; i < sections; ++i) {
        const double u = sections == 1
            ? 0.5
            : static_cast<double>(i) / static_cast<double>(sections - 1);

        const double curve = std::pow(u, 0.78 + 0.05 * static_cast<double>(variant % 4));

        auto& p = params[static_cast<size_t>(i)];
        p.theta = thetaLo + (thetaHi - thetaLo) * curve;
        p.theta += 0.012 * kPi *
            std::sin(1.37 * static_cast<double>(i + 1) + 0.91 * static_cast<double>(variant));

        const double rb = radiusBias[static_cast<size_t>(variant % radiusBias.size())];
        p.r = std::clamp(rb + 0.16 * bCurve +
                         0.045 * std::cos(2.1 * static_cast<double>(i + 1) + variant),
                         kMinR,
                         kMaxR);
    }

    clampParams(params, sections);
    return params;
}

inline std::array<PoleParam, 8> randomInitialGuess(int sections, std::mt19937& rng)
{
    std::array<PoleParam, 8> params {};

    std::uniform_real_distribution<double> radiusDist(0.05, 0.985);
    std::uniform_real_distribution<double> thetaDist(kMinTheta, kMaxTheta);

    for (int i = 0; i < sections; ++i) {
        auto& p = params[static_cast<size_t>(i)];
        p.r = radiusDist(rng);
        p.theta = thetaDist(rng);
    }

    clampParams(params, sections);
    return params;
}

inline double scoreParams(const std::array<PoleParam, 8>& params,
                   int sections,
                   double f0,
                   double B,
                   double sampleRate)
{
    return objectiveFunction(params, sections, f0, B, sampleRate);
}

inline void coordinateDescent(std::array<PoleParam, 8>& params,
                       int sections,
                       double f0,
                       double B,
                       double sampleRate)
{
    double bestScore = scoreParams(params, sections, f0, B, sampleRate);
    double stepR = 0.22;
    double stepTheta = 0.18 * kPi;

    for (int pass = 0; pass < 90; ++pass) {
        bool improved = false;

        for (int i = 0; i < sections; ++i) {
            for (int variable = 0; variable < 2; ++variable) {
                for (double sign : {-1.0, 1.0}) {
                    auto trial = params;
                    auto& p = trial[static_cast<size_t>(i)];

                    if (variable == 0)
                        p.r += sign * stepR;
                    else
                        p.theta += sign * stepTheta;

                    clampParams(trial, sections);

                    const double score = scoreParams(trial, sections, f0, B, sampleRate);
                    if (score < bestScore) {
                        params = trial;
                        bestScore = score;
                        improved = true;
                    }
                }
            }
        }

        if (!improved) {
            stepR *= 0.58;
            stepTheta *= 0.58;

            if (stepR < 1.0e-5 && stepTheta < 1.0e-5)
                break;
        }
    }
}

inline void annealPolish(std::array<PoleParam, 8>& params,
                  int sections,
                  double f0,
                  double B,
                  double sampleRate,
                  std::mt19937& rng)
{
    std::normal_distribution<double> normal(0.0, 1.0);

    double currentScore = scoreParams(params, sections, f0, B, sampleRate);
    double temperature = 1.0e-4;

    for (int iter = 0; iter < 1200; ++iter) {
        auto trial = params;

        const int section = static_cast<int>(rng() % static_cast<unsigned>(sections));
        auto& p = trial[static_cast<size_t>(section)];

        const double decay = std::exp(-static_cast<double>(iter) / 520.0);
        p.r += normal(rng) * (0.020 * decay + 0.00025);
        p.theta += normal(rng) * (0.022 * kPi * decay + 0.00025);

        clampParams(trial, sections);

        const double trialScore = scoreParams(trial, sections, f0, B, sampleRate);
        const double acceptProb =
            std::exp((currentScore - trialScore) / std::max(temperature, 1.0e-14));

        if (trialScore < currentScore || unitRandom(rng) < acceptProb) {
            params = trial;
            currentScore = trialScore;
        }

        temperature *= 0.996;
    }
}

} // namespace

inline std::array<PoleParam, 8> optimizeDispersionBank(double f0,
                                                double B,
                                                double sampleRate,
                                                int sections)
{
    std::mt19937 rng(makeSeed(f0, B, sampleRate));

    std::array<PoleParam, 8> best {};
    double bestScore = std::numeric_limits<double>::infinity();

    constexpr int deterministicStarts = 12;
    constexpr int randomStarts = 12;

    for (int start = 0; start < deterministicStarts + randomStarts; ++start) {
        std::array<PoleParam, 8> candidate =
            start < deterministicStarts
                ? deterministicInitialGuess(f0, B, sampleRate, sections, start)
                : randomInitialGuess(sections, rng);

        coordinateDescent(candidate, sections, f0, B, sampleRate);
        annealPolish(candidate, sections, f0, B, sampleRate, rng);
        coordinateDescent(candidate, sections, f0, B, sampleRate);

        const double score = scoreParams(candidate, sections, f0, B, sampleRate);
        if (score < bestScore) {
            best = candidate;
            bestScore = score;
        }
    }

    clampParams(best, sections);
    return best;
}

inline DispersionConstant designDispersionFromB(double f0, double B, double sampleRate)
{
    DispersionConstant result;
    result.f = f0;
    result.sections = sectionCountForF0(f0);

    if (!(f0 > 0.0) || !(B > 0.0) || !(sampleRate > 2.0 * f0))
        return result;

    const auto params = optimizeDispersionBank(f0, B, sampleRate, result.sections);

    for (int i = 0; i < result.sections; ++i) {
        const auto& p = params[static_cast<size_t>(i)];
        result.biquads[static_cast<size_t>(i)] = makeAllpassBiquad(p.r, p.theta);
    }

    return result;
}


#endif /* getDispersionConstants_hpp */
