//
//  HammerModel.hpp
//  bBpiano
//
//  Created by opus arc on 2026/4/6.
//
//  This document is not AI-assisted.
//

#ifndef Hammer_hpp
#define Hammer_hpp

#include <iostream>
#include <vector>

#include "StringModel.hpp"

class KeyModel;

class HammerModel {
    
public:
    // --------------------------------------------
    // MARK: 常量
    
    // 锤毡刚度系数
    static constexpr double K = 1.5e9;
    
    // 非线性指数
    static constexpr double P = 2.75;
    
    // 锤子质量
    static constexpr double m = 0.007; // kg
    
    // 击弦点
    // 这个不能和采样点完全一样
    static constexpr double strikePoint = 0.20;
    
    // hammer 材料指数 Hertz 接触
    // 决定这股力在空间上铺多宽
    static constexpr double sigmaCoeff = 6.5;
    
    // --------------------------------------------
    // MARK: 组件与基本信息
    //  初始化后固定的内容
    
    // 初始化
    // explicit 禁止隐式转换带来的语义污染
    explicit HammerModel(KeyModel *_pairedKey, int _midi_n) ;
    
    const KeyModel *pairedKey = nullptr;
    
    // midi 号码
    const int midi_n;
    
    // 弦的数量
    const int string_count;
    
    // Strings
    StringModel* pairedString_a;
    StringModel* pairedString_b;
    StringModel* pairedString_c;

    
    // --------------------------------------------
    // MARK: 实时值与其函数
    //  Derived Value（派生量）, Lazy Evaluation（惰性计算）, Cache（缓存）
    

    
    // --------------------------------------------
    // MARK: 状态值
    //  State 运行时会被反复修改的值
    
    // 对应的弦是否有能量
    mutable double strings_active = false;
    
    // active 弦检测剪枝计数器
    mutable int activityCounter = 0;
    
    // 初速度
//    double v0;
    
    // 接触速度
    double v_in = 0.0;
    
    // 弦速度
//    double string_v = 0.0;

    // 压缩距离
    double dy = 0.0;
    
    // 压缩速度
    double dv = 0.0;
    
    // 接触力
    double F = 0.0;
    
    // 上一次的接触力
    double F_Last = 0.0;
    
    // 力分布的尺度参数
    double sigma = 0.0;

    
    // --------------------------------------------
    // MARK: 计算函数
    
    // 接收按下的速度
    void setVIn(double _v_in);
    
    // 计算半个步长的力的大小
    double hammerHalfStepForce(double _string_v, double _half_Ts);
    
    // 计算力的分布尺度参数，Hertz 接触
    double computeSigma();
    
    // 计算高斯分布
    std::vector<float> computeGaussianForce(int start, int end);
    
    // 注入力
    void injectForce(std::vector<float>& string_F, int start, int end);
    
    
    // --------------------------------------------
    // MARK: 运动帧
    
    // 锤子的运动回合, 每帧的调用接口
    void hammerMovement();
    
    float getSample();
    

    void setInactive();
    
    
};


#endif /* Hammer_hpp */
