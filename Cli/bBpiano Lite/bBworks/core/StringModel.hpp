#include <iostream>
#include <algorithm>

// 我的目的是要设计一根能够对应现实世界的完美钢琴琴弦
class HammerModel;

class StringModel {
public:

    StringModel() {
        
        sampleRate = 44100.0;
        multiples = 1;
        f0 = 261.626;
        
        left.assign(10000, 0);
        right.assign(10000, 0);
        leftHead = 0;
        rightHead = 0;
        
        delay = sampleRate * multiples / (f0 * 2);
        if(delay < 16) throw std::runtime_error("delay 过小，琴弦初始化失败");
        delay_int = floor(delay - 1);
        delay_frac = delay - delay_int;
        
        strikePosition = 1.0 / 9.4;
        pickupPosition = 6.0 / 7.0;
        strikePort = SpatialPort(strikePosition, delay_int);
        pickupPort = SpatialPort(pickupPosition, delay_int);
        
        // 弦长 m
        length = 0.65;
        // 弦直径 mm
        d = 1.05;
        // 线密度 kg/m
        mu = 0.007;
        // 弦张力 N
        tension = 700;
        
        // 特性阻抗系数
        z = std::sqrt(mu * tension);
        
        std::cout
        << "delay: " << delay
        << ", f0: " << f0
        << ", z: " << z
        << '\n';
        
        hammerModel = nullptr;
        
    }
    
    ~StringModel() = default;
    
    inline void stringMovement() {
        for(int i = 0; i < multiples; i++)
        propagate();
    }
    
    inline float getSamples() {
        
        return left[originIndexToHeadIndex_l(pickupPort.index0)] * pickupPort.weight0 + left[originIndexToHeadIndex_l(pickupPort.index1)] * pickupPort.weight1 + right[originIndexToHeadIndex_r(pickupPort.index0)] * pickupPort.weight0 + right[originIndexToHeadIndex_r(pickupPort.index1)] * pickupPort.weight1;
    }
    
    inline void injectForce(float F) {
        injectForceAtJunction(strikePort.index0, F * strikePort.weight0);
        if (strikePort.index1 != strikePort.index0) {
            injectForceAtJunction(strikePort.index1, F * strikePort.weight1);
        }
    }
    
    HammerModel *hammerModel;
    
private:
    
    double sampleRate;
    int multiples;
    
    double f0;
    
    double delay;
    int delay_int;
    double delay_frac;
    
    std::vector<float> left;
    std::vector<float> right;
    int leftHead;
    int rightHead;
    
    double strikePosition;
    double pickupPosition;
    struct SpatialPort {
        int index0 = 0;
        int index1 = 0;
        double weight0 = 0.0f;
        double weight1 = 0.0f;
        SpatialPort(double position, int maxIndex) {
            position = std::clamp(position, 0.0, 1.0);
            maxIndex = std::max(maxIndex, 1);
            double originalIndex = position * maxIndex;
            index0 = std::floor(originalIndex);
            index1 = std::ceil(originalIndex);
            weight0 = originalIndex - index0;
            weight1 = 1- weight0;
        }
        SpatialPort() = default;
    };
    SpatialPort strikePort;
    SpatialPort pickupPort;
    
    // 弦长 m
    double length;
    // 弦直径 mm
    double d;
    // 线密度 kg/m
    double mu;
    // 弦张力 N
    double tension;
    // 特性阻抗系数
    double z;
    
    
    inline void injectForceAtJunction(int origin_junctionIndex, float F) {
        const int head_i_l = originIndexToHeadIndex_l(origin_junctionIndex);
        const int head_i_r = originIndexToHeadIndex_r(origin_junctionIndex + 1);
        // TODO: 此处力转换为速度增量的公式
        const float delta =  F / (2 * z);
        left[head_i_l] += delta;
        // TODO: 是否要改成 -=
        right[head_i_r] += delta;
        
        std::cout
        << "F: " << F
        << ", delta: " << delta
        << ", index: " << origin_junctionIndex
        << '\n';
    }
    
    // 0 <= i < delay_int !!!
    inline int originIndexToHeadIndex_l(int i) const {
        int x = leftHead + i;
        if (x >= delay_int)
            x -= delay_int;
        return x;
    }

    inline int originIndexToHeadIndex_r(int i) const {
        int x = rightHead + i;
        if (x >= delay_int)
            x -= delay_int;
        return x;
    }
    
    inline void propagate() {
        
        if (rightHead == 0) {
            rightHead = delay_int - 1;
        } else {
            --rightHead;
        }

        ++leftHead;
        
        if (leftHead == delay_int) {
            leftHead = 0;
        }
        
        boundaryFilter(left[leftHead], true);
        boundaryFilter(right[rightHead], false);
    }
    
    inline void boundaryFilter(float& boundary_value, bool isLeft) {
        if(isLeft) {
            boundary_value *= -1.0;
        } else {
            boundary_value *= -1.0;
        }
    }
    
    
};
