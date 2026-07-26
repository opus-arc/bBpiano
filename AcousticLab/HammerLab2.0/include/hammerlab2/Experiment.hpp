#pragma once

#include "hammerlab2/FeltModels.hpp"

#include <array>
#include <cstddef>
#include <filesystem>
#include <string>
#include <vector>

namespace hammerlab2 {

enum class ModelKind {
    power_law,
    single_stulov,
    prony_2,
};

enum class LoadKind {
    rigid_anvil,
    resistive_waveguide_port,
};

struct ExperimentConfig {
    std::string case_id = "manual";
    ModelKind model = ModelKind::single_stulov;
    LoadKind load = LoadKind::rigid_anvil;
    double sample_rate_hz = 48'000.0;
    int oversample = 4;
    double duration_seconds = 0.012;
    double impact_velocity_mps = 1.0;
    double hammer_mass_kg = 0.010;
    double port_impedance_ns_per_m = 7.0;
    int implicit_iterations = 24;
    double solver_tolerance_n = 1.0e-6;
    FeltParameters felt{};

    void validate() const;
};

struct TraceFrame {
    std::size_t internal_frame = 0;
    double time_seconds = 0.0;
    double compression_m = 0.0;
    double compression_velocity_mps = 0.0;
    double hammer_velocity_mps = 0.0;
    double surface_velocity_mps = 0.0;
    double force_n = 0.0;
    double solver_residual_n = 0.0;
    std::array<double, kMaximumRelaxationBranches> memory{};
};

struct ExperimentSummary {
    std::string case_id;
    std::string model_id;
    std::string load_id;
    double internal_sample_rate_hz = 0.0;
    double peak_force_n = 0.0;
    double peak_compression_mm = 0.0;
    double force_impulse_ns = 0.0;
    double first_contact_ms = 0.0;
    double total_contact_ms = 0.0;
    int contact_intervals = 0;
    double hysteresis_work_j = 0.0;
    double rebound_velocity_mps = 0.0;
    double maximum_solver_residual_n = 0.0;
    bool finite = true;
    bool solver_converged = true;
    double render_wall_seconds = 0.0;
    double realtime_factor = 0.0;
};

struct ExperimentResult {
    std::vector<TraceFrame> trace;
    ExperimentSummary summary;
};

[[nodiscard]] std::string to_string(ModelKind model);
[[nodiscard]] std::string to_string(LoadKind load);
[[nodiscard]] ModelKind parse_model_kind(const std::string& value);
[[nodiscard]] LoadKind parse_load_kind(const std::string& value);

[[nodiscard]] ExperimentResult run_experiment(const ExperimentConfig& config);
void write_trace_csv(const std::filesystem::path& path, const ExperimentResult& result);
void write_summary_json(
    const std::filesystem::path& path,
    const ExperimentConfig& config,
    const ExperimentSummary& summary
);

} // namespace hammerlab2
