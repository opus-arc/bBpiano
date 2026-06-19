//
//  TunningPresets.hpp
//  bBpiano
//
//  Created by opus arc on 2026/6/19.
//

#ifndef Precompute_f0_hpp
#define Precompute_f0_hpp

#include <iostream>
#include <array>
#include <cmath>
#include <algorithm>


namespace Parameters::Tuning {

enum class Temperament {
    equal,
    pythagore,
    zarlino,
    meantone,
    well,
    werckmeister
};


constexpr int kMidiMin = 21;   // A0
constexpr int kMidiMax = 108;  // C8
constexpr int kKeyCount = kMidiMax - kMidiMin + 1;
constexpr int kA4Midi = 69;
constexpr double kA4Frequency = 440.0;

using FrequencyTable = std::array<double, kKeyCount>;

using UnisonOffsetTable = std::array<double, kKeyCount>;

enum class StringIndex {
    left = 1,
    center = 2,
    right = 3
};

constexpr double kPreferredUnisonSpreadCents = 1.7;

}



class TunningPresets {
public:
    using Temperament = Parameters::Tuning::Temperament;
    using FrequencyTable = Parameters::Tuning::FrequencyTable;
    using StringIndex = Parameters::Tuning::StringIndex;
    using UnisonOffsetTable = Parameters::Tuning::UnisonOffsetTable;

    explicit TunningPresets()
    {
        initialize();
    }

    double getFrequency(int midi_n,
                        Temperament temperament = Temperament::equal,
                        StringIndex stringIndex = StringIndex::center) const
    {
        if (midi_n < Parameters::Tuning::kMidiMin ||
            midi_n > Parameters::Tuning::kMidiMax) {
            return 0.0;
        }

        const int index = midi_n - Parameters::Tuning::kMidiMin;
        const double baseFrequency = getBaseFrequencyByIndex(index, temperament);

        switch (stringIndex) {
            case StringIndex::left:
                return baseFrequency - unisonOffsetHz_[index];

            case StringIndex::center:
                return baseFrequency;

            case StringIndex::right:
                return baseFrequency + unisonOffsetHz_[index];
        }

        return baseFrequency;
    }

    const FrequencyTable& getEqualTemperamentTable() const
    {
        return equalTemperament_;
    }


private:
    FrequencyTable equalTemperament_{};
    FrequencyTable pythagoreTemperament_{};
    FrequencyTable zarlinoTemperament_{};
    FrequencyTable meantoneTemperament_{};
    FrequencyTable wellTemperament_{};
    FrequencyTable werckmeisterTemperament_{};

    UnisonOffsetTable unisonOffsetHz_{};

    void initialize()
    {
        computeEqualTemperament();

        // For now, keep the other temperament tables initialized to ET.
        // They will be replaced by their own formulas later.
        pythagoreTemperament_ = equalTemperament_;
        zarlinoTemperament_ = equalTemperament_;
        meantoneTemperament_ = equalTemperament_;
        wellTemperament_ = equalTemperament_;
        werckmeisterTemperament_ = equalTemperament_;

        computeUnisonOffsets();
    }


    static double strictEqualFrequency(int midi_n)
    {
        return Parameters::Tuning::kA4Frequency *
            std::pow(2.0,
                     static_cast<double>(midi_n - Parameters::Tuning::kA4Midi) / 12.0);
    }

    static double computeFittedB(double f0_hz)
    {
        constexpr double k1 = -1.3333333333333333;
        constexpr double k2 =  1.3333333333333333;

        constexpr double c0 = -6.5760753629956712;
        constexpr double c1 =  1.9137732290077178;
        constexpr double c2 =  0.27614763151078042;
        constexpr double c3 = -0.28572738557770405;
        constexpr double c4 = -0.30709648310494486;

        const double x = std::log2(f0_hz / 440.0);

        double log_b =
              c0
            + c1 * x
            + c2 * x * x
            + c3 * std::pow(std::max(x - k1, 0.0), 2.0)
            + c4 * std::pow(std::max(x - k2, 0.0), 2.0);

        log_b = std::clamp(log_b,
                           std::log(5e-6),
                           std::log(2e-2));

        return std::exp(log_b);
    }

    static double getInharmonicityBFromStrictEqual(int midi_n)
    {
        return computeFittedB(strictEqualFrequency(midi_n));
    }

    static double octaveTypeRhoForMidi(int lowerMidi)
    {
        // Rigaud-David-Daudet style octave-type model:
        // rho tends toward high octave types in the bass and toward 2:1 in the treble.
        constexpr double kappa = 3.5;
        constexpr double m0 = 60.0;
        constexpr double alpha = 25.0;

        const double x = (static_cast<double>(lowerMidi) - m0) / alpha;
        return (kappa * 0.5) * (1.0 - std::erf(x)) + 1.0;
    }

    static double tuneUpperOctaveFrequency(int lowerMidi,
                                           int upperMidi,
                                           double lowerF0)
    {
        const double rho = octaveTypeRhoForMidi(lowerMidi);
        const double rho2 = rho * rho;
        const double lowerB = getInharmonicityBFromStrictEqual(lowerMidi);
        const double upperB = getInharmonicityBFromStrictEqual(upperMidi);

        return 2.0 * lowerF0 *
            std::sqrt((1.0 + lowerB * 4.0 * rho2) /
                      (1.0 + upperB * rho2));
    }

    static double tuneLowerOctaveFrequency(int lowerMidi,
                                           int upperMidi,
                                           double upperF0)
    {
        const double rho = octaveTypeRhoForMidi(lowerMidi);
        const double rho2 = rho * rho;
        const double lowerB = getInharmonicityBFromStrictEqual(lowerMidi);
        const double upperB = getInharmonicityBFromStrictEqual(upperMidi);

        const double octaveStretchRatio =
            std::sqrt((1.0 + lowerB * 4.0 * rho2) /
                      (1.0 + upperB * rho2));

        return upperF0 / (2.0 * octaveStretchRatio);
    }

    static double interpolateAOctaveDeviationCents(
        int midi_n,
        const std::array<int, 8>& aMidiNotes,
        const std::array<double, 8>& aDeviationCents)
    {
        if (midi_n <= aMidiNotes.front()) {
            return aDeviationCents.front();
        }

        for (std::size_t i = 0; i + 1 < aMidiNotes.size(); ++i) {
            const int leftMidi = aMidiNotes[i];
            const int rightMidi = aMidiNotes[i + 1];

            if (midi_n <= rightMidi) {
                const double t = static_cast<double>(midi_n - leftMidi) /
                                 static_cast<double>(rightMidi - leftMidi);
                return aDeviationCents[i] +
                    t * (aDeviationCents[i + 1] - aDeviationCents[i]);
            }
        }

        const std::size_t last = aMidiNotes.size() - 1;
        const std::size_t prev = last - 1;
        const double slope = (aDeviationCents[last] - aDeviationCents[prev]) /
                             static_cast<double>(aMidiNotes[last] - aMidiNotes[prev]);

        return aDeviationCents[last] +
            slope * static_cast<double>(midi_n - aMidiNotes[last]);
    }

    void computeEqualTemperament()
    {
        // This is not strict mathematical ET.
        // It is an equal-temperament layout corrected by piano octave stretching.
        // A4 is kept at 440 Hz. The A-octaves are tuned by matching octave-related
        // inharmonic partials, then the deviation from strict ET is interpolated
        // across the full keyboard.
        FrequencyTable strictEqual{};
        for (int midi = Parameters::Tuning::kMidiMin;
             midi <= Parameters::Tuning::kMidiMax;
             ++midi) {
            const int index = midi - Parameters::Tuning::kMidiMin;
            strictEqual[index] = strictEqualFrequency(midi);
        }

        constexpr std::array<int, 8> aMidiNotes = {21, 33, 45, 57, 69, 81, 93, 105};
        std::array<double, aMidiNotes.size()> tunedAFrequencies{};
        std::array<double, aMidiNotes.size()> tunedADeviationsCents{};

        constexpr int a4AnchorIndex = 4;
        tunedAFrequencies[a4AnchorIndex] = Parameters::Tuning::kA4Frequency;

        for (int i = a4AnchorIndex + 1; i < static_cast<int>(aMidiNotes.size()); ++i) {
            tunedAFrequencies[i] = tuneUpperOctaveFrequency(
                aMidiNotes[i - 1],
                aMidiNotes[i],
                tunedAFrequencies[i - 1]
            );
        }

        for (int i = a4AnchorIndex - 1; i >= 0; --i) {
            tunedAFrequencies[i] = tuneLowerOctaveFrequency(
                aMidiNotes[i],
                aMidiNotes[i + 1],
                tunedAFrequencies[i + 1]
            );
        }

        for (std::size_t i = 0; i < aMidiNotes.size(); ++i) {
            tunedADeviationsCents[i] = 1200.0 * std::log2(
                tunedAFrequencies[i] / strictEqualFrequency(aMidiNotes[i])
            );
        }

        for (int midi = Parameters::Tuning::kMidiMin;
             midi <= Parameters::Tuning::kMidiMax;
             ++midi) {
            const int index = midi - Parameters::Tuning::kMidiMin;
            const double deviationCents = interpolateAOctaveDeviationCents(
                midi,
                aMidiNotes,
                tunedADeviationsCents
            );

            equalTemperament_[index] = strictEqual[index] *
                std::pow(2.0, deviationCents / 1200.0);
        }
    }

    double getBaseFrequencyByIndex(int index, Temperament temperament) const
    {
        switch (temperament) {
            case Temperament::equal:
                return equalTemperament_[index];

            case Temperament::pythagore:
                return pythagoreTemperament_[index];

            case Temperament::zarlino:
                return zarlinoTemperament_[index];

            case Temperament::meantone:
                return meantoneTemperament_[index];

            case Temperament::well:
                return wellTemperament_[index];

            case Temperament::werckmeister:
                return werckmeisterTemperament_[index];
        }

        return equalTemperament_[index];
    }

    static int stringCountForMidi(int midi_n)
    {
        if (midi_n <= 30) {
            return 1;
        }

        if (midi_n <= 37) {
            return 2;
        }

        return 3;
    }

    static double computeUnisonHalfOffsetHz(double f0, int string_count)
    {
        if (string_count <= 1) {
            return 0.0;
        }

        const double spreadRatio =
            std::pow(2.0, Parameters::Tuning::kPreferredUnisonSpreadCents / 1200.0);
        const double rightFrequency = f0 * spreadRatio;
        const double spreadHz = rightFrequency - f0;

        return spreadHz * 0.5;
    }

    void computeUnisonOffsets()
    {
        for (int midi = Parameters::Tuning::kMidiMin;
             midi <= Parameters::Tuning::kMidiMax;
             ++midi) {
            const int index = midi - Parameters::Tuning::kMidiMin;
            const int string_count = stringCountForMidi(midi);
            const double f0 = equalTemperament_[index];
            unisonOffsetHz_[index] = computeUnisonHalfOffsetHz(f0, string_count);
        }
    }
};

#endif /* Precompute_f0_hpp */
