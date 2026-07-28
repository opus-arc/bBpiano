//
//  HammerStringTest.cpp
//  HammerStringEvaluationTests
//
//  Created by opus arc on 2026/5/23.
//

#include "HammerStringTest.hpp"

#if defined(BBPIANO_ENABLE_HAMMER_STRING_COUPLING_TEST)

#include "../../Cli/bBpiano Lite/core/piano/PianoModel.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <complex>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <memory>
#include <numeric>
#include <streambuf>
#include <string>
#include <vector>

namespace {

constexpr double kSampleRate = 44'100.0;
constexpr double kTimeStep = 1.0 / kSampleRate;
constexpr double kObservationSeconds = 0.025;
constexpr int kObservationFrames =
    static_cast<int>(kObservationSeconds * kSampleRate);

struct CouplingResult {
    int midi = 60;
    double f0Hz = 0.0;
    double impactVelocity = 0.0;
    double peakForceN = 0.0;
    double forceImpulseNs = 0.0;
    double contactDurationMs = 0.0;
    double totalContactDurationMs = 0.0;
    double normalizedContactDuration = 0.0;
    int contactIntervals = 0;
    double forceCentroidHz = 0.0;
    double stringEnergyAtReleaseRatio = 0.0;
    double maximumTotalEnergyRatio = 0.0;
    double bridgeIncidentEnergyRatio = 0.0;
    double reboundVelocity = 0.0;
    double forceWeightedStringForceSpread = 0.0;
    bool finite = true;
};

class NullBuffer final : public std::streambuf {
protected:
    int overflow(int character) override {
        return traits_type::not_eof(character);
    }
};

class ScopedCoutSilencer final {
public:
    ScopedCoutSilencer()
        : previous_(std::cout.rdbuf(&buffer_)) {}

    ~ScopedCoutSilencer() {
        std::cout.rdbuf(previous_);
    }

private:
    NullBuffer buffer_;
    std::streambuf* previous_;
};

std::vector<StringModel*> stringsFor(HammerModel& hammer) {
    std::vector<StringModel*> strings;
    if (hammer.pairedString_a) strings.push_back(hammer.pairedString_a);
    if (hammer.pairedString_b) strings.push_back(hammer.pairedString_b);
    if (hammer.pairedString_c) strings.push_back(hammer.pairedString_c);
    return strings;
}

void resetString(StringModel& string) {
    std::fill(string.left.begin(), string.left.end(), 0.0f);
    std::fill(string.right.begin(), string.right.end(), 0.0f);
    string.leftHead = 0;
    string.rightHead = 0;

    string.fractional_x1_r = 0.0f;
    string.fractional_y1_r = 0.0f;
    string.fractional_x1_l = 0.0f;
    string.fractional_y1_l = 0.0f;

    string.loss_x1_r.fill(0.0f);
    string.loss_x2_r.fill(0.0f);
    string.loss_y1_r.fill(0.0f);
    string.loss_y2_r.fill(0.0f);
    string.loss_x1_l.fill(0.0f);
    string.loss_x2_l.fill(0.0f);
    string.loss_y1_l.fill(0.0f);
    string.loss_y2_l.fill(0.0f);

    string.dispersion_x1_r.fill(0.0f);
    string.dispersion_x2_r.fill(0.0f);
    string.dispersion_y1_r.fill(0.0f);
    string.dispersion_y2_r.fill(0.0f);
    string.dispersion_x1_l.fill(0.0f);
    string.dispersion_x2_l.fill(0.0f);
    string.dispersion_y1_l.fill(0.0f);
    string.dispersion_y2_l.fill(0.0f);

    string.damper_z1 = 0.0;
    string.damper_z2 = 0.0;
}

double waveEnergy(const std::vector<StringModel*>& strings) {
    double energy = 0.0;

    // The delay-line values are traveling-wave velocities. For one spatial
    // cell, E = mu * dx * v^2 = Z / fs * v^2.
    for (const StringModel* string : strings) {
        for (int i = 0; i < string->delay_int; ++i) {
            const double left = string->left[string->rToAIndex_l(i)];
            const double right = string->right[string->rToAIndex_r(i)];
            energy += (string->Z / kSampleRate)
                * (left * left + right * right);
        }
    }

    return energy;
}

double firstPassBridgeIncidentPower(
    const std::vector<StringModel*>& strings,
    int frame
) {
    double power = 0.0;

    // right[delay_index] is the bridge-bound incident traveling wave read by
    // StringModel::propagate() before the right boundary filter is applied.
    // Stop before the wave reflected at the agraffe can reach the bridge, so
    // the same circulating energy is not counted repeatedly.
    for (const StringModel* string : strings) {
        const int firstAgraffeReturnFrame = static_cast<int>(std::ceil(
            (1.0 + string->physical_strike_ratio) * string->delay
        ));
        if (frame >= firstAgraffeReturnFrame) continue;

        const double incident =
            string->right[string->rToAIndex_r(string->delay_index)];
        power += string->Z * incident * incident;
    }

    return power;
}

double spectralCentroid(const std::vector<double>& signal) {
    if (signal.size() < 4) return 0.0;

    const std::size_t binCount = signal.size() / 2;
    double weightedFrequency = 0.0;
    double totalPower = 0.0;

    for (std::size_t k = 1; k <= binCount; ++k) {
        std::complex<double> spectrum{0.0, 0.0};
        const double omega =
            -2.0 * std::acos(-1.0) * static_cast<double>(k)
            / static_cast<double>(signal.size());

        for (std::size_t n = 0; n < signal.size(); ++n) {
            spectrum += signal[n]
                * std::polar(1.0, omega * static_cast<double>(n));
        }

        const double power = std::norm(spectrum);
        const double frequency =
            static_cast<double>(k) * kSampleRate
            / static_cast<double>(signal.size());
        weightedFrequency += frequency * power;
        totalPower += power;
    }

    return totalPower > 0.0 ? weightedFrequency / totalPower : 0.0;
}

CouplingResult runCase(
    PianoModel& piano,
    int midi,
    double impactVelocity
) {
    KeyModel& key = *piano.pianoKeys[static_cast<std::size_t>(midi - 21)];
    HammerModel& hammer = *key.hammer;
    const auto strings = stringsFor(hammer);

    for (StringModel* string : strings) {
        resetString(*string);
    }

    hammer.setPhysicalImpactVelocity(impactVelocity);
    hammer.strings_active = true;
    hammer.activityCounter = 0;
    key.key_active = true;
    key.activityCounter = 0;

    CouplingResult result;
    result.midi = midi;
    result.f0Hz = strings.front()->get_f0();
    result.impactVelocity = impactVelocity;

    const double initialHammerEnergy =
        0.5 * hammer.m * impactVelocity * impactVelocity;
    double bridgeIncidentEnergy = 0.0;
    double maximumTotalEnergy = initialHammerEnergy;
    double stringEnergyAtRelease = 0.0;
    double stringForceSpreadWeightedSum = 0.0;
    double stringForceSpreadWeight = 0.0;
    std::vector<double> forceSignal;
    std::vector<double> stringEnergySignal;
    forceSignal.reserve(kObservationFrames);
    stringEnergySignal.reserve(kObservationFrames);

    for (int frame = 0; frame < kObservationFrames; ++frame) {
        bridgeIncidentEnergy +=
            firstPassBridgeIncidentPower(strings, frame) * kTimeStep;

        hammer.hammerMovement();

        const double force = std::max(0.0, hammer.F);
        forceSignal.push_back(force);
        result.peakForceN = std::max(result.peakForceN, force);
        result.forceImpulseNs += force * kTimeStep;

        const double hammerEnergy =
            0.5 * hammer.m * hammer.v_in * hammer.v_in;
        const double stringEnergy = waveEnergy(strings);
        stringEnergySignal.push_back(stringEnergy);

        // A hereditary felt law needs its own thermodynamically consistent
        // free-energy expression. Reusing the pure power-law spring energy
        // would double-count relaxed material state, so this invariant uses
        // only directly observable mechanical energy.
        maximumTotalEnergy = std::max(
            maximumTotalEnergy,
            hammerEnergy + stringEnergy
        );

        result.finite = result.finite
            && std::isfinite(force)
            && std::isfinite(hammer.v_in)
            && std::isfinite(hammer.dy)
            && std::isfinite(stringEnergy)
            && std::isfinite(maximumTotalEnergy);

        if (hammer.string_count > 1 && force > 0.0) {
            double minimumStringForce =
                std::numeric_limits<double>::infinity();
            double maximumStringForce = 0.0;
            double stringForceSum = 0.0;
            for (int i = 0; i < hammer.string_count; ++i) {
                const double stringForce =
                    hammer.stringForces[static_cast<std::size_t>(i)];
                minimumStringForce =
                    std::min(minimumStringForce, stringForce);
                maximumStringForce =
                    std::max(maximumStringForce, stringForce);
                stringForceSum += stringForce;
            }
            const double meanStringForce =
                stringForceSum / static_cast<double>(hammer.string_count);
            if (meanStringForce > 0.0) {
                const double spread =
                    (maximumStringForce - minimumStringForce)
                    / meanStringForce;
                stringForceSpreadWeightedSum += spread * force;
                stringForceSpreadWeight += force;
            }
        }
    }

    const double contactThreshold =
        std::max(1.0e-8, result.peakForceN * 1.0e-5);
    bool inContact = false;
    int firstContactStart = -1;
    int firstContactEnd = -1;
    int totalContactFrames = 0;

    for (int frame = 0; frame < kObservationFrames; ++frame) {
        const bool contacting = forceSignal[frame] > contactThreshold;
        if (contacting) ++totalContactFrames;

        if (contacting && !inContact) {
            ++result.contactIntervals;
            if (firstContactStart < 0) firstContactStart = frame;
        } else if (!contacting && inContact && firstContactEnd < 0) {
            firstContactEnd = frame;
            stringEnergyAtRelease = stringEnergySignal[frame];
        }
        inContact = contacting;
    }

    if (firstContactStart >= 0) {
        if (firstContactEnd < firstContactStart) {
            firstContactEnd = kObservationFrames;
            stringEnergyAtRelease = stringEnergySignal.back();
        }
        result.contactDurationMs =
            1000.0
            * static_cast<double>(firstContactEnd - firstContactStart)
            / kSampleRate;
        result.totalContactDurationMs =
            1000.0 * static_cast<double>(totalContactFrames) / kSampleRate;
        result.normalizedContactDuration =
            2.0 * result.f0Hz * result.contactDurationMs / 1000.0;
    }

    if (stringEnergyAtRelease == 0.0) {
        stringEnergyAtRelease = waveEnergy(strings);
    }

    if (initialHammerEnergy > 0.0) {
        result.stringEnergyAtReleaseRatio =
            stringEnergyAtRelease / initialHammerEnergy;
        result.maximumTotalEnergyRatio =
            maximumTotalEnergy / initialHammerEnergy;
        result.bridgeIncidentEnergyRatio =
            bridgeIncidentEnergy / initialHammerEnergy;
    }

    result.forceCentroidHz = spectralCentroid(forceSignal);
    if (stringForceSpreadWeight > 0.0) {
        result.forceWeightedStringForceSpread =
            stringForceSpreadWeightedSum / stringForceSpreadWeight;
    }
    result.reboundVelocity = hammer.v_in;
    key.key_active = false;
    return result;
}

bool isNumericallySafe(const CouplingResult& result) {
    return result.finite
        && result.peakForceN > 0.0
        && result.contactDurationMs > 0.0
        // Loss, dispersion, and passive felt cannot create observable
        // hammer-plus-string energy. Five percent allows for the discrete
        // traveling-wave energy proxy and uncounted filter-state energy.
        && result.maximumTotalEnergyRatio < 1.05;
}

const CouplingResult* findResult(
    const std::vector<CouplingResult>& results,
    int midi,
    double velocity
) {
    for (const auto& result : results) {
        if (result.midi == midi
            && std::abs(result.impactVelocity - velocity) < 1.0e-9) {
            return &result;
        }
    }
    return nullptr;
}

std::filesystem::path writeResults(
    const std::vector<CouplingResult>& results
) {
    const char* outputOverride =
        std::getenv("BBPIANO_HAMMER_BENCHMARK_CSV");
    const std::filesystem::path outputPath =
        outputOverride && *outputOverride
        ? std::filesystem::path(outputOverride)
        : std::filesystem::path(__FILE__).parent_path()
            / "HammerStringCouplingResult.csv";
    if (outputPath.has_parent_path()) {
        std::filesystem::create_directories(outputPath.parent_path());
    }
    std::ofstream output(outputPath);

    output
        << "midi,f0_hz,impact_velocity_m_s,peak_force_n,"
        << "force_impulse_n_s,first_contact_ms,total_contact_ms,"
        << "normalized_first_contact,contact_intervals,"
        << "force_centroid_hz,string_energy_at_release_ratio,"
        << "maximum_total_energy_ratio,first_pass_bridge_energy_ratio,"
        << "rebound_velocity_m_s,force_weighted_string_force_spread,finite\n";

    output << std::setprecision(12);
    for (const auto& result : results) {
        output
            << result.midi << ','
            << result.f0Hz << ','
            << result.impactVelocity << ','
            << result.peakForceN << ','
            << result.forceImpulseNs << ','
            << result.contactDurationMs << ','
            << result.totalContactDurationMs << ','
            << result.normalizedContactDuration << ','
            << result.contactIntervals << ','
            << result.forceCentroidHz << ','
            << result.stringEnergyAtReleaseRatio << ','
            << result.maximumTotalEnergyRatio << ','
            << result.bridgeIncidentEnergyRatio << ','
            << result.reboundVelocity << ','
            << result.forceWeightedStringForceSpread << ','
            << (result.finite ? 1 : 0) << '\n';
    }

    return outputPath;
}

} // namespace

int runHammerStringCouplingTest() {
    std::unique_ptr<PianoModel> piano;
    {
        ScopedCoutSilencer silenceConstructionLogs;
        piano = std::make_unique<PianoModel>();
    }

    // Disable the temporary damper so this diagnostic observes hammer-string
    // coupling plus the configured D274 loss/dispersion path.
    piano->test_sustainPedal_active = true;

    constexpr std::array<int, 8> midiNotes{
        24, 36, 48, 60, 72, 84, 96, 108
    };
    constexpr std::array<double, 3> impactVelocities{
        1.0, 4.0, 7.0
    };

    std::vector<CouplingResult> results;
    results.reserve(midiNotes.size() * impactVelocities.size());

    for (int midi : midiNotes) {
        for (double velocity : impactVelocities) {
            results.push_back(runCase(*piano, midi, velocity));
        }
    }

    bool numericalPass = true;
    const CouplingResult* largestEnergyCase = nullptr;
    for (const auto& result : results) {
        numericalPass = numericalPass && isNumericallySafe(result);
        if (!largestEnergyCase
            || result.maximumTotalEnergyRatio
                > largestEnergyCase->maximumTotalEnergyRatio) {
            largestEnergyCase = &result;
        }
    }

    int brighterWithVelocityCount = 0;
    int weakContactTimeDependenceCount = 0;
    for (int midi : midiNotes) {
        const auto* soft = findResult(results, midi, 1.0);
        const auto* loud = findResult(results, midi, 7.0);
        if (!soft || !loud) continue;

        if (loud->forceCentroidHz > soft->forceCentroidHz) {
            ++brighterWithVelocityCount;
        }

        const double denominator =
            std::max(soft->contactDurationMs, loud->contactDurationMs);
        const double relativeChange = denominator > 0.0
            ? std::abs(loud->contactDurationMs - soft->contactDurationMs)
                / denominator
            : std::numeric_limits<double>::infinity();
        if (relativeChange < 0.35) {
            ++weakContactTimeDependenceCount;
        }
    }

    std::cout
        << "D274/RT-425 hammer-string coupling diagnostic\n"
        << "Production model, 44.1 kHz, physical impact velocity = 4 m/s\n"
        << "MIDI  tc(ms)  2*f0*tc  peakF(N)  stringE/E0  bridgeEin/E0  intervals\n";

    std::cout << std::fixed << std::setprecision(3);
    for (int midi : midiNotes) {
        const auto* result = findResult(results, midi, 4.0);
        if (!result) continue;

        std::cout
            << std::setw(4) << result->midi << "  "
            << std::setw(6) << result->contactDurationMs << "  "
            << std::setw(7) << result->normalizedContactDuration << "  "
            << std::setw(8) << result->peakForceN << "  "
            << std::setw(10) << result->stringEnergyAtReleaseRatio << "  "
            << std::setw(12) << result->bridgeIncidentEnergyRatio << "  "
            << std::setw(9) << result->contactIntervals << '\n';
    }

    const auto outputPath = writeResults(results);

    std::cout
        << "Finite/stable and <=5% energy growth: "
        << (numericalPass ? "PASS" : "FAIL") << '\n'
        << "Largest total-energy ratio: "
        << (largestEnergyCase ? largestEnergyCase->maximumTotalEnergyRatio : 0.0)
        << " (MIDI "
        << (largestEnergyCase ? largestEnergyCase->midi : 0)
        << ", " << (largestEnergyCase ? largestEnergyCase->impactVelocity : 0.0)
        << " m/s)\n"
        << "Force spectrum brightens from 1 to 7 m/s: "
        << brighterWithVelocityCount << "/" << midiNotes.size() << '\n'
        << "Contact duration changes less than 35%: "
        << weakContactTimeDependenceCount << "/" << midiNotes.size() << '\n'
        << "Interpretation: 2*f0*tc near 1 means contact is near half a period.\n"
        << "CSV: " << outputPath.string() << '\n'
        << "This tests the RT-425 parameterized model and coupling invariants;\n"
        << "it is not a measured Steinway D-274 waveform comparison.\n";

    return numericalPass ? 0 : 1;
}

#if defined(BBPIANO_HAMMER_STRING_COUPLING_TEST_MAIN)
int main() {
    return runHammerStringCouplingTest();
}
#endif

#else

int runHammerStringCouplingTest() {
    return 0;
}

#endif
