//
//  HammerModel.cpp
//  bBpiano
//
//  Created by opus arc on 2026/4/6.
//
//  This document is not AI-assisted.
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
    if (!pairedString_a) return;

    // 1. 读弦速度
    string_v = pairedString_a->velocityAt(strikePoint);

    // 2. 算压缩速度
    dv = computeCompressionSpeed(string_v);

    // 3. 更新压缩量
    updateDy();

    // 4. 算接触力
    F = computeForce();
    
    // 5. 算高斯分布力
    int strikePoint_index = std::floor(strikePoint * pairedString_a->N_index); // 将击弦点从比例转换为索引
    sigma = computeSigma();
    int sigma_int = std::max(1, int(std::ceil(sigma))); // 计算 sigma 的 int 值，向上取整
    int start = std::max(0, strikePoint_index - 3 * sigma_int);
    int end   = std::min(pairedString_a->N_index, strikePoint_index + 3 * sigma_int);
    std::vector<float> string_F = computeGaussianForce(start, end);

    // 6. 把力注入弦
    injectForce(string_F, start, end);
    
    // 7. 反作用力减慢锤子
    v_in -= computeReactionForce();
    
}

double HammerModel::computeCompressionSpeed(double _string_v){
    
    // 锤子向着击弦点的相对速度
    float delta_v = v_in - pairedString_a->velocityAt(strikePoint);
    
    // 上一次力对锤子的反作用力
    float last_impact = F_Last / (2 * pairedString_a->Z);

    return delta_v - last_impact;
}

void HammerModel::updateDy() {
    dy += dv * pairedString_a->Ts;

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
	
std::vector<float> HammerModel::computeGaussianForce(int start, int end){
    // 创造一个和弦模型一样长的数组
    std::vector<float> string_F;
    string_F.assign(pairedString_a->N_int, 0.0);
    
    // 将击弦点从比例转换为索引
    int strikePoint_index = std::floor(strikePoint * pairedString_a->N_index);

    // 防一下
    if (sigma <= 0.0f) {
        return string_F;
    }
    
    // 将分布写入数组，并数组求和
    double sum = 0;
    for(int i = start; i <= end; i++) {
        double dx = i - strikePoint_index; // 算距离
        double exponent = -(dx * dx) / (2 * sigma * sigma); // 算指数
        string_F[i] = std::exp(exponent);
        sum += string_F[i];
    }
    
    // 我再防
    if (sum <= 0.0) {
        return string_F;
    }
    
    // 算出比例
    double scale = F / sum;
    
    // 将比例带入数组
    for(int i = start; i <= end; i++) {
        string_F[i] *= scale;
    }
    
    
    return string_F;
}
    
double HammerModel::computeReactionForce(){
    return (F / m) * pairedString_a->Ts;
}

void HammerModel::injectForce(std::vector<float>& string_F, int start, int end){
    
    for(int i = start; i <= end; i++) {
        pairedString_a->injectForce(i, static_cast<float>(string_F[i] / 3));
        pairedString_b->injectForce(i, static_cast<float>(string_F[i] / 3));
        pairedString_c->injectForce(i, static_cast<float>(string_F[i] / 3));
    }
    
}

double HammerModel::computeSigma(){
    return sigmaCoeff * std::sqrt(dy);
}

void HammerModel::setVIn(double _v_in){
    v_in = _v_in;
}
