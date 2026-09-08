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

#ifndef hammer_model_hpp
#define hammer_model_hpp

#include <iostream>
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <string>

class StringModel;

class HammerModel {
    
    
public:
    
    // ======================== ======================== ========================
    // Components
    // 元件
    // ======================== ======================== ========================
    // ======================== ========================
    // Basic Maxwell
    // 基础 Maxwell 单件
    // ======================== ========================
    struct MaxwellModel {
        
        double k = 0.0;
        double relaxation_time = 0.0; // relaxation_time_s
        double beta = 0.0; // weight
        double p = 0.0; // nonlinearity
        
        double ts = 0.0;
        
        double x = 0.0;
        double x_1 = 0.0;
        
        double force = 0.0;
        double force_1 = 0.0;
        
        // TODO: 压缩量是否会超出界限？
        
        MaxwellModel(double k, double relaxation_time, double beta, double p, double ts) :
        k(k), relaxation_time(relaxation_time), beta(beta), p(p), ts(ts) {
            if (k <= 0.0 || relaxation_time <= 0.0 || beta <= 0.0 || p <= 0.0 || ts <= 0.0)
                throw std::runtime_error("MaxwellModel: some constants are too small!");
        }
        
        inline void movement(double _x) {
            x = _x;
            force = (1 - ts / relaxation_time) * force_1 + k * beta * (std::pow(x, p) - std::pow(x_1, p));
            x_1 = x;
            force_1 = force;
        }
        
        inline double get_force() {
            return force;
        }
        inline double get_e() {
            return (1 - ts / relaxation_time) * force_1 - beta * k * std::pow(x_1, p);
        }
        inline double get_f() {
            return std::pow(ts, p) * k * beta;
        }
        inline double get_g_part() {
            return x_1 / ts;
        }
        inline double get_h() {
            return p;
        }
    
    };
    // ======================== ========================
    // General Maxwell
    // 广义 Maxwell 单件
    // ======================== ========================
    struct GeneralizedMaxwell {
        
        MaxwellModel maxwell_model;
        double k = 0.0;
        double force = 0.0;
        
        GeneralizedMaxwell(double k_spring, double k, double relaxation_time, double beta, double p, double ts)
        : k(k_spring) , maxwell_model(k, relaxation_time, beta, p, ts) {}
        
        inline void movement(double _x) {
            maxwell_model.movement(_x);
            force = k * std::pow(_x, maxwell_model.p) + maxwell_model.get_force();
        }
        
        inline double get_force() {
            return force;
        }
        
        // ========================
        // Constant operator
        // 常数算子
        // ========================
        inline double get_a() {
            return (1 - maxwell_model.ts / maxwell_model.relaxation_time) * maxwell_model.force_1 -
            maxwell_model.beta * maxwell_model.k * std::pow(maxwell_model.x_1, maxwell_model.p);
        }
        inline double get_b() {
            return std::pow(maxwell_model.ts, maxwell_model.p) * (maxwell_model.k * maxwell_model.beta + k);
        }
        inline double get_c_part() {
            return maxwell_model.x_1 / maxwell_model.ts;
        }
        inline double get_d() {
            return maxwell_model.p;
        }
        
    };
    
    // ======================== ======================== ========================
    // Layers
    // 层
    // ======================== ======================== ========================
    struct StringSideFeltLayer {
        GeneralizedMaxwell generalized_maxwell_model_a;
        GeneralizedMaxwell generalized_maxwell_model_b;
        GeneralizedMaxwell generalized_maxwell_model_c;
        double force = 0.0;
        
        StringSideFeltLayer(double ts)
            : generalized_maxwell_model_a(
                  6.6499201608e19,  // K∞ = 566 N/mm^5.69
                                   //    = 6.6499201608e19 N/m^5.69

                  3.3249600804e20,  // K = 2830 N/mm^5.69
                                   //   = 3.3249600804e20 N/m^5.69

                  1.0e-5,           // relaxation_time_s

                  0.8,              // beta，无单位

                  5.69,             // p，无单位

                  ts),              // time_step_s

              generalized_maxwell_model_b(
                  6.6499201608e19,   // N/m^5.69
                  3.3249600804e20,   // N/m^5.69
                  1.0e-5,            // s
                  0.8,               // 无单位
                  5.69,              // 无单位
                  ts),               // s

              generalized_maxwell_model_c(
                  6.6499201608e19,   // N/m^5.69
                  3.3249600804e20,   // N/m^5.69
                  1.0e-5,            // s
                  0.8,               // 无单位
                  5.69,              // 无单位
                  ts) {              // s
        }
        
        inline void movement(double string_a_x, double string_b_x, double string_c_x) {
            generalized_maxwell_model_a.movement(string_a_x);
            generalized_maxwell_model_b.movement(string_b_x);
            generalized_maxwell_model_c.movement(string_c_x);
            
            force = generalized_maxwell_model_a.get_force() +
            generalized_maxwell_model_b.get_force() +
            generalized_maxwell_model_c.get_force();
        }
        inline double get_force() {
            return force;
        }
        
        // ======================== ========================
        // Constant operator
        // 常数算子
        // ======================== ========================
        inline double get_a() {
            return generalized_maxwell_model_a.get_a() +
            generalized_maxwell_model_b.get_a() +
            generalized_maxwell_model_c.get_a();
        }
        inline double get_b1() {
            return generalized_maxwell_model_a.get_b();
        }
        inline double get_b2() {
            return generalized_maxwell_model_b.get_b();
        }
        inline double get_b3() {
            return generalized_maxwell_model_c.get_b();
        }
        inline double get_c1_part() {
            return generalized_maxwell_model_a.get_c_part();
        }
        inline double get_c2_part() {
            return generalized_maxwell_model_b.get_c_part();
        }
        inline double get_c3_part() {
            return generalized_maxwell_model_c.get_c_part();
        }
        inline double get_d1() {
            return generalized_maxwell_model_a.get_d();
        }
        inline double get_d2() {
            return generalized_maxwell_model_b.get_d();
        }
        inline double get_d3() {
            return generalized_maxwell_model_c.get_d();
        }
    };
    
    struct CoreSideFeltLayer {
        MaxwellModel maxwell_model_a;
        MaxwellModel maxwell_model_b;
        double k = 0.0;
        double force = 0.0;
        
        CoreSideFeltLayer(double ts)
            : maxwell_model_a(
                  8.52e9,   // K = 8520 N/mm²
                             //   = 8.52e9 N/m²

                  1.0e-5,   // relaxation_time_s

                  0.5,      // beta_1，无单位

                  2.0,      // p，无单位

                  ts),      // time_step_s

              maxwell_model_b(
                  8.52e9,   // N/m²

                  5.0e-6,   // relaxation_time_s

                  0.2,      // beta_2，无单位

                  2.0,      // p，无单位

                  ts),      // time_step_s

              k(2.556e9) {  // K∞ = 8520 × (1 - 0.5 - 0.2)
                            //    = 2556 N/mm²
                            //    = 2.556e9 N/m²
        }
            
        
        inline void movement(double _x) {
            maxwell_model_a.movement(_x);
            maxwell_model_b.movement(_x);
            
            force = maxwell_model_a.get_force() +
            maxwell_model_b.get_force() +
            k * std::pow(_x, maxwell_model_a.p);
        }
        inline double get_force() {
            return force;
        }
        
        // ======================== ========================
        // Constant operator
        // 常数算子
        // ======================== ========================
        inline double get_e() {
            return maxwell_model_a.get_e() + maxwell_model_b.get_e();
        }
        inline double get_f() {
            return maxwell_model_a
                .get_f() + maxwell_model_b
                .get_f() + k * std::pow(maxwell_model_a.ts, maxwell_model_a.p);
        }
        inline double get_g_part() {
            return maxwell_model_a.get_g_part();
        }
        inline double get_h() {
            return maxwell_model_a.get_h();
        }
    };
    
public:
    double samplerate = 44100.0;
    double ts = 1 / samplerate;
    
    double hammer_force = 0.0;
    
    double hammer_m_kg = 9.12e-3;
    double hammer_a_mms2 = 0.0;
    double hammer_v_mms = 0.0;
    
    StringSideFeltLayer string_side_felt_layer;
    CoreSideFeltLayer core_side_felt_layer;
    double middle_v = 0.0;
    double string_side_felt_layer_a_x = 0.0;
    double string_side_felt_layer_b_x = 0.0;
    double string_side_felt_layer_c_x = 0.0;
    double core_side_felt_layer_x = 0.0;
    StringModel* string = nullptr;
    
public:
    
    HammerModel(double samplerate) : samplerate(samplerate),
    string_side_felt_layer(1 / samplerate), core_side_felt_layer(1 / samplerate) {
    }
    
    inline void hammer_lanuch(double hammer_v_lanuch) {
        hammer_v_mms = hammer_v_lanuch;
    }
    
    inline void hammer_movement(double string_a_v, double string_b_v, double string_c_v) {
        // ======================== ========================
        // Solve for the velocity of the intermediate layer
        // 解出中间层速度
        // ======================== ========================
        middle_v = solve_middle_v(string_a_v,
                                  string_b_v,
                                  string_c_v,
                                  hammer_v_mms);
        
        // ======================== ========================
        // Calculate the compression
        // 计算压缩量
        // ======================== ========================
        string_side_felt_layer_a_x = (middle_v - string_a_v) * ts + string_side_felt_layer.generalized_maxwell_model_a.maxwell_model.x_1;
        string_side_felt_layer_b_x = (middle_v - string_b_v) * ts + string_side_felt_layer.generalized_maxwell_model_b.maxwell_model.x_1;
        string_side_felt_layer_c_x = (middle_v - string_c_v) * ts + string_side_felt_layer.generalized_maxwell_model_c.maxwell_model.x_1;
        
        core_side_felt_layer_x = (hammer_v_mms - middle_v) * ts + core_side_felt_layer.maxwell_model_a.x_1;
        
        // ======================== ========================
        // Movements of two layers
        // 上下层的运动
        // ======================== ========================
        string_side_felt_layer.movement(string_side_felt_layer_a_x,
                                        string_side_felt_layer_b_x,
                                        string_side_felt_layer_c_x);
        
        core_side_felt_layer.movement(core_side_felt_layer_x);
        
        // ======================== ========================
        // Reaction force on the hammer
        // 对击锤的反作用力
        // ======================== ========================
        hammer_a_mms2 = -(core_side_felt_layer.force / hammer_m_kg);
        hammer_v_mms += hammer_a_mms2 * ts;
    }

    inline double solve_middle_v(double string_a_v, double string_b_v, double string_c_v, double hammer_v) {
        double a = string_side_felt_layer.get_a();
        double b1 = string_side_felt_layer.get_b1();
        double b2 = string_side_felt_layer.get_b2();
        double b3 = string_side_felt_layer.get_b3();
        double c1 = string_a_v - string_side_felt_layer.get_c1_part();
        double c2 = string_b_v - string_side_felt_layer.get_c2_part();
        double c3 = string_c_v - string_side_felt_layer.get_c3_part();
        double d1 = string_side_felt_layer.get_d1();
        double d2 = string_side_felt_layer.get_d2();
        double d3 = string_side_felt_layer.get_d3();
        double e = core_side_felt_layer.get_e();
        double f = core_side_felt_layer.get_f();
        double g = hammer_v + core_side_felt_layer.get_g_part();
        double h = core_side_felt_layer.get_h();
        
        double upper_limit_i = g;
        double lower_limit_i = std::max({c1, c2, c3});
        double middle_v_i = (upper_limit_i + lower_limit_i) / 2;
        
        if (lower_limit_i > upper_limit_i) {
            throw std::runtime_error("hammer_model: lower_limit_i > upper_limit_i, lower_limit: " + std::to_string(lower_limit_i) + ", upper_limit_i: " + std::to_string(upper_limit_i));
        }
        
        constexpr int cycle_count = 24;
        
        for(int i = 1; i <= cycle_count; i++) {
            middle_v_i = (lower_limit_i + upper_limit_i) * 0.5;
            double f_a = a +
            b1 * std::pow((middle_v_i - c1), d1) +
            b2 * std::pow((middle_v_i - c2), d2) +
            b3 * std::pow((middle_v_i - c3), d3);
            double f_b = e + f * std::pow(g - middle_v_i, h);
            double diff = f_a - f_b;
            if(diff > 0.0) {
                upper_limit_i = middle_v_i;
            } else if (diff < 0.0) {
                lower_limit_i = middle_v_i;
            } else {
                break;
            }
        }
         
        return (lower_limit_i + upper_limit_i) * 0.5;;
    }
};

#endif /* hammer_model_hpp */

