#ifndef engine_rate_eval_hpp
#define engine_rate_eval_hpp

#include <cstdint>
#include <iomanip>
#include <iostream>

#include <mach/mach_time.h>

class EngineEval {
private:
    uint64_t start_ = 0;

    double sample_rate_ = 44100.0;
    double instant_rate_ = 0.0;

public:
    explicit EngineEval(double sample_rate)
        : sample_rate_(sample_rate) {}

    inline void start_timing() noexcept {
        start_ =
            clock_gettime_nsec_np(CLOCK_UPTIME_RAW);
    }

    inline void end_timing(int frame_count) noexcept {
        const uint64_t end =
            clock_gettime_nsec_np(CLOCK_UPTIME_RAW);

        const uint64_t actual_ns =
            end - start_;

        const double buffer_ns =
            1'000'000'000.0 *
            static_cast<double>(frame_count) /
            sample_rate_;

        const double current_rate =
            static_cast<double>(actual_ns) /
            buffer_ns;

        instant_rate_ =
            0.9 * instant_rate_ +
            0.1 * current_rate;
    }

    [[nodiscard]]
    inline double engine_rate() const noexcept {
        return instant_rate_;
    }

    [[nodiscard]]
    inline double engine_rate_percent() const noexcept {
        return instant_rate_ * 100.0;
    }

    [[nodiscard]]
    inline double realtime_multiple() const noexcept {
        if (instant_rate_ <= 0.0) {
            return 0.0;
        }

        return 1.0 / instant_rate_;
    }

    [[nodiscard]]
    inline bool realtime_capable() const noexcept {
        return instant_rate_ <= 1.0;
    }

    [[nodiscard]]
    double benchmark(
        std::unique_ptr<PianoModel>& piano,
        int warmup_frames = 512,
        int measure_frames = 8192,
        int repetitions = 5
    ) const {

        constexpr int kMaxRepetitions = 31;

        repetitions =
            std::clamp(repetitions, 1, kMaxRepetitions);

        std::array<double, kMaxRepetitions> results{};

        volatile float sample_sink = 0.0f;

        for (int repetition = 0;
             repetition < repetitions;
             ++repetition) {

            for (int frame = 0;
                 frame < warmup_frames;
                 ++frame) {

                piano->piano_movement();

                sample_sink =
                    sample_sink +
                    piano->get_sample();
            }

            const uint64_t start =
                clock_gettime_nsec_np(
                    CLOCK_UPTIME_RAW
                );

            for (int frame = 0;
                 frame < measure_frames;
                 ++frame) {

                piano->piano_movement();

                sample_sink =
                    sample_sink +
                    piano->get_sample();
            }

            const uint64_t end =
                clock_gettime_nsec_np(
                    CLOCK_UPTIME_RAW
                );

            const double actual_ns =
                static_cast<double>(end - start);

            const double rendered_ns =
                1'000'000'000.0 *
                static_cast<double>(measure_frames) /
                sample_rate_;

            results[repetition] =
                actual_ns / rendered_ns;
        }

        std::sort(
            results.begin(),
            results.begin() + repetitions
        );

        return results[
            static_cast<std::size_t>(
                repetitions / 2
            )
        ];
    }
    
    inline void reset() noexcept {
        instant_rate_ = 0.0;
    }
    void print_benchmark(std::unique_ptr<PianoModel>& piano, std::string range) const {
        const double rate =
            benchmark(piano);

        const double frame_budget_us =
            1'000'000.0 / sample_rate_;

        const double frame_cost_us =
            frame_budget_us * rate;

        std::cout
            << std::fixed
            << std::setprecision(2)

//            << "bbpl L1 DSP benchmark\n"
//            << "Sample rate: "
//            << sample_rate_
//            << " Hz\n"

            << "    Frame: "
            << frame_cost_us
            << " us / "
            << frame_budget_us
            << " us\n"

            << "    Occupancy: "
            << rate * 100.0
            << "%\n"

            << "    Speed: "
            << (1.0 / rate)
            << "x realtime\n"

            <<  "    " + range + " realtime: "
            << (rate <= 1.0 ? "PASS" : "FAIL")
            << "\n\n";
    }
    
};

#endif
