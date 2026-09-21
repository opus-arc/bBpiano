// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
// [AI-ASSISTED, HUMAN-UNDERSTOOD CODE]
// This file may have been written with the assistance of AI for explanation,
// discussion, review, implementation guidance, or non-critical engineering suggestions.
//
// However, every line included in this file has been reviewed, understood,
// and accepted by its author. Every implementation is expected to be explainable,
// reproducible, open to inspection, and subject to criticism and revision.
//
// AI is treated as an engineering assistant rather than an authority:
// the author remains responsible for the design decisions, assumptions,
// correctness, and final implementation contained in this file.
// —————————————————————————
// [本文件包含 AI 辅助下完成的代码]
// 本文件在编写过程中可能使用 AI 进行原理讲解、讨论、代码审阅、实现指导，
// 或提供非关键性的工程建议。
//
// 但最终保留在本文件中的每一行代码，均由作者亲自审阅、理解并确认。
// 所有实现都应能够由作者解释、复现、检查，并接受批评、修改与质疑。
//
// AI 在此仅作为工程辅助工具，而非技术权威；
// 本文件中的设计选择、假设、正确性以及最终实现，均由作者本人承担责任。
//
// Ziyang Tan
// 2026-06-19
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
// [AI-ASSISTED, HUMAN-UNDERSTOOD CODE]
// This file may have been written with the assistance of AI for explanation,
// discussion, review, implementation guidance, or non-critical engineering suggestions.
//
// However, every line included in this file has been reviewed, understood,
// and accepted by its author. Every implementation is expected to be explainable,
// reproducible, open to inspection, and subject to criticism and revision.
//
// AI is treated as an engineering assistant rather than an authority:
// the author remains responsible for the design decisions, assumptions,
// correctness, and final implementation contained in this file.
// —————————————————————————
// [本文件包含 AI 辅助下完成的代码]
// 本文件在编写过程中可能使用 AI 进行原理讲解、讨论、代码审阅、实现指导，
// 或提供非关键性的工程建议。
//
// 但最终保留在本文件中的每一行代码，均由作者亲自审阅、理解并确认。
// 所有实现都应能够由作者解释、复现、检查，并接受批评、修改与质疑。
//
// AI 在此仅作为工程辅助工具，而非技术权威；
// 本文件中的设计选择、假设、正确性以及最终实现，均由作者本人承担责任。
//
// Ziyang Tan
// 2026-06-19
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

#ifndef Precompute_f0_hpp
#define Precompute_f0_hpp

#include <algorithm>
#include <array>
#include <cmath>


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

// TODO: 这是什么
static_assert(kKeyCount == 88, "tuning preset table must cover 88 keys");

constexpr int kA4Midi = 69;
constexpr double kA4Frequency = 440.0;


using FrequencyTable = std::array<double, kKeyCount>;
using UnisonOffsetTable = std::array<double, kKeyCount>;
using StringImpedanceTable = std::array<double, kKeyCount>;

enum class StringIndex {
    left = 1,
    center = 2,
    right = 3
};


constexpr double kPreferredUnisonSpreadCents = 1.7;

}  // namespace Parameters::Tuning



class TunningPresets {
    TunningPresets(const TunningPresets&) = delete;
    TunningPresets& operator=(const TunningPresets&) = delete;
public:
    using Temperament = Parameters::Tuning::Temperament;
    using FrequencyTable = Parameters::Tuning::FrequencyTable;
    using StringIndex = Parameters::Tuning::StringIndex;
    using UnisonOffsetTable = Parameters::Tuning::UnisonOffsetTable;
    using StringImpedanceTable = Parameters::Tuning::StringImpedanceTable;


    constexpr TunningPresets() noexcept = default;


    double get_frequency(
        int midi_n,
        Temperament temperament = Temperament::equal,
        StringIndex stringIndex = StringIndex::center
    ) const noexcept
    {
        if (midi_n < Parameters::Tuning::kMidiMin ||
            midi_n > Parameters::Tuning::kMidiMax) {
            return 0.0;
        }

        const int index = midi_n - Parameters::Tuning::kMidiMin;

        const Tables& tables = sharedTables();

        const double baseFrequency =
            getBaseFrequencyByIndex(
                tables,
                index,
                temperament
            );

        switch (stringIndex) {
            case StringIndex::left:
                return baseFrequency - tables.unisonOffsetHz[index];

            case StringIndex::center:
                return baseFrequency;

            case StringIndex::right:
                return baseFrequency + tables.unisonOffsetHz[index];
        }

        return baseFrequency;
    }


    const FrequencyTable& getEqualTemperamentTable() const noexcept
    {
        return sharedTables().equalTemperament;
    }


    double get_characteristic_impedance(int midi_n) const noexcept
    {
        if (midi_n < Parameters::Tuning::kMidiMin ||
            midi_n > Parameters::Tuning::kMidiMax) {
            return 0.0;
        }

        const int index = midi_n - Parameters::Tuning::kMidiMin;
        return sharedTables().characteristicImpedance[index];
    }


    const StringImpedanceTable&
    getCharacteristicImpedanceTable() const noexcept
    {
        return sharedTables().characteristicImpedance;
    }


private:

    struct Tables {
        FrequencyTable equalTemperament{};

        FrequencyTable pythagoreTemperament{};
        FrequencyTable zarlinoTemperament{};
        FrequencyTable meantoneTemperament{};
        FrequencyTable wellTemperament{};
        FrequencyTable werckmeisterTemperament{};

        UnisonOffsetTable unisonOffsetHz{};
        StringImpedanceTable characteristicImpedance{};
    };


    static Tables makeTables()
    {
        Tables tables{};

        computeEqualTemperament(tables.equalTemperament);

        // For now, keep the other temperament tables initialized to ET.
        // They will be replaced by their own formulas later.
        tables.pythagoreTemperament = tables.equalTemperament;
        tables.zarlinoTemperament = tables.equalTemperament;
        tables.meantoneTemperament = tables.equalTemperament;
        tables.wellTemperament = tables.equalTemperament;
        tables.werckmeisterTemperament = tables.equalTemperament;

        computeUnisonOffsets(
            tables.equalTemperament,
            tables.unisonOffsetHz
        );

        return tables;
    }


    static const Tables& sharedTables()
    {
        static const Tables tables = makeTables();
        return tables;
    }


    static double strictEqualFrequency(int midi_n)
    {
        return Parameters::Tuning::kA4Frequency *
            std::pow(
                2.0,
                static_cast<double>(
                    midi_n - Parameters::Tuning::kA4Midi
                ) / 12.0
            );
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
            + c3 * std::pow(
                std::max(x - k1, 0.0),
                2.0
            )
            + c4 * std::pow(
                std::max(x - k2, 0.0),
                2.0
            );

        log_b = std::clamp(
            log_b,
            std::log(5e-6),
            std::log(2e-2)
        );

        return std::exp(log_b);
    }


    static double getInharmonicityBFromStrictEqual(int midi_n)
    {
        return computeFittedB(
            strictEqualFrequency(midi_n)
        );
    }


    static double octaveTypeRhoForMidi(int lowerMidi)
    {
        // Rigaud-David-Daudet style octave-type model:
        // rho tends toward high octave types in the bass
        // and toward 2:1 in the treble.

        constexpr double kappa = 3.5;
        constexpr double m0 = 60.0;
        constexpr double alpha = 25.0;

        const double x =
            (static_cast<double>(lowerMidi) - m0) /
            alpha;

        return
            (kappa * 0.5) *
            (1.0 - std::erf(x)) +
            1.0;
    }


    static double tuneUpperOctaveFrequency(
        int lowerMidi,
        int upperMidi,
        double lowerF0
    )
    {
        const double rho =
            octaveTypeRhoForMidi(lowerMidi);

        const double rho2 = rho * rho;

        const double lowerB =
            getInharmonicityBFromStrictEqual(lowerMidi);

        const double upperB =
            getInharmonicityBFromStrictEqual(upperMidi);

        return
            2.0 *
            lowerF0 *
            std::sqrt(
                (1.0 + lowerB * 4.0 * rho2) /
                (1.0 + upperB * rho2)
            );
    }


    static double tuneLowerOctaveFrequency(
        int lowerMidi,
        int upperMidi,
        double upperF0
    )
    {
        const double rho =
            octaveTypeRhoForMidi(lowerMidi);

        const double rho2 = rho * rho;

        const double lowerB =
            getInharmonicityBFromStrictEqual(lowerMidi);

        const double upperB =
            getInharmonicityBFromStrictEqual(upperMidi);

        const double octaveStretchRatio =
            std::sqrt(
                (1.0 + lowerB * 4.0 * rho2) /
                (1.0 + upperB * rho2)
            );

        return
            upperF0 /
            (2.0 * octaveStretchRatio);
    }


    static double interpolateAOctaveDeviationCents(
        int midi_n,
        const std::array<int, 8>& aMidiNotes,
        const std::array<double, 8>& aDeviationCents
    )
    {
        if (midi_n <= aMidiNotes.front()) {
            return aDeviationCents.front();
        }

        for (
            std::size_t i = 0;
            i + 1 < aMidiNotes.size();
            ++i
        ) {
            const int leftMidi =
                aMidiNotes[i];

            const int rightMidi =
                aMidiNotes[i + 1];

            if (midi_n <= rightMidi) {
                const double t =
                    static_cast<double>(
                        midi_n - leftMidi
                    ) /
                    static_cast<double>(
                        rightMidi - leftMidi
                    );

                return
                    aDeviationCents[i] +
                    t *
                    (
                        aDeviationCents[i + 1] -
                        aDeviationCents[i]
                    );
            }
        }

        const std::size_t last =
            aMidiNotes.size() - 1;

        const std::size_t prev =
            last - 1;

        const double slope =
            (
                aDeviationCents[last] -
                aDeviationCents[prev]
            ) /
            static_cast<double>(
                aMidiNotes[last] -
                aMidiNotes[prev]
            );

        return
            aDeviationCents[last] +
            slope *
            static_cast<double>(
                midi_n -
                aMidiNotes[last]
            );
    }


    static void computeEqualTemperament(
        FrequencyTable& equalTemperament
    )
    {
        // This is not strict mathematical ET.
        // It is an equal-temperament layout corrected by piano octave stretching.
        //
        // A4 is kept at 440 Hz.
        // The A-octaves are tuned by matching octave-related
        // inharmonic partials, then the deviation from strict ET
        // is interpolated across the full keyboard.

        FrequencyTable strictEqual{};

        for (
            int midi = Parameters::Tuning::kMidiMin;
            midi <= Parameters::Tuning::kMidiMax;
            ++midi
        ) {
            const int index =
                midi -
                Parameters::Tuning::kMidiMin;

            strictEqual[index] =
                strictEqualFrequency(midi);
        }


        constexpr std::array<int, 8> aMidiNotes = {
            21,
            33,
            45,
            57,
            69,
            81,
            93,
            105
        };


        std::array<
            double,
            aMidiNotes.size()
        > tunedAFrequencies{};


        std::array<
            double,
            aMidiNotes.size()
        > tunedADeviationsCents{};


        constexpr int a4AnchorIndex = 4;

        tunedAFrequencies[a4AnchorIndex] =
            Parameters::Tuning::kA4Frequency;


        for (
            int i = a4AnchorIndex + 1;
            i < static_cast<int>(aMidiNotes.size());
            ++i
        ) {
            tunedAFrequencies[i] =
                tuneUpperOctaveFrequency(
                    aMidiNotes[i - 1],
                    aMidiNotes[i],
                    tunedAFrequencies[i - 1]
                );
        }


        for (
            int i = a4AnchorIndex - 1;
            i >= 0;
            --i
        ) {
            tunedAFrequencies[i] =
                tuneLowerOctaveFrequency(
                    aMidiNotes[i],
                    aMidiNotes[i + 1],
                    tunedAFrequencies[i + 1]
                );
        }


        for (
            std::size_t i = 0;
            i < aMidiNotes.size();
            ++i
        ) {
            tunedADeviationsCents[i] =
                1200.0 *
                std::log2(
                    tunedAFrequencies[i] /
                    strictEqualFrequency(
                        aMidiNotes[i]
                    )
                );
        }


        for (
            int midi = Parameters::Tuning::kMidiMin;
            midi <= Parameters::Tuning::kMidiMax;
            ++midi
        ) {
            const int index =
                midi -
                Parameters::Tuning::kMidiMin;

            const double deviationCents =
                interpolateAOctaveDeviationCents(
                    midi,
                    aMidiNotes,
                    tunedADeviationsCents
                );

            equalTemperament[index] =
                strictEqual[index] *
                std::pow(
                    2.0,
                    deviationCents / 1200.0
                );
        }
    }


    static double getBaseFrequencyByIndex(
        const Tables& tables,
        int index,
        Temperament temperament
    ) noexcept
    {
        switch (temperament) {
            case Temperament::equal:
                return
                    tables.equalTemperament[index];

            case Temperament::pythagore:
                return
                    tables.pythagoreTemperament[index];

            case Temperament::zarlino:
                return
                    tables.zarlinoTemperament[index];

            case Temperament::meantone:
                return
                    tables.meantoneTemperament[index];

            case Temperament::well:
                return
                    tables.wellTemperament[index];

            case Temperament::werckmeister:
                return
                    tables.werckmeisterTemperament[index];
        }

        return
            tables.equalTemperament[index];
    }


    static int stringCountForMidi(
        int midi_n
    ) noexcept
    {
        // Steinway D-274:
        //
        // MIDI 21-28 : single string
        // MIDI 29-33 : bichord
        // MIDI 34-108: trichord

        if (midi_n <= 28) {
            return 1;
        }

        if (midi_n <= 33) {
            return 2;
        }

        return 3;
    }


    static double computeUnisonHalfOffsetHz(
        double f0,
        int string_count
    )
    {
        if (string_count <= 1) {
            return 0.0;
        }

        const double spreadRatio =
            std::pow(
                2.0,
                Parameters::Tuning::kPreferredUnisonSpreadCents /
                1200.0
            );

        const double rightFrequency =
            f0 * spreadRatio;

        const double spreadHz =
            rightFrequency - f0;

        return spreadHz * 0.5;
    }


    static void computeUnisonOffsets(
        const FrequencyTable& equalTemperament,
        UnisonOffsetTable& unisonOffsetHz
    )
    {
        for (
            int midi = Parameters::Tuning::kMidiMin;
            midi <= Parameters::Tuning::kMidiMax;
            ++midi
        ) {
            const int index =
                midi -
                Parameters::Tuning::kMidiMin;

            const int string_count =
                stringCountForMidi(midi);

            const double f0 =
                equalTemperament[index];

            unisonOffsetHz[index] =
                computeUnisonHalfOffsetHz(
                    f0,
                    string_count
                );
        }
    }

};


#endif /* Precompute_f0_hpp */

