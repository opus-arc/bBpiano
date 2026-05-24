//
//  MyCSVReader.cpp
//  bBpiano
//
//  Created by opus arc on 2026/5/17.
//

#include "MyCSVReader.hpp"

#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>

namespace {
    std::vector<std::string> splitCsvLine(const std::string& line) {
        std::vector<std::string> cells;
        std::stringstream ss(line);
        std::string cell;
        
        while (std::getline(ss, cell, ',')) {
            cells.push_back(cell);
        }
        
        return cells;
    }
}

std::vector<LossConstant> MyCSVReader::getLossConstant() {
    const std::string tablePath =
        "/Users/opusarc/XCodeProjects/bBpiano/AcousticLab/StringFilterLab/Generated/loss_filter_constants.csv";
    
    std::vector<LossConstant> constants;
    std::ifstream csv(tablePath);
    
    if (!csv.is_open()) {
        std::cerr << "Cannot open loss filter constants table: "
                  << tablePath << std::endl;
        return constants;
    }
    
    std::string line;
    bool isHeader = true;
    
    while (std::getline(csv, line)) {
        if (line.empty())
            continue;
        
        if (isHeader) {
            isHeader = false;
            continue;
        }
        
        const std::vector<std::string> cells = splitCsvLine(line);
        
        // expected:
        // key,f0,g,a1,pointCount,fitError
        if (cells.size() < 4)
            continue;
        
        LossConstant loss;
        
        try {
            loss.midi_n = std::stoi(cells[0]);
            loss.g = std::stod(cells[2]);
            loss.a_1 = std::stod(cells[3]);
        } catch (...) {
            continue;
        }
        
        loss.g = std::clamp(loss.g, 0.0, 1.0);
        loss.a_1 = std::clamp(loss.a_1, -0.98, 0.98);
        
        constants.push_back(loss);
    }
    
    std::sort(constants.begin(), constants.end(), [](const LossConstant& a, const LossConstant& b) {
        return a.midi_n < b.midi_n;
    });
    
    return constants;
}


DispersionConstant MyCSVReader::getDispersionConstantByMidi(int midi_n) {
    
    const std::string tablePath =
        "/Users/opusarc/XCodeProjects/bBpiano/AcousticLab/StringFilterLab/Generated/dispersion_filter_constants.csv";
    
    std::ifstream csv(tablePath);
    
    if (!csv.is_open()) {
        std::cerr << "Cannot open dispersion filter constants table: "
                  << tablePath << std::endl;
        return {};
    }
    
    std::string line;
    bool isHeader = true;
    
    while (std::getline(csv, line)) {
        
        if (line.empty())
            continue;
        
        if (isHeader) {
            isHeader = false;
            continue;
        }
        
        const std::vector<std::string> cells = splitCsvLine(line);
        
        // expected:
        // key,f0,a1,order,usedPartialCount,error
        
        if (cells.size() < 5)
            continue;
        
        try {
            
            const int currentMidi = std::stoi(cells[0]);
            
            if (currentMidi != midi_n)
                continue;
            
            DispersionConstant dispersion;
            
            dispersion.midi_n = currentMidi;
            dispersion.a1 = std::stod(cells[2]);
            dispersion.order = std::stoi(cells[3]);
            
            dispersion.a1 =
                std::clamp(dispersion.a1, -0.999, 0.999);
            
            return dispersion;
            
        } catch (...) {
            continue;
        }
    }
    
    return {};
}
