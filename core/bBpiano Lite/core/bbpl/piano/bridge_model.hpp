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
// 2026-09-15
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

#ifndef bridge_model_hpp
#define bridge_model_hpp

#include "./soundboard_model.hpp"
#include "../configuration/configuration.hpp"

class BridgeModel {
    
    const int midi_n_ = 21;
    SoundboardModel *soundboard_;
    
    float vb = 0.0f;
    
    std::array<float, Parameters::Bridge::TransmissionFIR::kFirTapCount> state_x_{};
    std::array<float, Parameters::Bridge::TransmissionFIR::kFirTapCount> state_y_{};
    int head_ = 0;
    
    const std::array<double, Parameters::Bridge::TransmissionFIR::kFirTapCount>* coefficients_;
    
    static constexpr int k_group_delay = (Parameters::Bridge::TransmissionFIR::kFirTapCount - 1) / 2;
    
    
public:
    
    BridgeModel (const int midi_n,
                 const std::array<double, Parameters::Bridge::TransmissionFIR::kFirTapCount>* coefficients,
                 SoundboardModel *soundboard) :
        midi_n_(midi_n),
        coefficients_(coefficients),
        soundboard_(soundboard)
    {}
    
    inline void vb_update_filter() {
        vb = 0.0f;
        int index = head_;
        if(index < 0) {
            index += Parameters::Bridge::TransmissionFIR::kFirTapCount;
        }
        for(int i = 0; i < Parameters::Bridge::TransmissionFIR::kFirTapCount; i++) {
            vb += static_cast<float>((*coefficients_)[i]) * state_y_[index];
            index--;
            if(index < 0) {
                index += Parameters::Bridge::TransmissionFIR::kFirTapCount;
            }
        }
    }
    
    inline void coupling_process(float* v_x, float* v_y) {
        
        add_history(*v_x, *v_y);
        
        vb_update_filter();
        
        int index = head_ - k_group_delay;
        if(index < 0) {
            index += Parameters::Bridge::TransmissionFIR::kFirTapCount;
        }
        
        const float vo = vb - state_y_[index]; // [n - D] ; D == 8

        const float x = -state_x_[index];
        const float y = vo;

        
        *v_x = 0.999848f * x - 0.017452f * y;
        *v_y = 0.017452f * x + 0.999848f * y;
        
    }
    
    inline void add_history(float vi_x, float vi_y) {
        head_++;
        if(head_ > (Parameters::Bridge::TransmissionFIR::kFirTapCount - 1)) {
            head_ -= Parameters::Bridge::TransmissionFIR::kFirTapCount;
        }
        state_y_[head_] = vi_y;
        state_x_[head_] = vi_x;
    }
    
    inline float get_vb() {
        return vb;
    }
    
    inline void system_reset() {
        head_ = 0;
        std::fill(state_y_.begin(), state_y_.end(), 0.0f);
        std::fill(state_x_.begin(), state_y_.end(), 0.0f);
        vb = 0.0f;
    }
    
    inline float get_state_energy() const {
        float energy = 0.0f;

        for (const float v : state_y_) {
            energy += v * v;
        }

        return energy;
    }
    
    inline void update_vb() {
        soundboard_->bridge_velocity[midi_n_ - 21] = vb;
    }
};

#endif /* bridge_model_hpp */
