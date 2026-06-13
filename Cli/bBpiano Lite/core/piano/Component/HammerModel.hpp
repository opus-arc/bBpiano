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

#include "StringModel.hpp"

class KeyModel;

class HammerModel {
    
public:
    // --------------------------------------------
    // MARK: 常量
    
    // Chaigne-Askenfelt 1994b C4 hammer 参数
    // 锤毡刚度系数
    static constexpr double K = 4.5e9;
    // 非线性指数
    static constexpr double P = 2.5;
    static constexpr double m = 0.00297;
    
    // 击弦点比例。构造时会转换成 strikeM，实时路径不再重复乘 delay_index。
    static constexpr double strikePoint = 0.12;
    
    // --------------------------------------------
    // MARK: 组件与基本信息
    
    explicit HammerModel(KeyModel *_pairedKey, int _midi_n);
    
    const KeyModel *pairedKey = nullptr;
    
    const int midi_n;
    const int string_count;
    
    StringModel* pairedString_a = nullptr;
    StringModel* pairedString_b = nullptr;
    StringModel* pairedString_c = nullptr;
    
    // Hammer-P 固定击弦格点。这里是相对波导下标，不是 vector 绝对下标。
    int strikeM = 1;
    
    // Hammer-P 以弦采样率两倍运行，所以每个子步是 Ts / 2。
    double hammerTs = 0.0;
    
    // --------------------------------------------
    // MARK: 状态值
    //  State 运行时会被反复修改的值
    
    // 对应的弦是否有能量
    mutable double strings_active = false;
    
    // active 弦检测剪枝计数器
    mutable int activityCounter = 0;
    
    // 接触速度
    double v_in = 0.0;
    
    // 压缩距离
    double dy = 0.0;
    
    // 压缩速度
    double dv = 0.0;
    
    // 接触力
    double F = 0.0;
    
    // 上一次的接触力
    double F_Last = 0.0;
    
    // --------------------------------------------
    // MARK: 计算函数
    
    void setVIn(double _v_in);
    
    // Hammer-P 子步力更新。_string_v 是当前子步看到的多弦总反馈速度。
    double hammerPHalfStepForce(double _string_v, double _dt);
    
    // 把 Hammer-P 的总接触力按弦数平均后做错位单点注入。
    void distributeHammerForce(int M, double _F);
    
    void moveStrings();
    
    // --------------------------------------------
    // MARK: 运动帧
    
    // 锤子的运动回合, 每帧的调用接口
    void hammerMovement();
    
    float getSample();
    
    void setInactive();
};

#endif /* Hammer_hpp */
