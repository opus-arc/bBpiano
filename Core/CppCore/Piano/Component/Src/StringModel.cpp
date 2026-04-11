//
//  StringModel.cpp
//  bBpiano
//
//  Created by opus arc on 2026/4/6.
//
//  This document is not AI-assisted.
//

#include "StringModel.hpp"
#include "../HammerModel.hpp"
#include "../KeyModel.hpp"

StringModel::StringModel(HammerModel *_pairedHammer, int _midi_n, int _stringNum) :

    pairedHammer(_pairedHammer),
    midi_n(_midi_n),
    string_index(_stringNum)

{
    
    Ts = 1 / sampleRate;
    

    // 计算波导长度
    N = double(sampleRate) / double(2 * compute_f0());

    // 取不大于波导长度的最大整数作为数组长度
    N_int = std::floor(N);
    if(N_int <= 0) N_int = 2;
    N_index = N_int - 1;

    // 计算力和速度的比例常数
    Z = std::sqrt(T * rho);

    // 初始化 N_int 个 0.0f 的离散位置
    right.assign(N_int, 0.0f);
    left.assign(N_int, 0.0f);
    rightNext.assign(N_int, 0.0f);
    leftNext.assign(N_int, 0.0f);
    
    
    std::cout << "midi_n: " << midi_n << ", string_index: " << string_index << ", f0: " << compute_f0() << std::endl;
}

// --------------------------------------------
// MARK: 实时值函数

// 根据 midi_n, reference_tone, temperament 计算 f0
// TODO: 这里能使用查表的方式降低加计算成本
float StringModel::get_f0() const {
    
    if(!f0_cached_version || // StringModel 的数据还未初始化
       f0_cached_version != ModelParameters::instance().tuning->version
       ){
        cache_f0 = compute_f0();
        f0_cached_version = ModelParameters::instance().tuning->version;
        return cache_f0;
    }
    
    
    return cache_f0;
}

float StringModel::compute_f0() const {
    const auto& tuning = ModelParameters::instance().tuning;;

    const float f_ref = tuning->reference_tone; // e.g. 440.0
    const int n_ref = 69; // A4
    const int n = midi_n;

    // Equal temperament base frequency
    float f_equal = f_ref * std::pow(2.0f, (n - n_ref) / 12.0f);

    // 获取当前音在八度内的位置
    int degree = (n % 12 + 12) % 12;

    float cent_offset = 0.0f;

    switch (tuning->temperament) {
    case ModelParameters::Tuning::Temperament::equal:
//        return f_equal;
            break;

    case ModelParameters::Tuning::Temperament::pythagore: {
        static const float cents[12] = {
            0, 113.7f, 203.9f, 317.6f, 407.8f, 521.5f,
            611.7f, 701.9f, 815.6f, 905.9f, 1019.5f, 1109.8f
        };
        cent_offset = cents[degree] - degree * 100.0f;
        break;
    }

    case ModelParameters::Tuning::Temperament::zarlino: {
        static const float cents[12] = {
            0, 112.0f, 204.0f, 316.0f, 386.0f, 498.0f,
            590.0f, 702.0f, 814.0f, 884.0f, 996.0f, 1088.0f
        };
        cent_offset = cents[degree] - degree * 100.0f;
        break;
    }

    case ModelParameters::Tuning::Temperament::meantone: {
        static const float cents[12] = {
            0, 76.0f, 193.2f, 310.3f, 386.3f, 503.4f,
            579.5f, 696.6f, 772.6f, 889.7f, 1006.8f, 1082.9f
        };
        cent_offset = cents[degree] - degree * 100.0f;
        break;
    }

    case ModelParameters::Tuning::Temperament::werckmeister: {
        static const float cents[12] = {
            0, 90.2f, 192.2f, 294.1f, 390.2f, 498.0f,
            588.3f, 696.6f, 792.2f, 888.3f, 996.1f, 1088.3f
        };
        cent_offset = cents[degree] - degree * 100.0f;
        break;
    }

    case ModelParameters::Tuning::Temperament::well:
        // 简化处理（可扩展为具体 well temperament）
//        return f_equal;
            break;
    }

    // 应用 cent 偏移
    float f = f_equal * std::pow(2.0f, cent_offset / 1200.0f);
    
    
    // 应用 usion width
    if(midi_n <= 52) {
        if(string_index == 1){
            f += tuning->unsion_width / 2.0;
        } else if (string_index == 2){
            f -= tuning->unsion_width / 2.0;
        }
    }else if(midi_n >= 53) {
        if(string_index == 1){
            f += tuning->unsion_width / 2.0;
        } else if (string_index == 3){
            f -= tuning->unsion_width / 2.0;
        }
    }

    return f;
}



// --------------------------------------------
// MARK: 计算函数


void StringModel::stringMovement() const {
    
    propagate();

    // 我这里只扫描第一根弦，我假设第一根弦能量不够了，那其他一到两根也差不多结束了
    // 再加上 128 帧扫描一次的剪枝
    if (!active) return;
    if (string_index == 1 && ++activityCounter >= 128) {
        activityCounter = 0;
        updateActivity();
        
    }
    
}

void StringModel::injectForce(double p, float F) const {
    if (std::abs(F) > 0.0f) active = true;
    
    // 边界条件
    if (p > 1)
        p = 1;
    if (p < 0)
        p = 0;

    int m = std::floor(p * N_index);
    
    if (m > N_index) m = N_index - 1;
    if (m < 0) m = 1;

    // 增量计算公式
    float delta = F / (2.0f * static_cast<float>(Z));

    // 注入弦
    right[m] += delta;
    left[m] += delta;
}

void StringModel::injectForce(int m, float F) const {
    if (std::abs(F) > 0.0f) active = true;
    
    // 边界条件
    if (m > N_index) m = N_index - 1;
    if (m < 0) m = 1;

    // 增量计算公式
    float delta = F / (2.0f * static_cast<float>(Z));

    // 注入弦
    right[m] += delta;
    left[m] += delta;
}

void StringModel::propagate() const {

    // 内部传播
    for (int i = 1; i <= N_index; ++i) {
        leftNext[i - 1] = left[i];
    }

    for (int i = 0; i <= N_index - 1; ++i) {
        rightNext[i + 1] = right[i];
    }

    // 边界反射
    rightNext[0] = -g * left[0];
    leftNext[N_index] = -g * right[N_index];

    std::swap(left, leftNext);
    std::swap(right, rightNext);
}

float StringModel::velocityAt(double p) const {
    // 边界条件
    if (p > 1)
        p = 1;
    if (p < 0)
        p = 0;

    int m = std::floor(p * N_index);

    return left[m] + right[m];
}

float StringModel::nextVelocityAt(double p) const {
    // 边界条件
    if (p > 1.0) p = 1.0;
    if (p < 0.0) p = 0.0;

    int m = std::floor(p * N_index);

    // 左边的右边那一帧下回就到拾音点
    int ml = m + 1;
    int mr = m - 1;

    if (ml > N_index) ml = N_index;
    if (mr < 0) mr = 0;

    return left[ml] + right[mr];
}



// TODO: 这里算能量还可以用包络近似，直接看该处的能量怎么样，简化计算
float StringModel::energy() const {
    double e = 0.0;
    for (int i = 0; i < N_int; ++i) {
        e += double(left[i]) * double(left[i]);
        e += double(right[i]) * double(right[i]);
    }
    return static_cast<float>(e);
}
bool StringModel::isActive() const {
    constexpr float energyThreshold = 1e-8f;
    return energy() > energyThreshold;
}
void StringModel::updateActivity() const {
    constexpr float energyThreshold = 1e-8f;

    double e = 0.0;
    for (int i = 0; i < N_int; ++i) {
        e += double(left[i]) * double(left[i]);
        e += double(right[i]) * double(right[i]);
    }

    active = (e > energyThreshold);
    
//    std::cout<<"midi_n: "<<midi_n<<", key_active: "<<active<<std::endl;
    
}
