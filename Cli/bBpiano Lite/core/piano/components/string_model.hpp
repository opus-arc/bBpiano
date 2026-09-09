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

#ifndef string_model_hpp
#define string_model_hpp

#include <iostream>
#include <vector>
#include <stdexcept>
#include <cmath>

class HammerModel;

class StringModel {
    
public:
    
    // ======================== ======================== ========================
    // Basic struct tools
    // 基础结构体工具
    // ======================== ======================== ========================
    // ======================== ========================
    // Spatial port
    // 分数窗口
    // ======================== ========================
    struct SpatialPort {
        int size = 0;
        int max_index = 0;
        double position = 0.0;
        double accurate_index = 0.0;;
        
        int index_a = 0;
        int index_b = 0;
        float weight_a = 1.0f;
        float weight_b = 0.0f;
        
        SpatialPort(int size, double position) : size(size), position(position) {
            if(size < 2)
                throw std::runtime_error("string_model: size is too small, size: " + std::to_string(size));
            if(position > 1.0 || position <= 0.0)
                throw std::runtime_error("string_model: position isn't good, position: " + std::to_string(position));
            max_index = size - 1;
            accurate_index = max_index * position;
            
            index_a = std::ceil(accurate_index);
            index_b = index_a - 1;
            
            weight_a = accurate_index - index_b;
            weight_b = 1 - weight_a;
        }
        
        inline float get_value(std::vector<float>& v, int head) {
            if(v.size() < 2)
                throw std::runtime_error("string_model: size is too small, size: " + std::to_string(v.size()));
            return weight_a * v[get_i(index_a, head)] + weight_b * v[get_i(index_b, head)];
        }
        inline void inject(std::vector<float>& v, float x, int head) {
            if(v.size() < 2)
                throw std::runtime_error("string_model: size is too small, size: " + std::to_string(v.size()));
            v[get_i(index_a, head)] += x * weight_a;
            v[get_i(index_b, head)] += x * weight_b;
        }
        inline int get_i(int real_index, int head) {
            return real_index + head <= max_index ?
                   real_index + head :
                   real_index + head - max_index - 1;
        }
    };
    
    // ======================== ========================
    // Waveguide
    // 波导
    // ======================== ========================
    struct Waveguide {
        int delay_int = 0;
        int traveling_wave_max_index = 0;
        std::vector<float> left;
        std::vector<float> right;
        int left_head = 0;
        int right_head = 0;
        float* left_boundary_point = nullptr;
        float* right_boundary_point = nullptr;
        
        Waveguide(int delay_int) :
            delay_int(delay_int),
            traveling_wave_max_index(delay_int - 1) {
                if(delay_int < 6)
                    throw std::runtime_error("string_model: delay_int is too small: " + std::to_string(delay_int));
                
                left.resize(delay_int, 0.0);
                right.resize(delay_int, 0.0);
                    
                left_head = 0;
                right_head = 0;
                
                left_boundary_point = &left[left_head];
                right_boundary_point = &right[get_i(traveling_wave_max_index, right_head)];
        }
        
        inline void propagate() {
            // ======================== ======================== ========================
            // Reverse moving boundary index
            // 反向移动边界下标
            // ======================== ======================== ========================
            if(left_head == traveling_wave_max_index)
                left_head = 0;
            else
                left_head++;
            if(right_head == 0)
                right_head = traveling_wave_max_index;
            else
                right_head--;
            
            right[get_i(0, right_head)] = -left[get_i(0, left_head)];
            left[get_i(traveling_wave_max_index, left_head)] =
                -right[get_i(traveling_wave_max_index, right_head)];
            
            left_boundary_point = &right[get_i(0, right_head)];
            right_boundary_point = &left[get_i(traveling_wave_max_index, left_head)];
        }
        
        // real_index <= traveling_wave_max_index < delay_int
        // head <= traveling_wave_max_index < delay_int
        // real_index + head <= 2 * traveling_wave_max_index < 2 * delay_int
        // real_index + head - delay_int <= 2 * traveling_wave_max_index - delay_int
        // real_index + head - delay_int <= 2 * traveling_wave_max_index - (traveling_wave_max_index + 1)
        // real_index + head - delay_int <= traveling_wave_max_index - 1
        // real_index + head - traveling_wave_max_index <= traveling_wave_max_index
        inline int get_i(int real_index, int head) {
            return real_index + head <= traveling_wave_max_index ?
                   real_index + head :
            real_index + head - delay_int;
        }
    };
    
public:
    
    int midi_n = 59;
    
    double samplerate = 44100.0;
    double f0 = 440.0;
    
    double delay = 0.0;
    int delay_int = 0;
    double delay_frac = 0.0;
    Waveguide waveguide;
    
    double strike_point = 1.0 / 9.0;
    double pickup_point = 7.0 / 9.0;
    SpatialPort strike_port;
    SpatialPort pickup_port;
    
    double z = 0.0;
    
public:
    
    StringModel(double samplerate, double f0) :
        delay(samplerate / (2 * f0)),
        delay_int(delay - 1),
        delay_frac(delay - delay_int),
        samplerate(samplerate),
        f0(f0),
        strike_port(delay_int, strike_point),
        pickup_port(delay_int, pickup_point),
        waveguide(delay_int)
    {}

    inline float string_movement(double hammer_force) {
        waveguide.propagate();
        return 0.0;
    }
    
    inline void inject_force(double hammer_force) {
        float v = 0.0; // 关于 hammer_force 的函数
        
        strike_port.inject(waveguide.left, v, waveguide.left_head);
        strike_port.inject(waveguide.right, v, waveguide.right_head);
    }
    
    inline float get_sample() {
        return
        pickup_port
            .get_value(waveguide.left, waveguide.left_head) +
        pickup_port
            .get_value(waveguide.right, waveguide.right_head);
    }
    
    inline double get_string_vs() {
        return
        strike_port
            .get_value(waveguide.left, waveguide.left_head) +
        strike_port
            .get_value(waveguide.right, waveguide.right_head);
    }
    
    inline float* get_left_boundary_point() {
        return waveguide.left_boundary_point;
    }
    inline float* get_right_boundary_point() {
        return waveguide.right_boundary_point;
    }
    
    inline int get_i(int real_index, int max_index, int head) {
        return real_index + head <= max_index ?
               real_index + head :
               real_index + head - delay_int;
    }
    
};

#endif /* string_model_hpp */
