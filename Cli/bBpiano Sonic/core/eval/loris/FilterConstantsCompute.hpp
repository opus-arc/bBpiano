//
//  computeDispersion.hpp
//  bBpiano
//
//  Created by opus arc on 2026/6/13.
//

#ifndef computeDispersion_hpp
#define computeDispersion_hpp

#include <array>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>
#include <sys/wait.h>
#include <vector>
#include "../../external/nlohmann/json.hpp"

struct LorisDataBreakpoint {
    double time_seconds = 0.0;
    double frequency_hz = 0.0;
    double amplitude_linear = 0.0;
    double phase_radians = 0.0;
    double bandwidth = 0.0;
};

struct LorisPartial {
    int partialIndex = -1;
    
    int id = -1;

    double mean_frequency_hz = 0.0;
    double frequency_at_max_amplitude_hz = 0.0;
    double start_frequency_hz = 0.0;
    double end_frequency_hz = 0.0;

    double max_amplitude_linear = 0.0;
    double mean_amplitude_linear = 0.0;

    double start_time_seconds = 0.0;
    double end_time_seconds = 0.0;
    double duration_seconds = 0.0;

    int breakpoint_count = 0;
    double mean_bandwidth = 0.0;

    std::vector<LorisDataBreakpoint> breakpoints;
    

};

struct LorisData {
    int exitCode = -1;
    double sample_rate = 0.0;
    int partial_count = 0;
    int exported_partial_count = 0;
    std::vector<LorisPartial> partials;
};

double powerMean12(const std::vector<double>& xs) {
    if (xs.empty()) {
        throw std::invalid_argument("xs is empty");
    }
    constexpr double p = 1.2;
    double sum = 0.0;
    for (double x : xs) {
        sum += std::pow(x, p);
    }
    return std::pow(sum / xs.size(), 1.0 / p);
}

class FilterConstantsCompute {

public:
    LorisData lorisData;
    double f0 = 0.0;
    
    FilterConstantsCompute(double f0,
                           const std::string& wavPath,
                           const std::string& lorisScriptPath = "/Users/opusarc/Projects/XCodeProjects/bBpiano/Cli/bBpiano Sonic/core/loris/loris.py",
                           const std::string& pythonExecutable = "/Users/opusarc/miniforge3/bin/python"
                           ) : f0(f0) {
        lorisData = getLorisData(wavPath, lorisScriptPath, pythonExecutable);

    }
    ~FilterConstantsCompute() = default;
    
    
    
    inline void computeFilterConstants() {
        
    }

    inline double computeB() {
        
        std::vector<std::pair<int, LorisPartial>> realPartials = findRealPartials();
        
        std::vector<double> Bs;
        
        for(int i = 0; i < realPartials.size(); i++) {
            
            double n = realPartials[i].first;
            double fn = realPartials[i].second.mean_frequency_hz;

            
            double ratio = fn / (n * f0);

            double B = (ratio * ratio - 1.0) / (n * n);
            
            Bs.push_back(B);
            
            std::cout << "B" << n << ": " << B << "\n";
        }
        
        
        

        std::cout << powerMean12(Bs) << "\n";
        return powerMean12(Bs);
    }



    inline std::vector<std::pair<int, LorisPartial>> findRealPartials() {
        
        const std::vector<LorisPartial> &partials = lorisData.partials;
        std::vector<std::pair<int, LorisPartial>> realPartials;
        
        double lastFrequency = 0.0;
        int currN = 0;
        double lastDelta = 0.0;
        
        for(auto& p : partials) {
            double f = p.mean_frequency_hz;
            int n = static_cast<int>(std::round(f / f0));
            double ideaF = n * f0;
            double delta = f - ideaF;
            
            double taylorExpansion = n * f0 + n * n * n * 0.1;
            if(f < taylorExpansion)
                continue;
            
            if(f > lastFrequency && f > ideaF && n >= currN) {
                realPartials.push_back({n, p});
                lastFrequency = f;
                currN = n;
                std::cout << "f" << n << ": " << f << "\n";
                std::cout << "delta: " << delta << "\n";
            }
        }
        
        
        return realPartials;
        
    }
    
    
    inline void printTopPartial() {
        for(int i = 0; i < lorisData.exported_partial_count; i++) {
            
            if(lorisData
               .partials[i].duration_seconds < 0.001 || lorisData
               .partials[i].breakpoint_count <= 1) continue;
            
            std::cout << "f" << i << ": " << lorisData
                .partials[i].mean_frequency_hz << ", duration_seconds: " << lorisData
                .partials[i].duration_seconds << ", breakpoint_count: " << lorisData
                .partials[i].breakpoint_count << "\n";
        }
    }

    
    inline std::string shellQuote(const std::string& value) {
        std::string quoted = "'";

        for (char c : value) {
            if (c == '\'') {
                quoted += "'\\''";
            } else {
                quoted += c;
            }
        }

        quoted += "'";
        return quoted;
    }

    inline LorisData getLorisData(
        const std::string& wavPath,
        const std::string& lorisScriptPath = "/Users/opusarc/Projects/XCodeProjects/bBpiano/Cli/bBpiano Sonic/core/loris/loris.py",
        const std::string& pythonExecutable = "/Users/opusarc/miniforge3/bin/python"
    ) {
        const std::string command =
            shellQuote(pythonExecutable)
            + " "
            + shellQuote(lorisScriptPath)
            + " "
            + shellQuote(wavPath);

        FILE* pipe = popen(command.c_str(), "r");

        if (pipe == nullptr) {
            throw std::runtime_error("Failed to launch loris.py");
        }

        std::array<char, 4096> buffer{};
        std::string json;

        while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe) != nullptr) {
            json += buffer.data();
        }

        const int status = pclose(pipe);

        LorisData data;

        if (WIFEXITED(status)) {
            data.exitCode = WEXITSTATUS(status);
        } else {
            data.exitCode = -1;
        }

        if (data.exitCode != 0) {
            throw std::runtime_error("loris.py failed with exit code " + std::to_string(data.exitCode));
        }

        auto root = nlohmann::json::parse(json);

        data.sample_rate = root.value("sample_rate", 0.0);
        data.partial_count = root.value("partial_count", 0);
        data.exported_partial_count = root.value("exported_partial_count", 0);

        for (const auto& partialJson : root["partials"]) {
            LorisPartial partial;

            partial.id = partialJson.value("id", -1);

            const auto& summary = partialJson["summary"];

            partial.mean_frequency_hz = summary.value("mean_frequency_hz", 0.0);
            partial.frequency_at_max_amplitude_hz = summary.value("frequency_at_max_amplitude_hz", 0.0);
            partial.start_frequency_hz = summary.value("start_frequency_hz", 0.0);
            partial.end_frequency_hz = summary.value("end_frequency_hz", 0.0);
            partial.max_amplitude_linear = summary.value("max_amplitude_linear", 0.0);
            partial.mean_amplitude_linear = summary.value("mean_amplitude_linear", 0.0);
            partial.start_time_seconds = summary.value("start_time_seconds", 0.0);
            partial.end_time_seconds = summary.value("end_time_seconds", 0.0);
            partial.duration_seconds = summary.value("duration_seconds", 0.0);
            partial.breakpoint_count = summary.value("breakpoint_count", 0);
            partial.mean_bandwidth = summary.value("mean_bandwidth", 0.0);

            if (partialJson.contains("breakpoints")) {
                for (const auto& bp : partialJson["breakpoints"]) {
                    if (bp.size() < 5) continue;

                    partial.breakpoints.push_back({
                        bp[0].get<double>(),
                        bp[1].get<double>(),
                        bp[2].get<double>(),
                        bp[3].get<double>(),
                        bp[4].get<double>()
                    });
                }
            }

            data.partials.push_back(std::move(partial));
        }

        return data;
    }

    
};












#endif /* computeDispersion_hpp */
