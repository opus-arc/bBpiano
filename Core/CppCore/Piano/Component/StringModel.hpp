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

#include <iostream>
#include <algorithm>
#include <vector>
#include <cmath>

class HammerModel;

class StringModel {
    
public:
    // --------------------------------------------
    // MARK: 常量
    
    // 采样率
    double sampleRate = 44100.0;
        
    // 弦的张力
    // 单位 牛顿
    // 这里暂时用一个固定值
    double T = 850.0;
    
    // 弦的线密度
    // 单位 kg / m
    // 这里暂时用一个固定值
    double rho = 0.002;
    
    // 反射衰减
    // 按理说 loss × dispersion × fractional delay
    // 但这里先用常数
    double g = 0.995;
    
    // --------------------------------------------
    // MARK: 传入值
    
    // 配对的击锤
    HammerModel *pairedHammer = nullptr;
    
    // 基频
    double f0;
    
    // --------------------------------------------
    // MARK: 计算值
    
    // 时间步长
    double Ts;
    
    // 音域
    double vocalRange;
    
    // 波导长度
    double N;
    int N_int;
    int N_index;

    // 力 ↔ 速度 的比例常数
    double Z;
    
    // 波的实时离散值
    std::vector<float> left;
    std::vector<float> right;
    
    // --------------------------------------------
    // MARK: 运动帧
    
    // 弦的运动回合，每帧的调用接口
    void stringMovement();
    
    
    // --------------------------------------------
    // MARK: 初始化
    
    
    StringModel(double f0);
    
    
    
    // --------------------------------------------
    // MARK: 函数
    
    // 注入
    //  将力变成波
    void injectForce(double p, float F);
    
    // 传播
    void propagate();
    
    // 获取速度的两种方式
    float velocityAt(double p);
    
};

#endif /* String_hpp */
