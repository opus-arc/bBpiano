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
    void hammerMovement() {
        string_a->stringMovement();
    }
    void letsStrikeTheString() {
        string_a->injectForce(100);
    }
    
    StringModel* string_a;
    
private:

    
};

