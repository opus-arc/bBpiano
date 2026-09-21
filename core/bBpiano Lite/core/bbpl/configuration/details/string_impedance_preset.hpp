#ifndef string_impedance_preset_hpp
#define string_impedance_preset_hpp

#include <array>
#include <cstddef>

namespace Parameters::String::RT425Wrapped {

struct StringImpedancePreset {
    double characteristic_impedance_kg_s;
};


// Characteristic transverse-wave impedance:
//
//      A  = pi * d^2 / 4
//      mu = rho * A
//      Z0 = sqrt(T0 * mu)
//
// Unit:
//
//      kg / s
//      == N / (m / s)
//
// Source:
// Chabassier & Duruflé,
// "Physical parameters for piano modeling",
// INRIA Technical Report RT-425, Appendix A,
// wrapped-string piano.
//
// MIDI 24-107:
// directly derived from the RT-425 values of
// diameter d, density rho and tension T0.
//
// MIDI 21-23 and MIDI 108:
// RT-425 Appendix A does not provide these four notes.
// They are boundary extrapolations from neighboring
// RT-425 characteristic impedances and are marked below.

inline constexpr std::array<
    StringImpedancePreset,
    88
> kStringImpedancePresets{{

    // RT-425 boundary extrapolation
    StringImpedancePreset{12.3988206919},  // MIDI 21 A0
    StringImpedancePreset{12.8891815684},  // MIDI 22 A#0
    StringImpedancePreset{13.1332105294},  // MIDI 23 B0

    // RT-425 Appendix A
    StringImpedancePreset{13.0839327734},  // MIDI 24 C1
    StringImpedancePreset{12.8890542964},  // MIDI 25
    StringImpedancePreset{12.3334372967},  // MIDI 26
    StringImpedancePreset{11.5713724036},  // MIDI 27
    StringImpedancePreset{10.6439459031},  // MIDI 28
    StringImpedancePreset{9.67353066531},  // MIDI 29
    StringImpedancePreset{8.69475953779},  // MIDI 30
    StringImpedancePreset{7.75861813737},  // MIDI 31
    StringImpedancePreset{6.90352041365},  // MIDI 32
    StringImpedancePreset{6.12894607314},  // MIDI 33
    StringImpedancePreset{5.42385158532},  // MIDI 34
    StringImpedancePreset{4.83877578739},  // MIDI 35
    StringImpedancePreset{4.35742291689},  // MIDI 36
    StringImpedancePreset{3.96601763469},  // MIDI 37
    StringImpedancePreset{3.66669615581},  // MIDI 38
    StringImpedancePreset{3.44305916601},  // MIDI 39
    StringImpedancePreset{3.25519545620},  // MIDI 40

    // transition from wrapped equivalent strings
    // toward ordinary steel strings
    StringImpedancePreset{2.48500589816},  // MIDI 41
    StringImpedancePreset{2.42870645282},  // MIDI 42
    StringImpedancePreset{2.36987715335},  // MIDI 43
    StringImpedancePreset{2.36555046436},  // MIDI 44
    StringImpedancePreset{2.30916471578},  // MIDI 45
    StringImpedancePreset{2.30054038444},  // MIDI 46
    StringImpedancePreset{2.29927159747},  // MIDI 47
    StringImpedancePreset{2.29950268773},  // MIDI 48
    StringImpedancePreset{2.29970769619},  // MIDI 49
    StringImpedancePreset{2.29838304084},  // MIDI 50
    StringImpedancePreset{2.29704308482},  // MIDI 51
    StringImpedancePreset{2.28984360988},  // MIDI 52
    StringImpedancePreset{2.27965770670},  // MIDI 53
    StringImpedancePreset{2.26948218038},  // MIDI 54
    StringImpedancePreset{2.25350012030},  // MIDI 55
    StringImpedancePreset{2.23460609259},  // MIDI 56
    StringImpedancePreset{2.21576495128},  // MIDI 57
    StringImpedancePreset{2.19482505542},  // MIDI 58
    StringImpedancePreset{2.17249421243},  // MIDI 59
    StringImpedancePreset{2.15023958991},  // MIDI 60 C4
    StringImpedancePreset{2.12661526438},  // MIDI 61
    StringImpedancePreset{2.10239760722},  // MIDI 62
    StringImpedancePreset{2.08038575212},  // MIDI 63
    StringImpedancePreset{2.05845122567},  // MIDI 64
    StringImpedancePreset{2.03659430135},  // MIDI 65
    StringImpedancePreset{2.01832734084},  // MIDI 66
    StringImpedancePreset{2.00011508361},  // MIDI 67
    StringImpedancePreset{1.98336683563},  // MIDI 68
    StringImpedancePreset{1.96874177635},  // MIDI 69 A4
    StringImpedancePreset{1.95347555206},  // MIDI 70
    StringImpedancePreset{1.94238693842},  // MIDI 71
    StringImpedancePreset{1.93063556045},  // MIDI 72
    StringImpedancePreset{1.92096807626},  // MIDI 73
    StringImpedancePreset{1.91131151156},  // MIDI 74
    StringImpedancePreset{1.90510595470},  // MIDI 75
    StringImpedancePreset{1.89683187889},  // MIDI 76
    StringImpedancePreset{1.89062632204},  // MIDI 77
    StringImpedancePreset{1.88370791961},  // MIDI 78
    StringImpedancePreset{1.87749789350},  // MIDI 79
    StringImpedancePreset{1.87263363374},  // MIDI 80
    StringImpedancePreset{1.86775947868},  // MIDI 81
    StringImpedancePreset{1.86154052370},  // MIDI 82
    StringImpedancePreset{1.85324858372},  // MIDI 83
    StringImpedancePreset{1.84702962875},  // MIDI 84
    StringImpedancePreset{1.83873768877},  // MIDI 85
    StringImpedancePreset{1.83044574880},  // MIDI 86
    StringImpedancePreset{1.82084619918},  // MIDI 87
    StringImpedancePreset{1.81125761501},  // MIDI 88
    StringImpedancePreset{1.79831447070},  // MIDI 89
    StringImpedancePreset{1.78746145235},  // MIDI 90
    StringImpedancePreset{1.77250647871},  // MIDI 91
    StringImpedancePreset{1.75836059104},  // MIDI 92
    StringImpedancePreset{1.74425126497},  // MIDI 93
    StringImpedancePreset{1.72685989245},  // MIDI 94
    StringImpedancePreset{1.70622057731},  // MIDI 95
    StringImpedancePreset{1.68895544985},  // MIDI 96
    StringImpedancePreset{1.66721274056},  // MIDI 97
    StringImpedancePreset{1.64681393361},  // MIDI 98
    StringImpedancePreset{1.62400957623},  // MIDI 99
    StringImpedancePreset{1.60055424045},  // MIDI 100
    StringImpedancePreset{1.57796485708},  // MIDI 101
    StringImpedancePreset{1.55227685138},  // MIDI 102
    StringImpedancePreset{1.52476666739},  // MIDI 103
    StringImpedancePreset{1.49936735910},  // MIDI 104
    StringImpedancePreset{1.47095957610},  // MIDI 105
    StringImpedancePreset{1.44393685561},  // MIDI 106
    StringImpedancePreset{1.41467684717},  // MIDI 107 B7

    // RT-425 boundary extrapolation
    StringImpedancePreset{1.38608153033},  // MIDI 108 C8
}};

}  // namespace Parameters::String::RT425Wrapped



class StringImpedancePresets {
    StringImpedancePresets(
        const StringImpedancePresets&
    ) = delete;

    StringImpedancePresets&
    operator=(
        const StringImpedancePresets&
    ) = delete;

public:
    using Preset =
        Parameters::String::RT425Wrapped::
            StringImpedancePreset;

    using PresetTable =
        std::array<
            Preset,
            88
        >;

    constexpr StringImpedancePresets() noexcept =
        default;


    const Preset* find_preset(
        int midi_n
    ) const noexcept
    {
        constexpr int kMidiMin = 21;
        constexpr int kMidiMax = 108;

        if (
            midi_n < kMidiMin ||
            midi_n > kMidiMax
        ) {
            return nullptr;
        }

        const std::size_t index =
            static_cast<std::size_t>(
                midi_n - kMidiMin
            );

        return &sharedPresets()[index];
    }


    double get_characteristic_impedance(
        int midi_n
    ) const noexcept
    {
        const Preset* preset =
            find_preset(midi_n);

        if (preset == nullptr) {
            return 0.0;
        }

        return
            preset->
                characteristic_impedance_kg_s;
    }


    const PresetTable&
    getPresetTable() const noexcept
    {
        return sharedPresets();
    }


private:
    static constexpr const PresetTable&
    sharedPresets() noexcept
    {
        return
            Parameters::String::RT425Wrapped::
                kStringImpedancePresets;
    }
};


#endif /* string_impedance_preset_hpp */
