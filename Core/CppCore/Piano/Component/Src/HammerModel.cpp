//
//  HammerModel.cpp
//  bBpiano
//
//  Created by opus arc on 2026/4/6.
//
//  This document is not AI-assisted.
//


#include "HammerModel.hpp"
#include "../KeyModel.hpp"



// ------------------------------------------------------------------------------------------
// MARK: 初始化
HammerModel::HammerModel(KeyModel *_pairedKey, int _midi_n) :
    pairedKey(_pairedKey),
    midi_n(_midi_n),
    string_count(midi_n <= 52 ? 2 : 3)

{
    pairedKey = _pairedKey;
    
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

// 管理mode以方便实验
void HammerModel::hammerMovement() {
    if(mode == HammerMode::HammerF) {
        hammerMovementHammerF();
    } else {
        hammerMovementNormal();
    }
}

void HammerModel::hammerMovementNormal() {
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
    int strikePoint_index = std::floor(strikePoint * pairedString_a->delay_index); // 将击弦点从比例转换为索引
    sigma = computeSigma();
    int sigma_int = std::max(1, int(std::ceil(sigma))); // 计算 sigma 的 int 值，向上取整
    int start = std::max(0, strikePoint_index - 3 * sigma_int);
    int end   = std::min(pairedString_a->delay_index, strikePoint_index + 3 * sigma_int);
    std::vector<float> string_F = computeGaussianForce(start, end);

    // 把力注入弦
    injectForce(string_F, start, end);
//    
//    double F_____ = 10.0;
//    if(string_count == 2) {
//        
//            pairedString_a->injectForce(1, static_cast<float>(F_____ / 2));
//            pairedString_b->injectForce(2, static_cast<float>(F_____ / 2));
//        
//    } else if(string_count == 3) {
//        
//            pairedString_a->injectForce(1, static_cast<float>(F_____ / 3));
//            pairedString_b->injectForce(2, static_cast<float>(F_____ / 3));
//            pairedString_c->injectForce(3, static_cast<float>(F_____ / 3));
//        
//    }
    
    
    
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
//    updateActivity();

    
}

void HammerModel::hammerMovementHammerF() {
    if (!pairedString_a) return;
    
    // Hammer-F 第一版只做单弦实验：
    double hammer_Ts = pairedString_a->Ts / 2.0;
    
    // 后续速度读取和力注入都复用同一个 M，避免格点不一致。
    int M = std::floor(strikePoint * pairedString_a->delay_index);
    
    // 第一个 hammer 子步：时间 nTs。
    // v_in,h(nTs) = y+(n, M) + y-(n, M)
    double string_v1 = pairedString_a->velocityAtGrid(M);
    double F1 = hammerFHalfStepForce(string_v1, hammer_Ts);
    
    // 第二个 hammer 子步：时间 nTs + Ts/2。
    // 弦速度使用 Bank 的速度上采样公式：
    // 当前 M 的行波，与下一帧会到达 M 的相邻行波取平均。
    double string_v2 = pairedString_a->velocityAtHalfSample(M);
    double F2 = hammerFHalfStepForce(string_v2, hammer_Ts);
    
    // Bank 对力的下采样：
    // hammer 以 2 倍采样率产生 F1/F2；string 仍以原采样率接收一个力，因此取平均。
    double forceAtStringRate = 0.5 * (F1 + F2);
    
    F = forceAtStringRate;
    
    // Hammer-F 第一版是单点错位注入，不使用 Gaussian sigma。
    sigma = 0.0;
    
    injectHammerFForce(M, forceAtStringRate);
    
    pairedString_a->stringMovement();
}

double HammerModel::hammerHalfStepForce(double _string_v, double dt) {

    if (!std::isfinite(v_in) || !std::isfinite(dy) || !std::isfinite(F_Last)) {
        dy = 0.0;
        dv = 0.0;
        F_Last = 0.0;
        v_in = 0.0;
        return 0.0;
    }

    if (!std::isfinite(_string_v) || !std::isfinite(dt) || dt <= 0.0) {
        return 0.0;
    }

    double Z = pairedString_a->Z;
    if (!std::isfinite(Z) || std::abs(Z) < 1e-12) {
        F_Last = 0.0;
        return 0.0;
    }

    double delta_v = v_in - _string_v;
    double last_impact = F_Last / (2.0 * Z);

    if (!std::isfinite(delta_v) || !std::isfinite(last_impact)) {
        dy = 0.0;
        dv = 0.0;
        F_Last = 0.0;
        return 0.0;
    }

    dv = delta_v - last_impact;
    dy += dv * dt;

    if (!std::isfinite(dy) || dy < 0.0) {
        dy = 0.0;
        dv = 0.0;
        F_Last = 0.0;
        return 0.0;
    }

    double F_new = 0.0;

    if (dy > 0.0) {
        F_new = K * std::pow(dy, P);
    }

    if (!std::isfinite(F_new) || F_new < 0.0) {
        dy = 0.0;
        dv = 0.0;
        F_Last = 0.0;
        return 0.0;
    }

    v_in -= (F_new / m) * dt;

    if (!std::isfinite(v_in)) {
        v_in = 0.0;
        dy = 0.0;
        dv = 0.0;
        F_Last = 0.0;
        return 0.0;
    }

    F_Last = F_new;
    return F_new;
}
	
std::vector<float> HammerModel::computeGaussianForce(int start, int end){
    // 创造一个和弦模型一样长的数组
    std::vector<float> string_F;
    string_F.assign(pairedString_a->delay_int, 0.0);
    
    // 将击弦点从比例转换为索引
    int strikePoint_index = std::floor(strikePoint * pairedString_a->delay_index);

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

double HammerModel::hammerFHalfStepForce(double _string_v, double _dt) {
    
    // Hammer-F / Bank 半步锤子更新。
    // _string_v 是当前 hammer 子步看到的弦速度。
    // _dt = Ts / 2，所以 hammer 以弦采样率的两倍运行。
    
    double Z0 = pairedString_a->Z;
    
    // Bank 结构里的 z^-1 延迟力反馈：
    // 上一个 hammer 子步的力 F_Last 通过 2Z0 转成速度修正项。
    double delayedForceVelocity = F_Last / (2.0 * Z0);
    
    // 锤毡压缩速度：
    // hammer 速度 - 弦速度 - 上一子步力造成的速度反馈。
    dv = (v_in - _string_v) - delayedForceVelocity;
    
    // 更新锤毡压缩量。
    dy += dv * _dt;
    
    // 锤子离开弦时，压缩量不能为负。
    if (dy < 0.0) {
        dy = 0.0;
    }
    
    // 非线性锤毡力：F = K * dy^P。
    double F_new = 0.0;
    if (dy > 0.0) {
        F_new = K * std::pow(dy, P);
    }
    
    // 接触力反作用到锤子，降低锤子速度。
    v_in -= (F_new / m) * _dt;
    
    // 保留 hammer-rate 的最后一个子步力。
    // 下一次 half step 会把它作为 Bank 结构里的延迟反馈。
    F_Last = F_new;
    
    return F_new;
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

void HammerModel::injectHammerFForce(int M, double _F) {
    
    // M 是 Bank 论文里的 M_in，也就是击弦点对应的相对波导格点。
    
    pairedString_a->injectHammerFStaggeredForce(M, static_cast<float>(_F));
}
    

double HammerModel::computeSigma(){
    return sigmaCoeff * std::sqrt(dy);
}

void HammerModel::setVIn(double _v_in){
    v_in = _v_in;
}

//为了复现Bank的测试在HammerF里先用单弦
void HammerModel::setMode(HammerMode _mode){
    mode = _mode;
    
    if(mode == HammerMode::HammerF) {
        if(pairedString_a) pairedString_a->setMode(StringMode::HammerFTest);
    } else {
        if(pairedString_a) pairedString_a->setMode(StringMode::Normal);
        if(pairedString_b) pairedString_b->setMode(StringMode::Normal);
        if(pairedString_c) pairedString_c->setMode(StringMode::Normal);
    }
}

void HammerModel::setInactive(){
    pairedKey->key_active = false;
}
