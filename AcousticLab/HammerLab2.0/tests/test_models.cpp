#include "hammerlab2/Experiment.hpp"

#include <cmath>
#include <iostream>
#include <stdexcept>
#include <string>

namespace {

int failures = 0;

void check(bool condition, const std::string& message) {
    if (!condition) {
        ++failures;
        std::cerr << "FAIL: " << message << '\n';
    }
}

hammerlab2::FeltParameters seed_parameters() {
    hammerlab2::FeltParameters parameters;
    parameters.stiffness_n_per_mm_p = 400.0;
    parameters.exponent = 2.4;
    parameters.branches[0] = {0.51, 4.0e-4};
    parameters.branches[1] = {0.20, 8.0e-5};
    return parameters;
}

void test_power_law() {
    auto parameters = seed_parameters();
    hammerlab2::PowerLawFelt model(parameters);
    const auto zero = model.step(-1.0e-4, 0.0, 1.0 / 192000.0);
    const auto low = model.step(1.0e-4, 0.0, 1.0 / 192000.0);
    const auto high = model.step(2.0e-4, 0.0, 1.0 / 192000.0);
    check(zero.force_n == 0.0, "power law must not pull the string");
    check(high.force_n > low.force_n && low.force_n > 0.0, "power law must be monotonic");
}

void test_hereditary_relaxation() {
    auto parameters = seed_parameters();
    hammerlab2::SingleStulovFelt model(parameters);
    const double dt = 1.0 / 192000.0;
    const double first = model.step(5.0e-4, 0.0, dt).force_n;
    double settled = first;
    for (int index = 0; index < 2000; ++index) {
        settled = model.step(5.0e-4, 0.0, dt).force_n;
    }
    check(std::isfinite(settled), "hereditary force must remain finite");
    check(settled < first, "felt force must relax under held compression");
    check(settled > 0.0, "relaxed felt must retain equilibrium stiffness");

    const double memory_at_release = model.step(0.0, 0.0, dt).memory[0];
    double memory_after_rest = memory_at_release;
    for (int index = 0; index < 2000; ++index) {
        memory_after_rest = model.step(0.0, 0.0, dt).memory[0];
    }
    check(memory_after_rest < memory_at_release, "felt memory must decay while out of contact");
}

void test_experiment(hammerlab2::ModelKind model_kind) {
    hammerlab2::ExperimentConfig config;
    config.case_id = "unit";
    config.model = model_kind;
    config.load = hammerlab2::LoadKind::resistive_waveguide_port;
    config.felt = seed_parameters();
    config.felt.branch_count = model_kind == hammerlab2::ModelKind::power_law ? 0
        : model_kind == hammerlab2::ModelKind::single_stulov ? 1 : 2;
    config.implicit_iterations = 24;
    config.solver_tolerance_n = 1.0e-6;
    const auto result = hammerlab2::run_experiment(config);
    check(result.summary.finite, "experiment must be finite");
    check(result.summary.peak_force_n > 0.0, "experiment must create force");
    check(result.summary.total_contact_ms > 0.0, "experiment must register contact");
    check(result.summary.contact_intervals >= 1, "experiment must find a contact interval");
    check(result.summary.solver_converged, "implicit contact solve must converge");
    check(result.trace.size() == 2304, "experiment trace size must be deterministic");
}

void test_validation() {
    auto parameters = seed_parameters();
    parameters.branch_count = 2;
    parameters.branches[1].amplitude = 0.60;
    bool threw = false;
    try {
        parameters.validate();
    } catch (const std::invalid_argument&) {
        threw = true;
    }
    check(threw, "non-passive relaxation weights must be rejected");
}

} // namespace

int main() {
    test_power_law();
    test_hereditary_relaxation();
    test_experiment(hammerlab2::ModelKind::power_law);
    test_experiment(hammerlab2::ModelKind::single_stulov);
    test_experiment(hammerlab2::ModelKind::prony_2);
    test_validation();
    if (failures != 0) return 1;
    std::cout << "PASS: HammerLab2.0 model tests\n";
    return 0;
}
