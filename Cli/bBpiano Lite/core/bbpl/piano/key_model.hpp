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

class KeyModel {
    
    // 弦锤耦合与tunning
    
public:
    
    double samplerate = 0.0;
    
    int midi_n;
    int string_count = 3;
    
    HammerModel hammer;
    std::array<StringModel, 3> strings;
    
    std::array<double, 3> string_vs = {0.0, 0.0, 0.0};
    
    bool key_down = false;
    bool key_active = false;
    
    KeyModel(int midi_n, double sample_rate, int string_count) :
        midi_n(midi_n),
        string_count(string_count),
        samplerate(sample_rate),
        hammer(sample_rate),
        strings{StringModel(sample_rate, 438.0),
        StringModel(sample_rate, 440.0),
        StringModel(sample_rate, 442.0)} {
        
    }
    
    void key_movement() {

        for(int i = 0; i < string_count; i++) {
            string_vs[i] = strings[i].get_string_vs();
        }
        
        double hammer_force = hammer.hammer_movement(string_vs[0]);
        
        std::cout << "hammer_force: " << hammer_force << "\n";
        
        for(int i = 0; i < string_count; i++) {
            strings[i].string_movement(hammer_force / double(string_count));
        }
        
    }
    
    void trigger(double velocity_mps) {
        hammer.trigger(velocity_mps);
    }
    
    float get_sample() {
        float result = 0.0;
        for(auto& string : strings) {
            result += string.get_sample();
        }
        return result;
    }
    
};

#endif /* key_model_hpp */
