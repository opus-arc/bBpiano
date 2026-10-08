// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
// [ZERO AI-GENERATED CODE]
// Every line in this file is written and understood by its author. Every result
// is reproducible, every assumption is open to inspection, and every
// implementation stands open to criticism and challenge. AI may be used for
// non-core, replaceable, engineering work; this file, however, contains core
// logic that the author considers necessary to understand firsthand, explain
// line by line, and take full responsibility for, and is therefore implemented
// entirely by hand.
// ---------------------------------------------------------------------------
// [本文件承诺不含任何 AI 生成代码]
// 每一行代码均由作者亲自编写，并确知其意义。一切结果可以复现，一切假设可经受检验，一切实现经得起批评与质疑。
// AI 可用于非核心、可替代的工程工作；
// 本文件承载作者认为必须亲自理解、能够逐行解释并为之负责的核心逻辑，因此刻意保持完全人工实现。
//
// Ziyang Tan
// 2026-09-04
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

#ifndef key_model_hpp
#define key_model_hpp

#include <array>

#include "./bridge_model.hpp"
#include "./hammer_model.hpp"
#include "./soundboard_model.hpp"
#include "./string_model.hpp"

#include "../configuration/configuration.hpp"

class KeyModel {

    // 弦锤耦合与tunning

public:
    double samplerate_ = 0.0;

    int midi_n_;
    int string_count_ = 3;

    std::array<BridgeModel, 3> bridges_;
    HammerModel hammer_;
    std::array<StringModel, 3> strings_;

    std::array<double, 3> string_vs_ = {0.0, 0.0, 0.0};

    bool key_down_ = false;
    bool key_active_ = false;

    bool sustainpedal_active_ = false;

    const Configuration *configuration_ = nullptr;

    // 构造后琴弦阻抗不再变化，因此提前计算
    double total_impedance_ = 0.0;
    double inverse_total_impedance_ = 0.0;
    double inverse_double_total_impedance_ = 0.0;
    std::array<double, 3> force_ratios_ = {0.0, 0.0, 0.0};

    // ======================== ========================
    // Compute
    // midi_n_ > 88
    // ======================== ========================
    std::array<float, 3> last_vb_88{};

    KeyModel(int midi_n, double sample_rate, int string_count,
             TunningPresets::Temperament temperament,
             SoundboardModel *soundboard,
             const Configuration *configuration)
    :   midi_n_(midi_n), string_count_(string_count), samplerate_(sample_rate),
        hammer_(sample_rate, midi_n, configuration),
        bridges_{
            BridgeModel(midi_n,
                configuration->bridge_transmission_presets.get_fir_coefficients(midi_n),
                soundboard),
            BridgeModel(midi_n,
                configuration->bridge_transmission_presets.get_fir_coefficients(midi_n),
                soundboard),
            BridgeModel(midi_n,
                configuration->bridge_transmission_presets.get_fir_coefficients(midi_n),
                soundboard)
        },
        strings_{
            StringModel(sample_rate, midi_n, temperament,
                        TunningPresets::StringIndex::left, configuration, &bridges_[0]),
            StringModel(sample_rate, midi_n, temperament,
                        TunningPresets::StringIndex::center, configuration, &bridges_[1]),
            StringModel(sample_rate, midi_n, temperament,
                        TunningPresets::StringIndex::right, configuration, &bridges_[2]),
        },
        configuration_(configuration)
    {
        for (int i = 0; i < string_count_; ++i) {
            total_impedance_ += strings_[i].z_s_;
        }

        if (total_impedance_ > 0.0) {
            inverse_total_impedance_ = 1.0 / total_impedance_;
            inverse_double_total_impedance_ = 1.0 / (total_impedance_ * 2.0);

            for (int i = 0; i < string_count_; ++i) {
                force_ratios_[i] = strings_[i].z_s_ * inverse_total_impedance_;
            }
        }
    }

    inline void key_movement() {

        sustainpedal_controller();

        double contact_string_velocity = 0.0;

        for (int i = 0; i < string_count_; ++i) {
            string_vs_[i] = strings_[i].get_string_vs();

            contact_string_velocity += force_ratios_[i] * string_vs_[i];
        }

        double hammer_force_1 = hammer_.hammer_movement(contact_string_velocity);

        double hammer_force_2 = hammer_.hammer_movement(contact_string_velocity +
                                                        hammer_force_1 * inverse_double_total_impedance_);

        double hammer_force = (hammer_force_1 + hammer_force_2) / 2.0;

        for (int i = 0; i < string_count_; i++) {
            if (strings_[i].midi_n_ < 88) {
                strings_[i].string_movement(hammer_force * force_ratios_[i]);
            } else {
                strings_[i].string_movement(hammer_force_1 * force_ratios_[i]);
                last_vb_88[i] = bridges_[i].get_vb();
                strings_[i].string_movement(hammer_force_2 * force_ratios_[i]);
            }
        }
        
        check_active();

        if (!key_active_) {
            system_reset();
        }
    }

    inline void trigger(double velocity_mps) {
        for (int i = 0; i < string_count_; i++) {
            strings_[i].is_active = true;
            string_vs_[i] = 0.0;
        }
        hammer_.trigger(velocity_mps);
    }

    inline void system_reset() {
        for (int i = 0; i < string_count_; i++) {
            strings_[i].system_reset();
            bridges_[i].system_reset();
        }
        hammer_.system_reset();
        key_down_ = false;
        key_active_ = false;
        string_vs_.fill(0.0);
        sustainpedal_active_ = false;
        last_vb_88.fill(0.0f);
    }

private:
    inline void check_active() {
        key_active_ = hammer_.is_contacting_;

        for (int i = 0; i < string_count_; ++i) {
            key_active_ = key_active_ || strings_[i].is_active;
            key_active_ = key_active_ || bridges_[i].get_state_energy() > 1.0e-5f;
        }
        
    }

    inline void sustainpedal_controller() {
        // key 抬起且延音踏板未踩下则启动制音器
        const bool damper_should_touch = !key_down_ && !sustainpedal_active_;

        for (int index = 0; index < string_count_; ++index) {
            strings_[index].damper_active = damper_should_touch;
        }
    }
    
};

#endif /* key_model_hpp */
