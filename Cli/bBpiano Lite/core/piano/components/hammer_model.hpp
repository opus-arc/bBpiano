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
#include <array>
#include <vector>

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
    // ======================== ========================
    // String Side Felt Layer
    // 弦侧毛毡层
    // ======================== ========================
    struct StringSideFeltLayer {
        
        int string_count = 3;
        std::array<GeneralizedMaxwell, 3> generalized_maxwell_models;
        double force = 0.0;
        std::array<double, 3> force_strings = {0.0, 0.0, 0.0};
        
        StringSideFeltLayer(double ts, int string_count)
            : string_count(string_count),
                generalized_maxwell_models({
                GeneralizedMaxwell(6.6499201608e19,  // K∞ = 566 N/mm^5.69 = 6.6499201608e19 N/m^5.69
                                   3.3249600804e20,  // K = 2830 N/mm^5.69 = 3.3249600804e20 N/m^5.69
                                   1.0e-5,           // relaxation_time_s
                                   0.8,              // beta
                                   5.69,             // p
                                   ts),              // time_step_s
                GeneralizedMaxwell(6.6499201608e19, 3.3249600804e20, 1.0e-5, 0.8, 5.69, ts),
                GeneralizedMaxwell(6.6499201608e19, 3.3249600804e20, 1.0e-5, 0.8, 5.69, ts)}) {
                    if(string_count < 1 || string_count > 3) throw std::runtime_error("hammer_model: string_count is: " + std::to_string(string_count));
        }
        
        inline void movement(const std::array<double, 3>& string_xs) {
            
            force = 0.0;
            std::fill(force_strings.begin(), force_strings.end(), 0.0);
            
            for(int i = 0; i < string_count; i++) {
                generalized_maxwell_models[i].movement(string_xs[i]);
                force_strings[i] = generalized_maxwell_models[i].get_force();
                force += force_strings[i];
            }
        }
        
        inline double get_force() {
            return force;
        }
        
        // ======================== ========================
        // Constant operator
        // 常数算子
        // ======================== ========================
        inline double get_a() {
            double a = 0;
            for(int i = 0; i < string_count; i++) {
                a += generalized_maxwell_models[i].get_a();
            }
            return a;
        }
    };
    
    // ======================== ========================
    // Core Side Felt Layer
    // 木质击锤核心侧毛毡层
    // ======================== ========================
    struct CoreSideFeltLayer {
        MaxwellModel maxwell_model_a;
        MaxwellModel maxwell_model_b;
        double k = 0.0;
        double force = 0.0;
        
        CoreSideFeltLayer(double ts)
            : maxwell_model_a(
                  8.52e9,   // K = 8520 N/mm² = 8.52e9 N/m²
                  1.0e-5,   // relaxation_time_s
                  0.5,      // beta_1
                  2.0,      // p
                  ts),      // time_step_s
              maxwell_model_b(8.52e9, 5.0e-6, 0.2, 2.0, ts), k(2.556e9) {
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
    
private:
    
    int string_count = 3;

    double hammer_force = 0.0;
    
    double samplerate = 44100.0;
    double ts = 1 / samplerate;
    
    double hammer_m_kg = 9.12e-3;
    double hammer_a_mps2 = 0.0;
    double hammer_v_mps = 0.0;
    
    StringSideFeltLayer string_side_felt_layer;
    CoreSideFeltLayer core_side_felt_layer;
    double middle_v = 0.0;
    std::array<double, 3> string_side_felt_layer_xs = {0.0, 0.0, 0.0};
    double core_side_felt_layer_x = 0.0;
    
public:
    
    HammerModel(double samplerate, int string_count) :
    samplerate(samplerate),
    string_count(string_count),
    string_side_felt_layer(1 / samplerate, string_count),
    core_side_felt_layer(1 / samplerate) {
        if(string_count < 1 || string_count > 3) throw std::runtime_error("hammer_model: string_count is: " + std::to_string(string_count));
    }
    
    inline void hammer_lanuch(double hammer_v_lanuch) {
        hammer_v_mps = hammer_v_lanuch;
    }
    
    inline void hammer_movement(const std::vector<double>& string_vs) {
        if(string_vs.size() != string_count)
            throw std::runtime_error("hammer_model: string_count has problems");
        
        // ======================== ========================
        // Solve for the velocity of the intermediate layer
        // 解出中间层速度
        // ======================== ========================
        middle_v = solve_middle_v(string_vs,
                                  hammer_v_mps);
        
        // ======================== ========================
        // Calculate the compression
        // 计算压缩量
        // ======================== ========================
        for(int i = 0; i < string_count; i++)
            string_side_felt_layer_xs[i] = (middle_v - string_vs[i]) * ts + string_side_felt_layer.generalized_maxwell_models[i].maxwell_model.x_1;
        core_side_felt_layer_x = (hammer_v_mps - middle_v) * ts + core_side_felt_layer.maxwell_model_a.x_1;
        
        // ======================== ========================
        // Movements of two layers
        // 上下层的运动
        // ======================== ========================
        string_side_felt_layer.movement(string_side_felt_layer_xs);
        
        core_side_felt_layer.movement(core_side_felt_layer_x);
        
        // ======================== ========================
        // Update the force
        // 更新力
        // ======================== ========================
        hammer_force = core_side_felt_layer.get_force();
        
        // ======================== ========================
        // Reaction force on the hammer
        // 对击锤的反作用力
        // ======================== ========================
        hammer_a_mps2 = -(core_side_felt_layer.force / hammer_m_kg);
        hammer_v_mps += hammer_a_mps2 * ts;
    }
    
    inline std::array<double, 3> get_force_strings() {
        return string_side_felt_layer.force_strings;
    }
    
private:

    inline double solve_middle_v(const std::vector<double>& string_vs, double hammer_v) {
        double a = string_side_felt_layer.get_a();
        double e = core_side_felt_layer.get_e();
        double f = core_side_felt_layer.get_f();
        double g = hammer_v + core_side_felt_layer.get_g_part();
        double h = core_side_felt_layer.get_h();
        
        // ======================== ========================
        // Calculate the search range
        // 计算搜索范围
        // ======================== ========================
        double upper_limit_i = g;
        double lower_limit_i = string_vs[0] - string_side_felt_layer.generalized_maxwell_models[0].get_c_part();
        for (int i = 1; i < string_count; ++i) {
            double c =
                string_vs[i] -
                string_side_felt_layer.generalized_maxwell_models[i].get_c_part();

            lower_limit_i = std::max(lower_limit_i, c);
        }
        
        double middle_v_i = (upper_limit_i + lower_limit_i) / 2;
        
        // ======================== ========================
        // Standard bisection method check
        // 标准二分法检查 a > b
        // ======================== ========================
        if (lower_limit_i > upper_limit_i) {
            throw std::runtime_error("hammer_model: lower_limit_i > upper_limit_i, lower_limit: " + std::to_string(lower_limit_i) + ", upper_limit_i: " + std::to_string(upper_limit_i));
        }

        // ======================== ========================
        // Bisection method
        // 二分法计算
        // ======================== ========================
        constexpr int cycle_count = 24;
        
        for(int i = 1; i <= cycle_count; i++) {
            middle_v_i = (lower_limit_i + upper_limit_i) * 0.5;
            
            double f_a_part = 0.0;
            for(int i = 0; i < string_count; i++) {
                double b = string_side_felt_layer.generalized_maxwell_models[i].get_b();
                double c = string_vs[i] - string_side_felt_layer.generalized_maxwell_models[i].get_c_part();
                double d = string_side_felt_layer.generalized_maxwell_models[i].get_d();
                f_a_part += b * std::pow(middle_v_i - c, d);
            }
            
            double f_a = a + f_a_part;
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

