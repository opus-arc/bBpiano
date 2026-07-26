#include "hammerlab2/Experiment.hpp"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <limits>
#include <stdexcept>

namespace hammerlab2 {
namespace {

template <typename FeltModel>
ExperimentResult simulate(const ExperimentConfig& config, FeltModel model) {
    const double internal_rate = config.sample_rate_hz * config.oversample;
    const double dt = 1.0 / internal_rate;
    const std::size_t frame_count = static_cast<std::size_t>(
        std::ceil(config.duration_seconds * internal_rate)
    );

    ExperimentResult result;
    result.trace.reserve(frame_count);
    result.summary.case_id = config.case_id;
    result.summary.model_id = to_string(config.model);
    result.summary.load_id = to_string(config.load);
    result.summary.internal_sample_rate_hz = internal_rate;

    double hammer_velocity = config.impact_velocity_mps;
    double compression = 0.0;
    double previous_force = 0.0;
    bool previously_in_contact = false;
    bool first_contact_open = false;
    std::size_t first_contact_frames = 0;
    std::size_t total_contact_frames = 0;

    const auto start_time = std::chrono::steady_clock::now();
    for (std::size_t frame = 0; frame < frame_count; ++frame) {
        const FeltModel model_at_start = model;
        double force_guess = previous_force;
        double final_force = 0.0;
        double final_compression = compression;
        double final_hammer_velocity = hammer_velocity;
        double final_surface_velocity = 0.0;
        double final_compression_velocity = 0.0;
        double final_residual = std::numeric_limits<double>::infinity();
        FeltModel final_model = model_at_start;
        FeltStepResult final_felt_result{};

        for (int iteration = 0; iteration < config.implicit_iterations; ++iteration) {
            const double candidate_hammer_velocity = hammer_velocity
                - dt * force_guess / config.hammer_mass_kg;
            const double candidate_surface_velocity =
                config.load == LoadKind::resistive_waveguide_port
                    ? force_guess / config.port_impedance_ns_per_m
                    : 0.0;
            const double candidate_compression = std::max(
                0.0,
                compression + dt * (
                    candidate_hammer_velocity - candidate_surface_velocity
                )
            );
            const double candidate_compression_velocity =
                (candidate_compression - compression) / dt;

            FeltModel candidate_model = model_at_start;
            const FeltStepResult candidate_result = candidate_model.step(
                candidate_compression,
                candidate_compression_velocity,
                dt
            );
            const double candidate_force = candidate_result.force_n;
            final_residual = std::abs(candidate_force - force_guess);
            final_force = candidate_force;
            final_compression = candidate_compression;
            final_hammer_velocity = candidate_hammer_velocity;
            final_surface_velocity = candidate_surface_velocity;
            final_compression_velocity = candidate_compression_velocity;
            final_model = candidate_model;
            final_felt_result = candidate_result;
            if (final_residual <= config.solver_tolerance_n) {
                break;
            }
            // Damped fixed-point iteration avoids the one-sample fictitious
            // force delay while remaining deterministic and allocation-free.
            force_guess = 0.5 * force_guess + 0.5 * candidate_force;
        }

        model = final_model;
        hammer_velocity = final_hammer_velocity;
        compression = final_compression;
        previous_force = final_force;

        const bool in_contact = final_force > 1.0e-9;
        if (in_contact) {
            ++total_contact_frames;
            if (!previously_in_contact) {
                ++result.summary.contact_intervals;
                if (result.summary.contact_intervals == 1) {
                    first_contact_open = true;
                }
            }
            if (first_contact_open) {
                ++first_contact_frames;
            }
        } else if (previously_in_contact && first_contact_open) {
            first_contact_open = false;
        }
        previously_in_contact = in_contact;

        result.summary.peak_force_n = std::max(
            result.summary.peak_force_n,
            final_force
        );
        result.summary.peak_compression_mm = std::max(
            result.summary.peak_compression_mm,
            1000.0 * compression
        );
        result.summary.force_impulse_ns += final_force * dt;
        result.summary.maximum_solver_residual_n = std::max(
            result.summary.maximum_solver_residual_n,
            final_residual
        );
        result.summary.solver_converged = result.summary.solver_converged
            && final_residual <= config.solver_tolerance_n;
        result.summary.finite = result.summary.finite
            && std::isfinite(final_force)
            && std::isfinite(compression)
            && std::isfinite(hammer_velocity)
            && std::isfinite(final_residual);

        if (!result.trace.empty()) {
            const auto& previous = result.trace.back();
            result.summary.hysteresis_work_j += 0.5
                * (previous.force_n + final_force)
                * (compression - previous.compression_m);
        }

        result.trace.push_back({
            frame,
            static_cast<double>(frame) * dt,
            compression,
            final_compression_velocity,
            hammer_velocity,
            final_surface_velocity,
            final_force,
            final_residual,
            final_felt_result.memory,
        });
    }

    result.summary.render_wall_seconds = std::chrono::duration<double>(
        std::chrono::steady_clock::now() - start_time
    ).count();
    result.summary.realtime_factor = result.summary.render_wall_seconds
        / config.duration_seconds;
    result.summary.first_contact_ms = 1000.0 * first_contact_frames * dt;
    result.summary.total_contact_ms = 1000.0 * total_contact_frames * dt;
    result.summary.rebound_velocity_mps = hammer_velocity;
    return result;
}

std::string escape_json(const std::string& input) {
    std::string result;
    result.reserve(input.size());
    for (const char character : input) {
        switch (character) {
            case '\\': result += "\\\\"; break;
            case '"': result += "\\\""; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            default: result += character; break;
        }
    }
    return result;
}

} // namespace

void ExperimentConfig::validate() const {
    if (!(sample_rate_hz > 0.0) || oversample < 1 || !(duration_seconds > 0.0)) {
        throw std::invalid_argument("invalid time configuration");
    }
    if (!(impact_velocity_mps > 0.0) || !(hammer_mass_kg > 0.0)) {
        throw std::invalid_argument("invalid hammer mass or velocity");
    }
    if (load == LoadKind::resistive_waveguide_port
        && !(port_impedance_ns_per_m > 0.0)) {
        throw std::invalid_argument("waveguide port impedance must be positive");
    }
    if (implicit_iterations < 1 || !(solver_tolerance_n > 0.0)) {
        throw std::invalid_argument("invalid implicit solver settings");
    }
    felt.validate();
}

std::string to_string(ModelKind model) {
    switch (model) {
        case ModelKind::power_law: return "power";
        case ModelKind::single_stulov: return "stulov";
        case ModelKind::prony_2: return "prony2";
    }
    throw std::invalid_argument("unknown model kind");
}

std::string to_string(LoadKind load) {
    switch (load) {
        case LoadKind::rigid_anvil: return "rigid";
        case LoadKind::resistive_waveguide_port: return "resistive-port";
    }
    throw std::invalid_argument("unknown load kind");
}

ModelKind parse_model_kind(const std::string& value) {
    if (value == "power") return ModelKind::power_law;
    if (value == "stulov") return ModelKind::single_stulov;
    if (value == "prony2") return ModelKind::prony_2;
    throw std::invalid_argument("model must be power, stulov, or prony2");
}

LoadKind parse_load_kind(const std::string& value) {
    if (value == "rigid") return LoadKind::rigid_anvil;
    if (value == "resistive-port") return LoadKind::resistive_waveguide_port;
    throw std::invalid_argument("load must be rigid or resistive-port");
}

ExperimentResult run_experiment(const ExperimentConfig& config) {
    config.validate();
    switch (config.model) {
        case ModelKind::power_law:
            return simulate(config, PowerLawFelt(config.felt));
        case ModelKind::single_stulov:
            return simulate(config, SingleStulovFelt(config.felt));
        case ModelKind::prony_2:
            return simulate(config, Prony2Felt(config.felt));
    }
    throw std::invalid_argument("unknown model kind");
}

void write_trace_csv(const std::filesystem::path& path, const ExperimentResult& result) {
    if (!path.parent_path().empty()) {
        std::filesystem::create_directories(path.parent_path());
    }
    std::ofstream output(path);
    if (!output) throw std::runtime_error("could not write trace CSV");
    output << "internal_frame,time_sec,compression_m,compression_velocity_mps,"
              "hammer_velocity_mps,surface_velocity_mps,force_n,solver_residual_n,"
              "memory_0,memory_1,memory_2\n";
    output << std::setprecision(17);
    for (const auto& frame : result.trace) {
        output << frame.internal_frame << ','
            << frame.time_seconds << ','
            << frame.compression_m << ','
            << frame.compression_velocity_mps << ','
            << frame.hammer_velocity_mps << ','
            << frame.surface_velocity_mps << ','
            << frame.force_n << ','
            << frame.solver_residual_n << ','
            << frame.memory[0] << ','
            << frame.memory[1] << ','
            << frame.memory[2] << '\n';
    }
}

void write_summary_json(
    const std::filesystem::path& path,
    const ExperimentConfig& config,
    const ExperimentSummary& summary
) {
    if (!path.parent_path().empty()) {
        std::filesystem::create_directories(path.parent_path());
    }
    std::ofstream output(path);
    if (!output) throw std::runtime_error("could not write summary JSON");
    output << std::setprecision(17)
        << "{\n"
        << "  \"schema_version\": 1,\n"
        << "  \"case_id\": \"" << escape_json(summary.case_id) << "\",\n"
        << "  \"model_id\": \"" << summary.model_id << "\",\n"
        << "  \"load_id\": \"" << summary.load_id << "\",\n"
        << "  \"sample_rate_hz\": " << config.sample_rate_hz << ",\n"
        << "  \"oversample\": " << config.oversample << ",\n"
        << "  \"internal_sample_rate_hz\": " << summary.internal_sample_rate_hz << ",\n"
        << "  \"impact_velocity_mps\": " << config.impact_velocity_mps << ",\n"
        << "  \"hammer_mass_kg\": " << config.hammer_mass_kg << ",\n"
        << "  \"felt_stiffness_n_per_mm_p\": " << config.felt.stiffness_n_per_mm_p << ",\n"
        << "  \"felt_exponent\": " << config.felt.exponent << ",\n"
        << "  \"peak_force_n\": " << summary.peak_force_n << ",\n"
        << "  \"peak_compression_mm\": " << summary.peak_compression_mm << ",\n"
        << "  \"force_impulse_ns\": " << summary.force_impulse_ns << ",\n"
        << "  \"first_contact_ms\": " << summary.first_contact_ms << ",\n"
        << "  \"total_contact_ms\": " << summary.total_contact_ms << ",\n"
        << "  \"contact_intervals\": " << summary.contact_intervals << ",\n"
        << "  \"hysteresis_work_j\": " << summary.hysteresis_work_j << ",\n"
        << "  \"rebound_velocity_mps\": " << summary.rebound_velocity_mps << ",\n"
        << "  \"maximum_solver_residual_n\": " << summary.maximum_solver_residual_n << ",\n"
        << "  \"finite\": " << (summary.finite ? "true" : "false") << ",\n"
        << "  \"solver_converged\": " << (summary.solver_converged ? "true" : "false") << ",\n"
        << "  \"render_wall_seconds\": " << summary.render_wall_seconds << ",\n"
        << "  \"realtime_factor\": " << summary.realtime_factor << "\n"
        << "}\n";
}

} // namespace hammerlab2
