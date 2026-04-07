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


class HammerModel {
    
public:
    // --------------------------------------------
    // MARK: 常量
    
    // 锤毡刚度系数
    double K = 1e9;
    
    // 非线性指数
    double P = 2.3;
    
    //锤子质量
    double m = 0.003; // kg
    
    // 击弦点
    double strikePoint = 0.14;
    
    // hammer 材料指数
    double k_sigma = 1.5;
    
    
    // --------------------------------------------
    // MARK: 传入值
    
    // 绑定的 String
    StringModel* pairedString = nullptr;
    
    // 初速度
    double v0;
    
    // 接触速度
    double v_in;
    
    
    // --------------------------------------------
    // MARK: 计算值
    
    // 弦速度
    double string_v;	

    // 压缩距离
    double dy;
    
    // 压缩速度
    double dv;
    
    // 接触力
    double F;
    
    // 上一次的接触力
    double F_Last;
    
    // 力分布的尺度参数
    double sigma;
    
    // --------------------------------------------
    // MARK: 运动帧
    
    // 锤子的运动回合, 每帧的调用接口
    void hammerMovement();
    
    
    // --------------------------------------------
    // MARK: 初始化
    
    
    HammerModel(double _v_in);
    
    
    // --------------------------------------------
    // MARK: 函数
    
    // 计算压缩速度
    double computeCompressionSpeed(double _string_v);
    
    // 更新压缩量
    void updateDy();
    
    // 计算力的大小
    double computeForce();
    
    // 计算力的分布尺度参数，Hertz 接触
    double computeSigma();
    
    // 计算高斯分布
    std::vector<float> computeGaussianForce(int start, int end);
    
    // 注入力
    void injectForce(std::vector<float>& string_F, int start, int end);
    
    // 根据反作用力更新锤子的速度
    double computeReactionForce();
    
    
};


#endif /* Hammer_hpp */
