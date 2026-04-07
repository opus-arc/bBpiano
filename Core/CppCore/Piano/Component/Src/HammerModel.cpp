//
//  HammerModel.cpp
//  bBpiano
//
//  Created by opus arc on 2026/4/6.
//

#include "HammerModel.hpp"

HammerModel::HammerModel(double _v_in)
: v0(_v_in),
  v_in(_v_in),
  string_v(0.0),
  dy(0.0),
  dv(0.0),
  F(0.0),
  F_Last(0.0) {
      
}

void HammerModel::hammerMovement() {
    if (!pairedString) return;

    // 1. 读弦速度
    string_v = pairedString->velocityAt(strikePoint);

    // 2. 算压缩速度
    dv = computeCompressionSpeed(string_v);

    // 3. 更新压缩量
    updateDy();

    // 4. 算接触力
    F = computeForce();

    // 5. 反作用力减慢锤子
    v_in -= (F / m) * pairedString->Ts;

    // 6. 把力注入弦
    pairedString->injectForce(strikePoint, static_cast<float>(F));
}

double HammerModel::computeCompressionSpeed(double _string_v){
    
    // 锤子向着击弦点的相对速度
    float delta_v = v_in - pairedString->velocityAt(strikePoint);
    
    // 上一次力对锤子的反作用力
    float last_impact = F_Last / (2 * pairedString->Z);

    return delta_v - last_impact;
}

void HammerModel::updateDy() {
    dy += dv * pairedString->Ts;

    if (dy < 0.0) {
        dy = 0.0;
    }
}

double HammerModel::computeForce() {
    F_Last = F;

    if (dy <= 0.0) {
        F = 0.0;
        return F;
    }

    F = K * std::pow(dy, P);
    return F;
}
    

