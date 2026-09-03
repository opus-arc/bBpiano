#include <iostream>
#include <algorithm>

// 我的目的是要设计一根能够对应现实世界的完美钢琴琴弦
class HammerModel;

class StringModel {
public:

    StringModel() {
        
        sampleRate = 44100.0;
        multiples = 1;
        dt = 1.0 / (sampleRate * multiples);
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
        
        strikeDisplacement = 0.0;
        
        // 弦长 m
        length = 0.65;
        // 弦直径 mm
        d = 1.05;
        // 线密度 kg/m
        mu = 0.006751517084270981;
        // 弦张力 N。由 f0、弦长和线密度反推，使
        // f0 = sqrt(tension / mu) / (2 * length) 与 waveguide 一致。
        const double waveSpeed = 2.0 * length * f0;
        tension = mu * waveSpeed * waveSpeed;
        
        // 特性阻抗系数
        z = std::sqrt(mu * tension);
        
        // fractional filter
        fractional_a1 = double(1 - delay_frac) / double(1 + delay_frac);

        std::cout
        << "delay: " << delay
        << ", f0: " << f0
        << ", z: " << z
        << '\n';
        
        hammerModel = nullptr;
        
    }
    
    ~StringModel() = default;
    
    inline void stringMovement() {
        for(int i = 0; i < multiples; i++) {
            // HammerModel injects force immediately before this call.
            // Integrate the power-conjugate strike velocity before propagation
            // moves the newly emitted waves away from the contact junction.
            renewStrikeDisplacement();
            propagate();
        }
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
    
    inline double getDisplacementAtStrike() {
        return strikeDisplacement;
    }
    
    HammerModel *hammerModel;
    
private:
    
    double sampleRate;
    double dt;
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
            const double originalIndex = position * maxIndex;
            index0 = std::floor(originalIndex);
            index1 = std::min(index0 + 1, maxIndex);
            const double fraction = originalIndex - index0;
            weight1 = index1 == index0 ? 0.0 : fraction;
            weight0 = 1.0 - weight1;
        }
        SpatialPort() = default;
    };
    SpatialPort strikePort;
    SpatialPort pickupPort;
    
    double strikeDisplacement;
    
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
    
    
    // fractional filter
    double fractional_a1 = 0.0;
    mutable float fractional_x1_r = 0.0; // 上一次传入 allpass 的值
    mutable float fractional_y1_r = 0.0;
    mutable float fractional_x1_l = 0.0; // 上一次传入 allpass 的值
    mutable float fractional_y1_l = 0.0;


    inline void injectForceAtJunction(
        int origin_junctionIndex,
        float F
    ) {
        const int head_i_l =
            originIndexToHeadIndex_l(origin_junctionIndex);

        const int head_i_r =
            originIndexToHeadIndex_r(origin_junctionIndex);

        const float delta = F / (2 * z);

        left[head_i_l] += delta;
        right[head_i_r] += delta;
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
        // 在移动 head 之前，保存即将离开两个固定端的行波。
        const float outgoingAtLeft =
            left[originIndexToHeadIndex_l(0)];

        const float outgoingAtRight =
            right[originIndexToHeadIndex_r(delay_int - 1)];

        // 正常传播两个 travelling-wave rail。
        if (rightHead == 0) {
            rightHead = delay_int - 1;
        } else {
            --rightHead;
        }

        ++leftHead;
        if (leftHead == delay_int) {
            leftHead = 0;
        }

        // 左端固定边界：
        // 离开左端的左行波，负反射为右行波。
        float reflectedFromLeft = outgoingAtLeft;
        boundaryFilter(reflectedFromLeft, true);

        right[originIndexToHeadIndex_r(0)] =
            reflectedFromLeft;

        // 右端固定边界：
        // 离开右端的右行波，负反射为左行波。
        float reflectedFromRight = outgoingAtRight;
        boundaryFilter(reflectedFromRight, false);

        left[originIndexToHeadIndex_l(delay_int - 1)] =
            reflectedFromRight;
    }
    
    inline void boundaryFilter(float& boundary_value, bool isLeft) {
        if(isLeft) {
            boundary_value *= -0.996;
            fractionalFilter(
                boundary_value,
                fractional_x1_l,
                fractional_y1_l
            );
        } else {
            boundary_value *= -0.996;
            fractionalFilter(
                boundary_value,
                fractional_x1_r,
                fractional_y1_r
            );
        }
    }
    
    
    inline float getVelocityAtStrike() {
        return left[originIndexToHeadIndex_l(strikePort.index0)] * strikePort.weight0 + left[originIndexToHeadIndex_l(strikePort.index1)] * strikePort.weight1 + right[originIndexToHeadIndex_r(strikePort.index0)] * strikePort.weight0 + right[originIndexToHeadIndex_r(strikePort.index1)] * strikePort.weight1;
    }
    
    inline void renewStrikeDisplacement() {
        strikeDisplacement += getVelocityAtStrike() * dt;
    }
    
    inline void fractionalFilter(float &x, float &x1, float &y1) const {
        // y = a1 * x + x1 - a1 * y1;
        float y = static_cast<float>(fractional_a1 * x)
            + static_cast<float>(x1)
            - static_cast<float>(fractional_a1 * y1);
        y1 = y;
        x1 = x;
        x = y;
    }


};
