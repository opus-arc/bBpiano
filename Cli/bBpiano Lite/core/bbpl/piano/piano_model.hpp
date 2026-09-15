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

#ifndef piano_model_hpp
#define piano_model_hpp

#include <iostream>
#include <array>

#include "./key_model.hpp"
#include "./bridge_model.hpp"
#include "./soundboard_model.hpp"
#include "../configuration/configuration.hpp"


class PianoModel {
    PianoModel(const PianoModel&) = delete;
    PianoModel& operator=(const PianoModel&) = delete;
public:
    
    static constexpr Configuration configuration{};
    BridgeModel bridge_;
    SoundboardModel soundboard_;
    std::array<KeyModel*, 88> piano_keys;
    
    PianoModel(double sample_rate) {
        for (std::size_t index = 0; index < piano_keys.size(); ++index) {
            const int midi_n = static_cast<int>(index) + 21;

            int string_count = 1;
            if (midi_n >= 34) {
                string_count = 3;
            } else if (midi_n >= 29) {
                string_count = 2;
            }
            
            piano_keys[index] = new KeyModel(midi_n,
                                             sample_rate,
                                             string_count,
                                             TunningPresets::Temperament::equal,
                                             &soundboard_,
                                             &bridge_,
                                             &configuration);
        }
    }
    
    inline void piano_movement() {
        for(int i = 0; i < piano_keys.size(); i++) {
            if(piano_keys[i]->key_active_)
                piano_keys[i]->key_movement();
        }
        
//        piano_keys[69-21]->key_movement();
    }
    
    inline float get_sample() {
        return 0.01 * soundboard_.get_sample();
    }
    
    inline void sustainpedal_control(bool is_active) {
        if(is_active) {
            for(int i = 0; i < piano_keys.size(); i++) {
                piano_keys[i]->sustainpedal_active_ = true;
            }
        } else {
            for(int i = 0; i < piano_keys.size(); i++) {
                piano_keys[i]->sustainpedal_active_ = false;
            }
        }
    }
    
    inline void system_reset() {
        for(int i = 0; i < piano_keys.size(); i++) {
            piano_keys[i]->system_reset();
        }
        soundboard_.system_reset();
    }
    
    ~PianoModel() noexcept {
        for(int i = 0; i < piano_keys.size(); i++) {
            delete piano_keys[i];
        }
    }
};

#endif /* piano_model_hpp */
