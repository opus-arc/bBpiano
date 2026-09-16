// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
// [ZERO AI-GENERATED CODE]
// Every line in this file is written and understood by its author. Every result is
// reproducible, every assumption is open to inspection, and every implementation
// stands open to criticism and challenge.
// AI may be used for non-core, replaceable, engineering work;
// this file, however, contains core logic that the author considers
// necessary to understand firsthand, explain line by line, and take full responsibility for,
// and is therefore implemented entirely by hand.
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

#include "./string_model.hpp"
#include "./hammer_model.hpp"
#include "./bridge_model.hpp"
#include "./soundboard_model.hpp"

#include "../configuration/configuration.hpp"

class KeyModel {
    
    // 弦锤耦合与tunning
    
public:
    
    double samplerate_ = 0.0;
    
    int midi_n_;
    int string_count_ = 3;
    
    const BridgeModel *bridge_;
    const SoundboardModel *soundboard_;
    HammerModel hammer_;
    std::array<StringModel, 3> strings_;
    
    std::array<double, 3> string_vs_ = {0.0, 0.0, 0.0};
    
    bool key_down_ = false;
    bool key_active_ = false;
    
    bool sustainpedal_active_ = false;
    
    const Configuration *configuration_ = nullptr;
    
    KeyModel(int midi_n,
             double sample_rate,
             int string_count,
             TunningPresets::Temperament temperament,
             const SoundboardModel *soundboard,
             const BridgeModel *bridge,
             const Configuration *configuration) :
        midi_n_(midi_n),
        string_count_(string_count),
        samplerate_(sample_rate),
        hammer_(sample_rate, midi_n, configuration),
        strings_{
            StringModel(sample_rate,
                        midi_n,
                        temperament,
                        TunningPresets::StringIndex::left,
                        configuration),
            StringModel(sample_rate,
                        midi_n,
                        temperament,
                        TunningPresets::StringIndex::center,
                        configuration),
            StringModel(sample_rate,
                        midi_n,
                        temperament,
                        TunningPresets::StringIndex::right,
                        configuration),
        },
        soundboard_(soundboard),
        bridge_(bridge),
        configuration_(configuration)
    {}
    
    void key_movement() {
        
        sustainpedal_controller();
        
        double total_impedance = 0.0;
        double weighted_string_velocity = 0.0;
        
        for (int i = 0; i < string_count_; ++i) {
            string_vs_[i] = strings_[i].get_string_vs();

            total_impedance += strings_[i].z_;

            weighted_string_velocity += strings_[i].z_ * string_vs_[i];
        }
        
        double contact_string_velocity = 0.0;
        
        if (total_impedance > 0.0) {
            contact_string_velocity = weighted_string_velocity / total_impedance;
        }
        
        double hammer_force_1 = hammer_.hammer_movement(contact_string_velocity);
        double hammer_force_2 = hammer_.hammer_movement(contact_string_velocity + hammer_force_1 / (total_impedance * 2.0));
        double hammer_force = (hammer_force_1 + hammer_force_2) / 2.0;
        
        for(int i = 0; i < string_count_; i++) {
            const double force_ratio = strings_[i].z_ / total_impedance;
            const double string_force = hammer_force * force_ratio;
            strings_[i].string_movement(string_force);
            // bridge_->process(strings_[i].right_boundary_point);
        }
        
        update_bridge_force();
        
        check_active();
        
    }
    
    void trigger(double velocity_mps) {
        for(int i = 0; i < string_count_; i++) {
            strings_[i].is_active = true;
            string_vs_[i] = 0.0;
        }
        hammer_.trigger(velocity_mps);
    }
    
    void update_bridge_force() {
        float result = 0.0;
        for(int i = 0; i < string_count_; i++) {
            result += strings_[i].get_bridge_force();
        }
        soundboard_->bridge_force[midi_n_ - 21] = result;
    }
    
    void system_reset() {
        for(int i = 0; i < string_count_; i++) {
            strings_[i].system_reset();
        }
        hammer_.system_reset();
        key_down_ = false;
        key_active_ = false;
        string_vs_.fill(0.0);
        sustainpedal_active_ = false;
    }
    
private:
    inline void check_active() {
        key_active_ = hammer_.is_contacting_;

        for (int i = 0; i < string_count_; ++i) {
            key_active_ = key_active_ || strings_[i].is_active;
        }
    }
    
    void sustainpedal_controller() {
        // key 抬起且延音踏板未踩下则启动制音器
        const bool damper_should_touch = !key_down_ && !sustainpedal_active_;
        for (int index = 0; index < string_count_; ++index) {
            strings_[index].damper_active = damper_should_touch;
        }
    }
};

#endif /* key_model_hpp */
