//
//  HammerModel.hpp
//  bBpiano
//
//  Created by opus arc on 2026/4/6.
//

#ifndef Hammer_hpp
#define Hammer_hpp

#include <iostream>
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
    
    // --------------------------------------------
    // MARK: 函数
    
    HammerModel(double _v_in);
    
    // 锤子的运动回合, 每帧的调用接口
    void hammerMovement();
    
    // 计算压缩速度
    double computeCompressionSpeed(double _string_v);
    
    // 更新压缩量
    void updateDy();
    
    // 计算力的大小
    double computeForce();
    
};


#endif /* Hammer_hpp */
