#include <iostream>

// Internal
#include "./StringModel.hpp"

class HammerModel {
public:

    HammerModel() {
        string_a = new StringModel();
    }
    ~HammerModel() {
        delete string_a;
    }
    
    inline void hammerMovement() {
        string_a->stringMovement();
        
        float stringY =
                string_a->getDisplacementAtStrike();

            float compression =
                x - stringY;

            float F = 0;

            if(compression > 0) {
                F = process(compression);
            }

            // hammer 受到反作用
            acceleration = -F / m;
            v += acceleration * dt;
            x += v * dt;

            if(F>0)
                string_a->injectForce(F);
    }
    inline void setVIn(double _v) {
        v = _v;
    }
    
    StringModel* string_a;
    
private:
    
    double sampleRate = 44100.0;
    double dt = 1.0 / sampleRate;
    
    // 合计旋转等效质量 g
    double m = 15 * 0.001;
    double v = 0.0;
    
    double x = 0.0;
    double acceleration = 0.0;
    
    double hardness = 10000.0; // Renner
    
    // 非线性指数
    double p = 3.95; // Renner
    
    // 控制加载和卸载是否一样
    double epsilon = 0.992; // Renner
    
    // 松弛时间 mu s
    double tau0 = 0.002;
    
    // 历史压缩 毛毡记忆
    double history = 0.0;
    
    // 数值积分状态
    double last_up = 0.0;
    
    // u 为毛毡压缩量
    float process(float u) {

        float up = pow(u,p);

        float derivative =
            (up - last_up) / dt;

        float a = exp(-dt / tau0);

        history =
            a * history
            +
            (1-a) * derivative * tau0;

        float F = hardness * (up - epsilon * history);

        last_up = up;

        return F;
    }
    
};



