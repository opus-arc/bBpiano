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
    static constexpr double rho = 0.002;
    
    // 反射衰减
    // 按理说 loss × dispersion × fractional delay
    // 但这里先用常数
    static constexpr double g = 0.995;
    
    
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
    float compute_f0() const;
    mutable float cache_f0;
    mutable float f0_cached_version = 0;
    
    
    // --------------------------------------------
    // MARK: 状态值
    //  State 运行时会被反复修改的值
    
public:
    
    // 这根弦的振动能量达到一个 epcilon 以上
    mutable bool active = false;
    
    // active 弦检测剪枝计数器
    mutable int activityCounter = 0;
    
    mutable std::vector<float> leftNext;
    mutable std::vector<float> rightNext;
    
    // 时间步长
    double Ts;
    
    // 波导长度
    double N;
    int N_int;
    int N_index;

    // 力 ↔ 速度 的比例常数
    double Z;
    
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
    
    // 传播
    void propagate() const ;

    
    float energy() const;
    bool isActive() const;
    void updateActivity() const;
    
    // --------------------------------------------
    // MARK: 运动帧
    
public:
    
    // 弦的运动回合，每帧的调用接口
    void stringMovement() const ;
    
    // 获取速度的两种方式
    float velocityAt(double p) const ;
    
};

#endif /* String_hpp */
