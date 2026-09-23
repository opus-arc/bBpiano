#ifndef soundboard_model_hpp
#define soundboard_model_hpp

#include <array>
#include <algorithm>
#include <cmath>
#include <cstddef>

class SoundboardModel {
    static constexpr bool soundboard_active_ = true;

    SoundboardModel(const SoundboardModel&) = delete;
    SoundboardModel& operator=(const SoundboardModel&) = delete;

    static constexpr std::size_t kKeyCount = 88;
    static constexpr std::size_t kFDNSize = 8;
    static constexpr std::size_t kMaxDelay = 721;

    // Bank:
    //
    // 37, 87, 181, 271, 359, 492, 687, 721 samples
    //
    // Short delays are essential for preserving the early energy
    // of the piano soundboard impulse response.
    static constexpr std::array<std::size_t, kFDNSize>
    kDelayLengths{
        37,
        87,
        181,
        271,
        359,
        492,
        687,
        721
    };


    // ================================================================
    // Delay line
    // ================================================================

    struct DelayLine {
        std::array<float, kMaxDelay> data{};

        std::size_t length = 1;
        std::size_t index = 0;

        inline void set_length(std::size_t new_length) noexcept {
            length = std::clamp<std::size_t>(
                new_length,
                1,
                kMaxDelay
            );

            index = 0;
            data.fill(0.0f);
        }

        inline float read() const noexcept {
            return data[index];
        }

        inline void write(float x) noexcept {
            data[index] = x;

            ++index;

            if (index >= length) {
                index = 0;
            }
        }

        inline void reset() noexcept {
            data.fill(0.0f);
            index = 0;
        }
    };


    static constexpr std::size_t kLossSectionCount = 8;


    // ================================================================
    // 单一二阶filter的公式：
    //              b0 + b1 z^-1 + b2 z^-2
    // H(z) = ---------------------------------
    //              1 + a1 z^-1 + a2 z^-2
    // 此处阶数的意思是系统差分方程需要记忆多少阶过去状态
    // ================================================================

    struct LossSection {
        float b0 = 1.0f;
        float b1 = 0.0f;
        float b2 = 0.0f;

        float a1 = 0.0f;
        float a2 = 0.0f;

        float s1 = 0.0f;
        float s2 = 0.0f;

        inline float process(float x) noexcept {
            const float y =
                b0 * x
                + s1;

            s1 =
                b1 * x
                - a1 * y
                + s2;

            s2 =
                b2 * x
                - a2 * y;

            return y;
        }

        inline void reset() noexcept {
            s1 = 0.0f;
            s2 = 0.0f;
        }
    };


    // ================================================================
    // 二阶filters的串联公式：
    // H_total(z) = H_1(z) H_2(z) ... H_M(z)
    // ================================================================

    struct LossFilter {
        std::array<LossSection, kLossSectionCount> sections{};

        inline float process(float x) noexcept {
            for (auto& section : sections) {
                x = section.process(x);
            }

            return x;
        }

        inline void reset() noexcept {
            for (auto& section : sections) {
                section.reset();
            }
        }
    };


    // ================================================================
    // Bridge-region shaping
    //
    // Bank notes that different excitation positions on the bridge
    // produce different force-pressure responses.
    //
    // However, they excite essentially the same soundboard modes.
    // Therefore a shared FDN is used after position-dependent
    // shaping filters.
    //
    // This low/high decomposition is a provisional low-order
    // approximation until measured P/F FIR coefficients are available.
    // ================================================================

    struct RegionShaper {
        float alpha = 0.0f;

        float low_gain = 1.0f;
        float high_gain = 1.0f;

        float low_state = 0.0f;

        inline float process(float x) noexcept {
            low_state =
                alpha * low_state
                + (1.0f - alpha) * x;

            const float low = low_state;
            const float high = x - low;

            return
                low_gain * low
                + high_gain * high;
        }

        inline void reset() noexcept {
            low_state = 0.0f;
        }
    };


    std::array<DelayLine, kFDNSize> delay_lines_{};
    std::array<LossFilter, kFDNSize> loss_filters_{};

    RegionShaper bass_shaper_{};
    RegionShaper middle_shaper_{};
    RegionShaper treble_shaper_{};

    double sample_rate_ = 44100.0;


public:

    mutable std::array<float, kKeyCount> bridge_force{};


    explicit SoundboardModel(
        double sample_rate = 44100.0
    ) :
        sample_rate_(sample_rate)
    {
        initialize_delays();
        initialize_losses();
        initialize_region_shapers();
    }


    // ================================================================
    // Per-sample soundboard radiation
    //
    // bridge force
    //
    //      ↓
    //
    // spatial region shaping
    //
    //      ↓
    //
    // sum
    //
    //      ↓
    //
    // 8-line high-modal-density FDN
    //
    //      ↓
    //
    // radiation proxy
    // ================================================================

    inline float get_sample() noexcept {
        if(!soundboard_active_) {
            float sum = 0.0;
            for(int i = 0; i < bridge_force.size(); i++) {
                sum += bridge_force[i];
            }
            return sum;
        }

        // ------------------------------------------------------------
        // 1. Spatial bridge-force reduction
        //
        // Current split is provisional.
        //
        // MIDI 21...40  -> bass
        // MIDI 41...75  -> middle
        // MIDI 76...108 -> treble
        // ------------------------------------------------------------

        float bass_force = 0.0f;
        float middle_force = 0.0f;
        float treble_force = 0.0f;

        for (std::size_t i = 0; i < 20; ++i) {
            bass_force += bridge_force[i];
        }

        for (std::size_t i = 20; i < 55; ++i) {
            middle_force += bridge_force[i];
        }

        for (std::size_t i = 55; i < 88; ++i) {
            treble_force += bridge_force[i];
        }


        // ------------------------------------------------------------
        // 2. Position-dependent shaping
        //
        // The treble region intentionally retains more high-frequency
        // energy than the bass region.
        // ------------------------------------------------------------

        const float shaped_force =
            bass_shaper_.process(bass_force)
            +
            middle_shaper_.process(middle_force)
            +
            treble_shaper_.process(treble_force);


        // ------------------------------------------------------------
        // 3. Read delayed states and apply frequency-dependent losses
        //
        // In FDN notation:
        //
        // s(z) = D(z) [ A s(z) + b x(z) ]
        // ------------------------------------------------------------

        std::array<float, kFDNSize> delayed{};

        float delayed_sum = 0.0f;

        for (std::size_t i = 0; i < kFDNSize; ++i) {

            delayed[i] =
                loss_filters_[i].process(
                    delay_lines_[i].read()
                );

            delayed_sum += delayed[i];
        }


        // ------------------------------------------------------------
        // 4. FDN output vector c
        //
        // A sign-alternating normalized vector avoids all delay lines
        // contributing with identical phase/sign.
        // ------------------------------------------------------------

        constexpr float q =
            0.3535533905932738f; // 1 / sqrt(8)

        const float fdn_output =
            q *
            (
                delayed[0]
                - delayed[1]
                + delayed[2]
                - delayed[3]
                + delayed[4]
                - delayed[5]
                + delayed[6]
                - delayed[7]
            );


        // ------------------------------------------------------------
        // 5. Orthogonal feedback matrix
        //
        // Start with the Householder matrix:
        //
        //          A = I - (2/N) J
        //
        // N = 8:
        //
        //          A = I - 0.25 J
        //
        //
        // Bank shifts this circular matrix by one position.
        //
        // Therefore:
        //
        // feedback[i]
        //      = delayed[(i + 1) mod 8]
        //        - 0.25 * sum(delayed)
        //
        //
        // This is mathematically equivalent to the full 8×8
        // matrix multiplication but much cheaper.
        // ------------------------------------------------------------

        const float common =
            0.25f * delayed_sum;


        // ------------------------------------------------------------
        // Input vector b.
        //
        // Normalized equal excitation of all delay lines.
        // ------------------------------------------------------------

        const float input =
            q * shaped_force;


        for (std::size_t i = 0; i < kFDNSize; ++i) {

            const std::size_t shifted =
                (i + 1) & 7;

            const float feedback =
                delayed[shifted]
                - common;

            delay_lines_[i].write(
                feedback + input
            );
        }


        // ------------------------------------------------------------
        // 6. Direct component
        //
        // Eq. (5.4) allows:
        //
        // y = c^T s + d x
        //
        // A modest direct component preserves the immediate
        // bridge-to-air transient while the FDN represents the dense
        // decaying modal field.
        //
        // This value is provisional and should eventually be included
        // in measured force-pressure calibration.
        // ------------------------------------------------------------

        constexpr float direct_gain = 0.18f;

        return
            fdn_output
            + direct_gain * shaped_force;
    }


    inline void system_reset() noexcept {

        bridge_force.fill(0.0f);

        for (auto& delay : delay_lines_) {
            delay.reset();
        }

        for (auto& loss : loss_filters_) {
            loss.reset();
        }

        bass_shaper_.reset();
        middle_shaper_.reset();
        treble_shaper_.reset();
    }


private:

    inline void initialize_delays() noexcept {

        for (std::size_t i = 0; i < kFDNSize; ++i) {
            delay_lines_[i].set_length(
                kDelayLengths[i]
            );
        }
    }


    inline void initialize_losses() noexcept {

        // ============================================================
        // Bank calibrates these filters from measured soundboard
        // decay times in octave bands.
        //
        // The exact numerical coefficients are not published.
        //
        // For this provisional implementation we reproduce the same
        // structure and use a physically meaningful decay mapping:
        //
        // gain =
        //
        //       10 ^ [ -3 * delay_time / T60 ]
        //
        //
        // so that repeated circulation decays by 60 dB in T60.
        //
        // The one-pole term adds frequency-dependent damping.
        // ============================================================

        constexpr double target_t60 = 0.34;

        // Keep this fairly weak.
        //
        // Too high a value makes the piano sound artificially dark.
        constexpr float loss_pole = 0.075f;


        for (std::size_t i = 0; i < kFDNSize; ++i) {

            const double delay_seconds =
                static_cast<double>(kDelayLengths[i])
                / sample_rate_;

            const double gain =
                std::pow(
                    10.0,
                    -3.0 *
                    delay_seconds /
                    target_t60
                );


            // H(z) = 1
            //
            // b0 = 1
            // b1 = b2 = a1 = a2 = 0

            for (auto& section : loss_filters_[i].sections) {
                section.b0 = 1.0f;
                section.b1 = 0.0f;
                section.b2 = 0.0f;

                section.a1 = 0.0f;
                section.a2 = 0.0f;

                section.s1 = 0.0f;
                section.s2 = 0.0f;
            }


            // 让二阶filter塌缩成一阶filter，以此先证明两者可在此参数设置下等价:
            //
            //            g (1 - a)
            // H(z) = ----------------
            //            1 - a z^-1
            //
            // 通过进行以下的参数代入：
            // b0 = g (1 - a)
            // b1 = 0
            // b2 = 0
            // a1 = -a
            // a2 = 0

            auto& section =
                loss_filters_[i].sections[0];

            section.b0 =
                static_cast<float>(gain)
                * (1.0f - loss_pole);

            section.b1 = 0.0f;
            section.b2 = 0.0f;

            section.a1 = -loss_pole;
            section.a2 = 0.0f;
        }
    }


    inline void initialize_region_shapers() noexcept {

        // One-pole crossover around 1.8 kHz.
        //
        // alpha = exp(-2πfc/Fs)

        constexpr double crossover_hz = 1800.0;

        const float alpha =
            static_cast<float>(
                std::exp(
                    -2.0 *
                    3.14159265358979323846 *
                    crossover_hz /
                    sample_rate_
                )
            );


        // ------------------------------------------------------------
        // These three gain pairs are NOT published Bank coefficients.
        //
        // They only approximate the experimentally observed tendency:
        //
        // low bridge excitation  -> softer radiation
        // high bridge excitation -> brighter radiation
        //
        // Bank explicitly reports this behavior in the measurements.
        // ------------------------------------------------------------

        bass_shaper_.alpha = alpha;
        bass_shaper_.low_gain = 1.00f;
        bass_shaper_.high_gain = 0.82f;


        middle_shaper_.alpha = alpha;
        middle_shaper_.low_gain = 1.00f;
        middle_shaper_.high_gain = 1.00f;


        treble_shaper_.alpha = alpha;
        treble_shaper_.low_gain = 0.95f;
        treble_shaper_.high_gain = 1.28f;
    }
};

#endif /* soundboard_model_hpp */





//
//class SoundboardModel {
//    SoundboardModel(const SoundboardModel&) = delete;
//    SoundboardModel& operator=(const SoundboardModel&) = delete;
//
//    // ================================================================
//    // Reduced soundboard radiation model
//    //
//    // Input:
//    //      bridge force F_b[n]
//    //
//    // Output:
//    //      radiated acoustic pressure proxy p[n]
//    //
//    // This model is intentionally outside the mechanical feedback loop.
//    //
//    // It does NOT model the bridge impedance seen by the strings.
//    // Instead, it approximates the spatially dependent transfer:
//    //
//    //                  P(ω)
//    //      H(ω) = ----------------
//    //                F_bridge(ω)
//    //
//    // using three bridge regions and four damped modes per region.
//    // ================================================================
//
//    static constexpr int kModeCount = 4;
//
//    struct RadiationMode {
//        // Transfer function:
//        //
//        //                 b0
//        // H(z) = -----------------------
//        //         1 + a1 z^-1 + a2 z^-2
//        //
//        // implemented in transposed DF-II.
//
//        float b0 = 0.0f;
//        float a1 = 0.0f;
//        float a2 = 0.0f;
//
//        float s1 = 0.0f;
//        float s2 = 0.0f;
//
//        inline float process(float x) noexcept {
//            const float y =
//                b0 * x + s1;
//
//            s1 =
//                -a1 * y + s2;
//
//            s2 =
//                -a2 * y;
//
//            return y;
//        }
//
//        inline void reset() noexcept {
//            s1 = 0.0f;
//            s2 = 0.0f;
//        }
//    };
//
//    struct RadiationRegion {
//        std::array<RadiationMode, kModeCount> modes{};
//
//        inline float process(float force) noexcept {
//            float pressure = 0.0f;
//
//            for (auto& mode : modes) {
//                pressure += mode.process(force);
//            }
//
//            return pressure;
//        }
//
//        inline void reset() noexcept {
//            for (auto& mode : modes) {
//                mode.reset();
//            }
//        }
//    };
//
//    RadiationRegion bass_;
//    RadiationRegion middle_;
//    RadiationRegion treble_;
//
//    double sample_rate_ = 44100.0;
//
//public:
//
//    mutable std::array<float, 88> bridge_force{};
//
//    explicit SoundboardModel(double sample_rate = 44100.0)
//        : sample_rate_(sample_rate)
//    {
//        // ------------------------------------------------------------
//        // Bass bridge region
//        //
//        // Lower-frequency modal emphasis.
//        // ------------------------------------------------------------
//
//        bass_.modes[0] =
//            make_mode(95.0,   0.10, 0.32);
//
//        bass_.modes[1] =
//            make_mode(210.0,  0.08, 0.25);
//
//        bass_.modes[2] =
//            make_mode(430.0,  0.07, 0.16);
//
//        bass_.modes[3] =
//            make_mode(850.0,  0.09, 0.08);
//
//
//        // ------------------------------------------------------------
//        // Middle bridge region
//        // ------------------------------------------------------------
//
//        middle_.modes[0] =
//            make_mode(140.0,  0.09, 0.20);
//
//        middle_.modes[1] =
//            make_mode(370.0,  0.07, 0.24);
//
//        middle_.modes[2] =
//            make_mode(820.0,  0.06, 0.18);
//
//        middle_.modes[3] =
//            make_mode(1650.0, 0.08, 0.10);
//
//
//        // ------------------------------------------------------------
//        // Treble bridge region
//        //
//        // More energy is retained in the upper modal range.
//        // ------------------------------------------------------------
//
//        treble_.modes[0] =
//            make_mode(230.0,  0.10, 0.10);
//
//        treble_.modes[1] =
//            make_mode(720.0,  0.08, 0.17);
//
//        treble_.modes[2] =
//            make_mode(1700.0, 0.07, 0.20);
//
//        treble_.modes[3] =
//            make_mode(3400.0, 0.10, 0.12);
//    }
//
//
//    inline float get_sample() noexcept {
//
//        // ============================================================
//        // Spatial reduction of the bridge-force field
//        //
//        // MIDI:
//        //
//        // 21 ... 40   bass
//        // 41 ... 75   middle
//        // 76 ... 108  treble
//        //
//        // bridge_force[0] corresponds to MIDI 21.
//        // ============================================================
//
//        float bass_force = 0.0f;
//        float middle_force = 0.0f;
//        float treble_force = 0.0f;
//
//        for (int i = 0; i < 20; ++i) {
//            bass_force += bridge_force[i];
//        }
//
//        for (int i = 20; i < 55; ++i) {
//            middle_force += bridge_force[i];
//        }
//
//        for (int i = 55; i < 88; ++i) {
//            treble_force += bridge_force[i];
//        }
//
//
//        // ============================================================
//        // Bridge force -> radiated pressure proxy
//        // ============================================================
//
//        const float bass_pressure =
//            bass_.process(bass_force);
//
//        const float middle_pressure =
//            middle_.process(middle_force);
//
//        const float treble_pressure =
//            treble_.process(treble_force);
//
//
//        return
//            bass_pressure +
//            middle_pressure +
//            treble_pressure;
//    }
//
//
//    inline void system_reset() noexcept {
//
//        bridge_force.fill(0.0f);
//
//        bass_.reset();
//        middle_.reset();
//        treble_.reset();
//    }
//
//
//private:
//
//    RadiationMode make_mode(
//        double frequency_hz,
//        double damping_ratio,
//        double gain)
//    {
//        // ============================================================
//        // Continuous damped mode:
//        //
//        // poles:
//        //
//        // s = -ζω₀ ± j ω₀ sqrt(1-ζ²)
//        //
//        // Mapping directly into discrete-time poles:
//        //
//        // r     = exp(-ζω₀Ts)
//        //
//        // theta = ω₀ sqrt(1-ζ²) Ts
//        //
//        // denominator:
//        //
//        // 1 - 2r cos(theta) z^-1 + r² z^-2
//        //
//        // Therefore:
//        //
//        // a1 = -2r cos(theta)
//        // a2 = r²
//        // ============================================================
//
//        const double omega =
//            2.0 *
//            std::numbers::pi *
//            frequency_hz;
//
//        const double ts =
//            1.0 / sample_rate_;
//
//        const double damping =
//            std::max(
//                0.0,
//                std::min(damping_ratio, 0.999999)
//            );
//
//        const double radius =
//            std::exp(
//                -damping *
//                omega *
//                ts
//            );
//
//        const double damped_omega =
//            omega *
//            std::sqrt(
//                1.0 -
//                damping * damping
//            );
//
//        const double theta =
//            damped_omega * ts;
//
//        RadiationMode mode;
//
//        mode.a1 =
//            static_cast<float>(
//                -2.0 *
//                radius *
//                std::cos(theta)
//            );
//
//        mode.a2 =
//            static_cast<float>(
//                radius * radius
//            );
//
//        // Modal excitation coefficient.
//        //
//        // This normalization keeps the gain parameter usable as a
//        // relative modal-strength parameter rather than allowing the
//        // pole radius alone to dominate the amplitude.
//        mode.b0 =
//            static_cast<float>(
//                gain *
//                (1.0 - radius)
//            );
//
//        return mode;
//    }
//};
//
//#endif /* soundboard_model_hpp */
//





//#ifndef soundboard_model_hpp
//#define soundboard_model_hpp
//
//#include <array>
//class SoundboardModel {
//    SoundboardModel(const SoundboardModel&) = delete;
//    SoundboardModel& operator=(const SoundboardModel&) = delete;
//
//    // Temporary bridge-force -> acoustic-radiation proxy.
//    //
//    // This filter is intentionally placed outside the mechanical
//    // string/bridge feedback loop. It does not represent the soundboard
//    // mobility itself; it only approximates the spectral coloration
//    // between total bridge excitation and radiated acoustic output.
//    //
//    // Fs = 44100 Hz
//    float radiation_s1_ = 0.0f;
//    float radiation_s2_ = 0.0f;
//
//public:
//    mutable std::array<float, 88> bridge_force{};
//
//    explicit SoundboardModel() = default;
//
//    inline float get_sample() {
//        float bridge_excitation = 0.0f;
//
//        for (std::size_t i = 0; i < bridge_force.size(); ++i) {
//            bridge_excitation += bridge_force[i];
//        }
//
//        /*
//            Temporary radiation transfer function:
//
//                         b0 + b1 z^-1 + b2 z^-2
//                H(z) = -------------------------
//                          1 + a1 z^-1 + a2 z^-2
//
//            This is NOT a physical soundboard model.
//
//            It is a low-order approximation of the transfer from
//            aggregated bridge excitation to radiated acoustic output,
//            introduced only to avoid treating raw bridge force as PCM.
//        */
//
//        constexpr float b0 = 0.08315987f;
//        constexpr float b1 = 0.16631974f;
//        constexpr float b2 = 0.08315987f;
//
//        constexpr float a1 = -1.03517121f;
//        constexpr float a2 = 0.36781069f;
//
//        const float radiation =
//            b0 * bridge_excitation + radiation_s1_;
//
//        radiation_s1_ =
//            b1 * bridge_excitation
//            - a1 * radiation
//            + radiation_s2_;
//
//        radiation_s2_ =
//            b2 * bridge_excitation
//            - a2 * radiation;
//
//        return radiation;
//    }
//
//    inline void system_reset() {
//        bridge_force.fill(0.0f);
//
//        radiation_s1_ = 0.0f;
//        radiation_s2_ = 0.0f;
//    }
//};
//
////class SoundboardModel {
////    SoundboardModel(const SoundboardModel&) = delete;
////    SoundboardModel& operator=(const SoundboardModel&) = delete;
////
////    float radiation_z1_ = 0.0f;
////    float radiation_z2_ = 0.0f;
////
////public:
////    mutable std::array<float, 88> bridge_force{};
////
////    explicit SoundboardModel() {
////
////    }
//////    inline float get_sample() {
//////        float bridge_sum = 0.0f;
//////
//////        for (int i = 0; i < bridge_force.size(); ++i) {
//////            bridge_sum += bridge_force[i];
//////        }
//////
//////        // ============================================================
//////        // Temporary soundboard radiation filter
//////        //
//////        // 这里只处理最终辐射输出，不参与 string / bridge feedback。
//////        // 目的：暂时抑制直接 bridge-force 输出中过强的高频与尖锐感。
//////        // ============================================================
//////
//////        // 二阶 Butterworth 风格低通。
//////        // 当前系数按 44.1 kHz 下约 5 kHz 截止频率使用。
//////        constexpr float b0 = 0.08315987f;
//////        constexpr float b1 = 0.16631974f;
//////        constexpr float b2 = 0.08315987f;
//////
//////        constexpr float a1 = -1.03517121f;
//////        constexpr float a2 = 0.36781069f;
//////
//////        const float filtered =
//////            b0 * bridge_sum + radiation_z1_;
//////
//////        radiation_z1_ =
//////            b1 * bridge_sum
//////            - a1 * filtered
//////            + radiation_z2_;
//////
//////        radiation_z2_ =
//////            b2 * bridge_sum
//////            - a2 * filtered;
//////
//////        // 保留少量未经滤波的 bridge signal，
//////        // 避免暂时的低通使 attack 完全失去存在感。
//////        constexpr float direct_mix = 0.15f;
//////        constexpr float filtered_mix = 0.85f;
//////
//////        return direct_mix * bridge_sum
//////             + filtered_mix * filtered;
//////    }
////    inline float get_sample() {
////        float sum = 0.0f;
////        for(int i = 0; i < bridge_force.size(); i++) {
////            sum += bridge_force[i];
////        }
////        return sum;
////    }
////
////    inline void system_reset() {
////        bridge_force.fill(0.0f);
////
////        radiation_z1_ = 0.0f;
////        radiation_z2_ = 0.0f;
////    }
////};
//
//#endif /* soundboard_model_hpp */
