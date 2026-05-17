//
//  MyPitch.cpp
//  bBpiano
//
//  Created by opus arc on 2026/5/17.
//

#include "MyPitch.hpp"

#define A4_STANDARD 440.0


int MyPitch::nameToMidi(const std::string& noteName) {
    if (noteName.size() < 2) {
        throw std::invalid_argument("Invalid note name: " + noteName);
    }

    const char letterName = noteName[0];
    int accidental = 0;
    size_t octaveIndex = 1;

    // 当前采样命名使用 Cs / Ds / Fs / Gs / As 表示升号音，
    // 同时保留 # / b 兼容性。
    if (noteName.size() >= 3) {
        const char accidentalName = noteName[1];

        if (accidentalName == 's' || accidentalName == '#') {
            accidental = 1;
            octaveIndex = 2;
        } else if (accidentalName == 'b') {
            accidental = -1;
            octaveIndex = 2;
        }
    }

    if (octaveIndex >= noteName.size()) {
        throw std::invalid_argument("Invalid octave in note name: " + noteName);
    }

    const int octaveNumber = std::stoi(noteName.substr(octaveIndex));
    int semitoneInOctave;

    switch (letterName) {
        case 'C':
            semitoneInOctave = 0;
            break;

        case 'D':
            semitoneInOctave = 2;
            break;

        case 'E':
            semitoneInOctave = 4;
            break;

        case 'F':
            semitoneInOctave = 5;
            break;

        case 'G':
            semitoneInOctave = 7;
            break;

        case 'A':
            semitoneInOctave = 9;
            break;

        case 'B':
            semitoneInOctave = 11;
            break;

        default:
            throw std::invalid_argument("Invalid note letter: " + noteName);
    }

    return semitoneInOctave + accidental + octaveNumber * 12 + 12;
}

float MyPitch::midiToFrequency(const int midi) {
    const int semitoneFromA4 = midi - 69;

    return static_cast<float>(A4_STANDARD * std::pow(2.0, semitoneFromA4 / 12.0));
}

float MyPitch::getFrequency(const std::string& noteName) {

    int midi = nameToMidi(noteName);

    return midiToFrequency(midi);
}

std::string MyPitch::findName(const std::string& fileName) {
    size_t pos = fileName.find('_');
    if (pos != std::string::npos)
        return fileName.substr(0, pos);
    return "A4";
}
