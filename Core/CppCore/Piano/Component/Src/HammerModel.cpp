//
//  HammerModel.cpp
//  bBpiano
//
//  Created by opus arc on 2026/4/6.
//
//  This document is not AI-assisted.
//


#include "HammerModel.hpp"

// ------------------------------------------------------------------------------------------
// MARK: 初始化
HammerModel::HammerModel(KeyModel *_pairedKey, int _midi_n) :

    pairedKey(_pairedKey),
    midi_n(_midi_n),
    string_count(midi_n <= 52 ? 2 : 3)

{
    
    
    pairedString_a = new StringModel(this, midi_n, 1);
    pairedString_b = new StringModel(this, midi_n, 2);
    pairedString_c = new StringModel(this, midi_n, 3);
    
      
}

// ------------------------------------------------------------------------------------------
// MARK: 整合 Sample
float HammerModel::getSample(){
    
    if(string_count == 2) {
        return pairedString_a->velocityAt(0.7) + pairedString_b->velocityAt(0.7);
    } else {
        return pairedString_a->velocityAt(0.7) + pairedString_b->velocityAt(0.7) + pairedString_c->velocityAt(0.7);
    }
    
    return pairedString_a->velocityAt(0.7);

}

// ------------------------------------------------------------------------------------------
// MARK: 运动帧
void HammerModel::hammerMovement() {
    if (!pairedString_a) return;
    
    // 取一半的步长
    double half_Ts = pairedString_a->Ts / 2.0;

    // 读弦速度
    double string_v1 = pairedString_a->velocityAt(strikePoint);
    double string_v2 = pairedString_a->nextVelocityAt(strikePoint);

    // 算接触力
    double F1 = hammerHalfStepForce(string_v1, half_Ts); // 上半帧
    double F2 = hammerHalfStepForce(string_v2, half_Ts); // 下半帧
    F = 0.5 * (F1 + F2);
//    std::cout << F << std::endl;
    
    // 算高斯分布力
    int strikePoint_index = std::floor(strikePoint * pairedString_a->N_index); // 将击弦点从比例转换为索引
    sigma = computeSigma();
    int sigma_int = std::max(1, int(std::ceil(sigma))); // 计算 sigma 的 int 值，向上取整
    int start = std::max(0, strikePoint_index - 3 * sigma_int);
    int end   = std::min(pairedString_a->N_index, strikePoint_index + 3 * sigma_int);
    std::vector<float> string_F = computeGaussianForce(start, end);

    // 把力注入弦
    injectForce(string_F, start, end);
    

    
    // 弦移动帧
    if(string_count == 2) {
        pairedString_a->stringMovement();
        pairedString_b->stringMovement();
    }else if (string_count == 3) {
        pairedString_a->stringMovement();
        pairedString_b->stringMovement();
        pairedString_c->stringMovement();
    }
    
    // 检测弦振动
    updateActivity();

    
}

double HammerModel::hammerHalfStepForce(double _string_v, double dt) {
    
    // 锤子向着击弦点的相对速度
    double delta_v = v_in - _string_v;
    
    // 上一次力对锤子的反作用力
    double last_impact = F_Last / (2 * pairedString_a->Z);

    dv = delta_v - last_impact;
    
    // 计算压缩速度
    dy += dv * dt; // 此处用了一半的时间步长

    if (dy < 0.0) {
        dy = 0.0;
    }
    
    // 用上面算好的参数合成力
    if (dy < 0.0) dy = 0.0;

    double F_new = 0.0;
    
    if (dy > 0.0) F_new = K * std::pow(dy, P);
    
    // 反作用力减慢锤子
    v_in -= (F_new / m) * dt;
    
    // 历史力更新成这一个 half-step 的输出
    F_Last = F_new;
    
    return F_new;
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
    

void HammerModel::injectForce(std::vector<float>& string_F, int start, int end){
    
    if(string_count == 2) {
        for(int i = start; i <= end; i++) {
            pairedString_a->injectForce(i, static_cast<float>(string_F[i] / 2));
            pairedString_b->injectForce(i, static_cast<float>(string_F[i] / 2));
        }
    } else if(string_count == 3) {
        for(int i = start; i <= end; i++) {
            pairedString_a->injectForce(i, static_cast<float>(string_F[i] / 3));
            pairedString_b->injectForce(i, static_cast<float>(string_F[i] / 3));
            pairedString_c->injectForce(i, static_cast<float>(string_F[i] / 3));
        }
    }

    
}

double HammerModel::computeSigma(){
    return sigmaCoeff * std::sqrt(dy);
}

void HammerModel::setVIn(double _v_in){
    v_in = _v_in;
}


void HammerModel::updateActivity() const {
    if (++activityCounter >= 128) {
        activityCounter = 0;
        strings_active = pairedString_a->active;
    }
}
