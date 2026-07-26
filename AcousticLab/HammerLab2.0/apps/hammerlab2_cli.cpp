#include "hammerlab2/Experiment.hpp"

#include <cstdlib>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>

namespace {

struct Options {
    hammerlab2::ExperimentConfig config{};
    std::filesystem::path trace_path;
    std::filesystem::path summary_path;
};

[[noreturn]] void usage(const char* program, int status) {
    std::ostream& stream = status == 0 ? std::cout : std::cerr;
    stream
        << "Usage: " << program << " simulate [options]\n\n"
        << "  --case-id ID\n"
        << "  --model power|stulov|prony2\n"
        << "  --load rigid|resistive-port\n"
        << "  --sample-rate HZ       --oversample N\n"
        << "  --duration SEC         --vin M_PER_SEC\n"
        << "  --mass KG              --impedance N_SEC_PER_M\n"
        << "  --stiffness-n-mm-p K   --exponent P\n"
        << "  --epsilon1 VALUE       --tau1 SEC\n"
        << "  --epsilon2 VALUE       --tau2 SEC\n"
        << "  --iterations N         --tolerance N\n"
        << "  --trace PATH           --summary PATH\n\n"
        << "Defaults are engineering seed values, not fitted instrument data.\n";
    std::exit(status);
}

std::string next_value(int& index, int argc, char** argv) {
    if (++index >= argc) throw std::invalid_argument("missing option value");
    return argv[index];
}

Options parse_options(int argc, char** argv) {
    if (argc < 2 || std::string(argv[1]) != "simulate") usage(argv[0], argc < 2 ? 1 : 0);

    Options options;
    auto& config = options.config;
    config.felt.branches[0] = {0.51, 4.0e-4};
    config.felt.branches[1] = {0.20, 8.0e-5};

    for (int index = 2; index < argc; ++index) {
        const std::string argument = argv[index];
        if (argument == "--help" || argument == "-h") usage(argv[0], 0);
        if (argument == "--case-id") config.case_id = next_value(index, argc, argv);
        else if (argument == "--model") config.model = hammerlab2::parse_model_kind(next_value(index, argc, argv));
        else if (argument == "--load") config.load = hammerlab2::parse_load_kind(next_value(index, argc, argv));
        else if (argument == "--sample-rate") config.sample_rate_hz = std::stod(next_value(index, argc, argv));
        else if (argument == "--oversample") config.oversample = std::stoi(next_value(index, argc, argv));
        else if (argument == "--duration") config.duration_seconds = std::stod(next_value(index, argc, argv));
        else if (argument == "--vin") config.impact_velocity_mps = std::stod(next_value(index, argc, argv));
        else if (argument == "--mass") config.hammer_mass_kg = std::stod(next_value(index, argc, argv));
        else if (argument == "--impedance") config.port_impedance_ns_per_m = std::stod(next_value(index, argc, argv));
        else if (argument == "--stiffness-n-mm-p") config.felt.stiffness_n_per_mm_p = std::stod(next_value(index, argc, argv));
        else if (argument == "--exponent") config.felt.exponent = std::stod(next_value(index, argc, argv));
        else if (argument == "--epsilon1") config.felt.branches[0].amplitude = std::stod(next_value(index, argc, argv));
        else if (argument == "--tau1") config.felt.branches[0].tau_seconds = std::stod(next_value(index, argc, argv));
        else if (argument == "--epsilon2") config.felt.branches[1].amplitude = std::stod(next_value(index, argc, argv));
        else if (argument == "--tau2") config.felt.branches[1].tau_seconds = std::stod(next_value(index, argc, argv));
        else if (argument == "--iterations") config.implicit_iterations = std::stoi(next_value(index, argc, argv));
        else if (argument == "--tolerance") config.solver_tolerance_n = std::stod(next_value(index, argc, argv));
        else if (argument == "--trace") options.trace_path = next_value(index, argc, argv);
        else if (argument == "--summary") options.summary_path = next_value(index, argc, argv);
        else throw std::invalid_argument("unknown option: " + argument);
    }

    if (config.model == hammerlab2::ModelKind::power_law) config.felt.branch_count = 0;
    else if (config.model == hammerlab2::ModelKind::single_stulov) config.felt.branch_count = 1;
    else config.felt.branch_count = 2;
    return options;
}

} // namespace

int main(int argc, char** argv) {
    try {
        const Options options = parse_options(argc, argv);
        const auto result = hammerlab2::run_experiment(options.config);
        if (!options.trace_path.empty()) hammerlab2::write_trace_csv(options.trace_path, result);
        if (!options.summary_path.empty()) {
            hammerlab2::write_summary_json(options.summary_path, options.config, result.summary);
        }

        const auto& summary = result.summary;
        std::cout << std::setprecision(8)
            << "case=" << summary.case_id
            << " model=" << summary.model_id
            << " load=" << summary.load_id
            << " peak_force_n=" << summary.peak_force_n
            << " contact_ms=" << summary.first_contact_ms
            << " rebound_mps=" << summary.rebound_velocity_mps
            << " realtime_factor=" << summary.realtime_factor
            << " converged=" << (summary.solver_converged ? "true" : "false")
            << '\n';
        return summary.finite ? 0 : 2;
    } catch (const std::exception& error) {
        std::cerr << "hammerlab2: " << error.what() << '\n';
        return 1;
    }
}
