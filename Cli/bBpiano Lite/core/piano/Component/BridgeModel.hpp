//
//  BridgeModel.hpp
//  bBpiano Lite
//
//  Created by opus arc on 2026/6/24.
//

#ifndef BridgeModel_hpp
#define BridgeModel_hpp

#include <cmath>
#include <iostream>
#include "StringModel.hpp"

class BridgeModel {

public:
    int test = 0;

    std::vector<StringModel*> strings;

    BridgeModel(int i) : test(i) {
        strings.reserve(16);
    }

    ~BridgeModel() = default;

    float bridgeX = 0.0f;
    float bridgeV = 0.0f;

    inline void processJunction() {
        float incidentForce = 0.0f;
        float stringDamping = 0.0f;

        for (auto* s : strings) {
            if (!s) continue;
            incidentForce += s->bridgeIncidentForce();
            stringDamping += s->bridgeCharacteristicImpedance();
        }

        constexpr float fs = 44100.0f;
        constexpr float dt = 1.0f / fs;

        // First-order bridge model (mass + damping only).
        constexpr float bridgeMass = 0.003f;
        constexpr float bridgeDamping = 200.0f;
        const float effectiveDamping = bridgeDamping + stringDamping;

        // Velocity-wave junction:
        // F_string = 2 * Z * v_incident - Z * v_bridge, summed over strings.
        // So mass * dv/dt + (bridgeDamping + sum(Z)) * v = sum(2Z v_incident).
        const float decay = std::exp(-effectiveDamping * dt / bridgeMass);
        const float targetV = incidentForce / effectiveDamping;
        bridgeV = decay * bridgeV + (1.0f - decay) * targetV;
        bridgeX += bridgeV * dt;

        // Feed the shared bridge velocity back to every attached string.
        for (auto* s : strings) {
            if (!s) continue;
            s->bridgeVelocity = bridgeV;
        }
    }

    inline float getSamples() const {
        return bridgeV;
    }

    inline void BridgeCoupling(std::vector<double> StringsF){

    };
};

#endif /* BridgeModel_hpp */
