//
//  StringModel.hpp
//  bBpiano
//
//  Created by opus arc on 2026/4/6.
//
//  This document is not AI-assisted.
//

#ifndef String_hpp
#define String_hpp

#include "../../ModelParameters/ModelParameters.hpp"

#include <iostream>
#include <algorithm>
#include <vector>
#include <cmath>

class HammerModel;

struct LossConstant;

class StringModel {

    // --------------------------------------------
    // MARK: 常量
    
private:
    
    // 采样率
    private: static constexpr double sampleRate = 44100.0;
        
    // 弦的张力
    // 单位 牛顿
    // 这里暂时用一个固定值
    static constexpr double T = 850.0;
    
    // 弦的线密度
    // 单位 kg / m
    // 这里暂时用一个固定值
    static constexpr double rho = 0.006;
    
    // 反射衰减
    // 按理说 loss × dispersion × fractional delay
    // 但这里先用常数
    static constexpr double g = 0.996;
    
    
    // --------------------------------------------
    // MARK: 组件与基本信息
    //  初始化后固定的内容
    
public:
    
    // 初始化
    // explicit 禁止隐式转换带来的语义污染
    explicit StringModel(HammerModel *_pairedHammer, int _midi_n, int _stringNum);
    
    // 配对的击锤
    const HammerModel *pairedHammer = nullptr;
    
    // midi 号码
    const int midi_n;
    
    // 弦的编号
    const int string_index;
        
    
    // --------------------------------------------
    // MARK: 实时值与其函数
    //  Derived Value（派生量） + Lazy Evaluation（惰性计算） + Cache（缓存）
    
public:
    
    // 依赖 midi_n, reference_tone, temperament 计算 f0
    // 只与 ModelParameters::shared->tuning->version 有关
    float get_f0() const;
    mutable float cache_f0;
    
    
    // --------------------------------------------
    // MARK: 状态值
    //  State 运行时会被反复修改的值
    
public:
    
    // 这根弦的振动能量达到一个 epcilon 以上
//    mutable bool active = false;
    
    mutable std::vector<float> leftNext;
    mutable std::vector<float> rightNext;
    
    // 时间步长
    double Ts = 0;
    
    // 波导长度
    double Delay = 0.0;
    int Delay_Int = 0;
    int Delay_Index = 0;
    double Delay_Frac = 0.0;// 波导长度小数部分
    double Allpass_A1 = 0.0;
    mutable float FractionalAllpass_X1_r = 0.0; // 上一次传入 allpass 的值
    mutable float FractionalAllpass_Y1_r = 0.0;
    mutable float FractionalAllpass_X1_l = 0.0; // 上一次传入 allpass 的值
    mutable float FractionalAllpass_Y1_l = 0.0;
    
    // loss filter
    std::vector<LossConstant> lossConstants;
    mutable float Loss_X1_r = 0.0; // 上一次传入 allpass 的值
    mutable float Loss_Y1_r = 0.0;
    mutable float Loss_X1_l = 0.0; // 上一次传入 allpass 的值
    mutable float Loss_Y1_l = 0.0;
    double loss_a1 = -0.01;
    double loss_g = 0.999293;

    // loss filter
    mutable float Dispersion_X1_r = 0.0; // 上一次传入 allpass 的值
    mutable float Dispersion_Y1_r = 0.0;
    mutable float Dispersion_X1_l = 0.0; // 上一次传入 allpass 的值
    mutable float Dispersion_Y1_l = 0.0;
    double Dispersion_A1 = 0.0;

    
    // 力 ↔ 速度 的比例常数
    double Z = 0.0;
    
    // 波的实时离散值
    mutable std::vector<float> left;
    mutable std::vector<float> right;
    
    
    // --------------------------------------------
    // MARK: 计算函数

public:
    
    // 注入
    //  将力变成波
    void injectForce(double p, float F) const ;
    void injectForce(int m, float F) const ;
    
    float processLoss(float x, float& y1) const;
    
    // 传播
    void propagate() const ;
    
    // --------------------------------------------
    // MARK: 运动帧
    
public:
    
    // 弦的运动回合，每帧的调用接口
    void stringMovement() const ;
    
    // 获取速度的方式
    float velocityAt(double p) const ;
    float nextVelocityAt(double p) const ;
    
    float activityProbe() const;
};





#endif /* String_hpp */
