#include <iostream>
#include <algorithm>
#include <cmath>

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

    int testCount = 0;

    inline void hammerMovement() {
        testCount++;
        
        const double stringY =
                string_a->getDisplacementAtStrike();

            const double compression_m =
                x - stringY;

            double modelCompression_m =
                std::max(0.0, compression_m);

            if(contactState == ContactState::ReleasedUntilNextStrike) {
                modelCompression_m = 0.0;
            }

            const double rawForce =
                process(modelCompression_m);

            double F = 0.0;

            switch(contactState) {
                case ContactState::Separated:
                    if(compression_m > 0.0) {
                        if(rawForce > 0.0) {
                            contactState = ContactState::Contacting;
                            F = rawForce;
                        } else {
                            contactState =
                                ContactState::ReleasedUntilNextStrike;
                        }
                    }
                    break;

                case ContactState::Contacting:
                    if(
                        compression_m <= 0.0
                        || rawForce <= 0.0
                    ) {
                        contactState =
                            ContactState::ReleasedUntilNextStrike;
                    } else {
                        // Positive compression means that local force
                        // fluctuations remain part of the same contact.
                        F = rawForce;
                    }
                    break;

                case ContactState::ReleasedUntilNextStrike:
                    break;
            }

            contactForce = F;

            // hammer 受到反作用
            acceleration = -F / m;
            v += acceleration * dt;
            x += v * dt;

        if(F>0) {
            string_a->injectForce(F);
//            std::cout
//            << "Time: " << dt * testCount
//            << ", F: " << F
//            << '\n';
        }

        string_a->stringMovement();
    }
    void setVIn(double newVelocity) {
        v = newVelocity;
        x = string_a->getDisplacementAtStrike();
        acceleration = 0.0;
        contactForce = 0.0;

        contactState = ContactState::Separated;
    }

    double getContactForce() const {
        return contactForce;
    }
    
    StringModel* string_a;
    
private:
    enum class ContactState {
        Separated,
        Contacting,
        ReleasedUntilNextStrike
    };

    ContactState contactState = ContactState::Separated;
    double contactForce = 0.0;
    
    double sampleRate = 44100.0;
    double dt = 1.0 / sampleRate;
    
    // 合计旋转等效质量 g
    double m = 15 * 0.001;
    double v = 0.0;
    
    double x = 0.0;
    double acceleration = 0.0;
    
    // N / mm^p
    double hardness = 109.87459216355096;
    
    // 非线性指数
    double p = 1.5;
    
    // 控制加载和卸载是否一样
    double epsilon = 0.8104484280879438;
    
    // 松弛时间，单位 s
    double tau0 = 8.741820760093148e-06;
    
    // 历史压缩 毛毡记忆
    double history = 0.0;
    
    // 数值积分状态
    double last_up = 0.0;
    
    // compression_m 为毛毡压缩量，单位为 m
    double process(double compression_m) {
        const double compression_mm =
            std::max(0.0, compression_m) * 1000.0;
        const double up = std::pow(compression_mm, p);
        const double a = std::exp(-dt / tau0);
        const double tauOverDt = tau0 / dt;
        const double previousWeight =
            tauOverDt * (1.0 - a) - a;
        const double currentWeight =
            1.0 - tauOverDt * (1.0 - a);

        // history = (1/tau0) * integral(
        //     u^p(xi) * exp((xi - t) / tau0) dxi
        // )
        // 在相邻采样点之间对 u^p 使用分段线性插值。
        history =
            a * history
            + previousWeight * last_up
            + currentWeight * up;

        const double F =
            hardness * (up - epsilon * history);

        last_up = up;

        return F;
    }
    
};
