#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

#define private public
#include "../core/HammerModel.hpp"
#undef private

namespace {

double waveEnergy(const StringModel& string) {
    double sumSquares = 0.0;
    for (int index = 0; index < string.delay_int; ++index) {
        const double left = string.left[index];
        const double right = string.right[index];
        sumSquares += left * left + right * right;
    }

    // For y[n] = a*x[n] + x[n-1] - a*y[n-1], define the state
    // s[n] = x[n] - a*y[n].  Its lossless storage term is
    // s[n]^2 / (1 - a^2), because
    // x[n]^2 + storage[n-1] = y[n]^2 + storage[n].
    const double allpassA = string.fractional_a1;
    const double allpassDenominator = 1.0 - allpassA * allpassA;
    if (allpassDenominator > 0.0) {
        const double leftState =
            string.fractional_x1_l
            - allpassA * string.fractional_y1_l;
        const double rightState =
            string.fractional_x1_r
            - allpassA * string.fractional_y1_r;
        sumSquares +=
            (leftState * leftState + rightState * rightState)
            / allpassDenominator;
    }

    return string.z * string.dt * sumSquares;
}

struct Arguments {
    double velocity = 2.0;
    std::filesystem::path forceLog = "hammer_energy_guard_force.txt";
};

Arguments parseArguments(int argc, char** argv) {
    Arguments arguments;
    for (int index = 1; index < argc; ++index) {
        const std::string option = argv[index];
        if (option == "--velocity" && index + 1 < argc) {
            arguments.velocity = std::stod(argv[++index]);
        } else if (option == "--force-log" && index + 1 < argc) {
            arguments.forceLog = argv[++index];
        } else {
            throw std::runtime_error(
                "Usage: hammer_energy_guard "
                "[--velocity M_PER_S] [--force-log PATH]"
            );
        }
    }
    if (!(arguments.velocity > 0.0)) {
        throw std::runtime_error("Velocity must be positive.");
    }
    return arguments;
}

} // namespace

int main(int argc, char** argv) {
    try {
        const Arguments arguments = parseArguments(argc, argv);
        if (arguments.forceLog.has_parent_path()) {
            std::filesystem::create_directories(
                arguments.forceLog.parent_path()
            );
        }
        std::ofstream forceLog(arguments.forceLog);
        if (!forceLog) {
            throw std::runtime_error("Cannot open force log.");
        }
        forceLog << std::setprecision(15);

        HammerModel hammer;
        hammer.setVIn(arguments.velocity);
        StringModel& string = *hammer.string_a;

        const double initialHammerEnergy =
            0.5 * hammer.m * arguments.velocity * arguments.velocity;
        double maximumMechanicalEnergy = initialHammerEnergy;
        double stringEnergyAtRelease = 0.0;
        double forceImpulse = 0.0;
        double maximumForce = 0.0;
        int firstContactSample = -1;
        int lastContactSample = -1;
        int positiveForceSamples = 0;
        bool wasContacting = false;
        bool finite = true;

        const int observationSamples = static_cast<int>(
            std::round(0.020 / hammer.dt)
        );
        for (int sample = 0; sample < observationSamples; ++sample) {
            hammer.hammerMovement();

            const double force = hammer.getContactForce();
            const double hammerEnergy =
                0.5 * hammer.m * hammer.v * hammer.v;
            const double stringEnergy = waveEnergy(string);
            maximumMechanicalEnergy = std::max(
                maximumMechanicalEnergy,
                hammerEnergy + stringEnergy
            );
            finite =
                finite
                && std::isfinite(force)
                && std::isfinite(hammerEnergy)
                && std::isfinite(stringEnergy);

            const bool contacting = force > 0.0;
            if (contacting) {
                if (firstContactSample < 0) {
                    firstContactSample = sample;
                }
                lastContactSample = sample;
                ++positiveForceSamples;
                maximumForce = std::max(maximumForce, force);
                forceImpulse += force * hammer.dt;
                forceLog
                    << "Time: " << sample * hammer.dt
                    << ", F: " << force
                    << '\n';
            } else if (wasContacting && stringEnergyAtRelease == 0.0) {
                stringEnergyAtRelease = stringEnergy;
            }
            wasContacting = contacting;
        }

        if (stringEnergyAtRelease == 0.0) {
            stringEnergyAtRelease = waveEnergy(string);
        }

        const double finalHammerEnergy =
            0.5 * hammer.m * hammer.v * hammer.v;
        const double finalStringEnergy = waveEnergy(string);
        const double reboundVelocity = hammer.v;
        const double maximumEnergyRatio =
            maximumMechanicalEnergy / initialHammerEnergy;
        const double releaseEnergyRatio =
            stringEnergyAtRelease / initialHammerEnergy;
        const double contactSpan =
            firstContactSample >= 0
            ? (lastContactSample - firstContactSample) * hammer.dt
            : 0.0;
        const double physicalF0 =
            std::sqrt(string.tension / string.mu)
            / (2.0 * string.length);
        const double f0RelativeError =
            std::abs(physicalF0 - string.f0) / string.f0;

        const StringModel::SpatialPort integerPort(0.5, 10);
        const bool integerPortPass =
            integerPort.index0 == 5
            && integerPort.weight0 == 1.0
            && integerPort.weight1 == 0.0;

        const double displacementBeforeReset =
            string.getDisplacementAtStrike();
        hammer.setVIn(arguments.velocity);
        const double resetPositionError =
            std::abs(hammer.x - displacementBeforeReset);

        constexpr double energyTolerance = 1.0e-6;
        const bool pass =
            finite
            && positiveForceSamples > 0
            && maximumEnergyRatio <= 1.0 + energyTolerance
            && f0RelativeError <= 1.0e-12
            && integerPortPass
            && resetPositionError <= 1.0e-15;

        std::cout
            << std::setprecision(12)
            << "status=" << (pass ? "PASS" : "FAIL") << '\n'
            << "velocity_m_s=" << arguments.velocity << '\n'
            << "force_log=" << arguments.forceLog << '\n'
            << "positive_force_samples=" << positiveForceSamples << '\n'
            << "contact_span_s=" << contactSpan << '\n'
            << "maximum_force_N=" << maximumForce << '\n'
            << "force_impulse_Ns=" << forceImpulse << '\n'
            << "initial_hammer_energy_J=" << initialHammerEnergy << '\n'
            << "maximum_mechanical_energy_J="
            << maximumMechanicalEnergy << '\n'
            << "maximum_mechanical_energy_ratio="
            << maximumEnergyRatio << '\n'
            << "string_energy_at_release_J="
            << stringEnergyAtRelease << '\n'
            << "string_energy_at_release_ratio="
            << releaseEnergyRatio << '\n'
            << "final_hammer_energy_J=" << finalHammerEnergy << '\n'
            << "final_string_energy_J=" << finalStringEnergy << '\n'
            << "rebound_velocity_m_s=" << reboundVelocity << '\n'
            << "physical_f0_hz=" << physicalF0 << '\n'
            << "configured_f0_hz=" << string.f0 << '\n'
            << "f0_relative_error=" << f0RelativeError << '\n'
            << "integer_spatial_port_pass="
            << integerPortPass << '\n'
            << "strike_reset_position_error_m="
            << resetPositionError << '\n';

        return pass ? EXIT_SUCCESS : EXIT_FAILURE;
    } catch (const std::exception& error) {
        std::cerr << "hammer_energy_guard: " << error.what() << '\n';
        return EXIT_FAILURE;
    }
}
