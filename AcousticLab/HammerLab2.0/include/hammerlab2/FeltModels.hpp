#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <stdexcept>

namespace hammerlab2 {

constexpr std::size_t kMaximumRelaxationBranches = 3;

struct RelaxationBranch {
    double amplitude = 0.0;
    double tau_seconds = 1.0e-4;
};

struct FeltParameters {
    // Input convention is N/mm^p because it is the convention used by the
    // Stulov measurements. Runtime conversion to N/m^p happens once here.
    double stiffness_n_per_mm_p = 400.0;
    double exponent = 2.4;
    std::array<RelaxationBranch, kMaximumRelaxationBranches> branches{};
    std::size_t branch_count = 0;

    [[nodiscard]] double runtime_stiffness_n_per_m_p() const noexcept {
        return stiffness_n_per_mm_p * std::pow(1000.0, exponent);
    }

    void validate() const {
        if (!(stiffness_n_per_mm_p > 0.0) || !(exponent > 1.0)) {
            throw std::invalid_argument("felt stiffness must be positive and exponent must exceed one");
        }
        if (branch_count > branches.size()) {
            throw std::invalid_argument("too many felt relaxation branches");
        }
        double amplitude_sum = 0.0;
        for (std::size_t index = 0; index < branch_count; ++index) {
            const auto& branch = branches[index];
            if (!(branch.amplitude >= 0.0) || !(branch.tau_seconds > 0.0)) {
                throw std::invalid_argument("invalid felt relaxation branch");
            }
            amplitude_sum += branch.amplitude;
        }
        if (!(amplitude_sum < 1.0)) {
            throw std::invalid_argument("relaxation amplitudes must sum to less than one");
        }
    }
};

struct FeltStepResult {
    double force_n = 0.0;
    std::array<double, kMaximumRelaxationBranches> memory{};
};

class PowerLawFelt final {
public:
    explicit PowerLawFelt(FeltParameters parameters)
        : parameters_(parameters), stiffness_(parameters.runtime_stiffness_n_per_m_p()) {
        parameters_.branch_count = 0;
        parameters_.validate();
    }

    void reset() noexcept {}

    [[nodiscard]] FeltStepResult step(
        double compression_m,
        double /* compression_velocity_mps */,
        double /* dt_seconds */
    ) noexcept {
        return {
            compression_m > 0.0
                ? stiffness_ * std::pow(compression_m, parameters_.exponent)
                : 0.0,
            {}
        };
    }

private:
    FeltParameters parameters_;
    double stiffness_ = 0.0;
};

template <std::size_t BranchCount>
class HereditaryFelt final {
    static_assert(BranchCount >= 1 && BranchCount <= kMaximumRelaxationBranches);

public:
    explicit HereditaryFelt(FeltParameters parameters)
        : parameters_(parameters), stiffness_(parameters.runtime_stiffness_n_per_m_p()) {
        parameters_.branch_count = BranchCount;
        parameters_.validate();
    }

    void reset() noexcept {
        memory_.fill(0.0);
    }

    [[nodiscard]] FeltStepResult step(
        double compression_m,
        double /* compression_velocity_mps */,
        double dt_seconds
    ) noexcept {
        if (!(dt_seconds > 0.0)) {
            return {0.0, memory_};
        }

        const double nonlinear_compression = compression_m > 0.0
            ? std::pow(compression_m, parameters_.exponent)
            : 0.0;
        double relaxed_component = 0.0;
        for (std::size_t index = 0; index < BranchCount; ++index) {
            const auto& branch = parameters_.branches[index];
            const double pole = std::exp(-dt_seconds / branch.tau_seconds);
            memory_[index] = pole * memory_[index]
                + branch.amplitude * (1.0 - pole) * nonlinear_compression;
            relaxed_component += memory_[index];
        }

        const double force = compression_m > 0.0
            ? stiffness_ * std::max(0.0, nonlinear_compression - relaxed_component)
            : 0.0;
        return {force, memory_};
    }

private:
    FeltParameters parameters_;
    double stiffness_ = 0.0;
    std::array<double, kMaximumRelaxationBranches> memory_{};
};

using SingleStulovFelt = HereditaryFelt<1>;
using Prony2Felt = HereditaryFelt<2>;

} // namespace hammerlab2
