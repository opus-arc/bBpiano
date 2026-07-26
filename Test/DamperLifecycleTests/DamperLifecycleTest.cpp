#include "../../Cli/bBpiano Lite/core/piano/PianoModel.hpp"

#include <cmath>
#include <iostream>
#include <memory>
#include <streambuf>

namespace {

class NullBuffer final : public std::streambuf {
protected:
    int overflow(int character) override {
        return traits_type::not_eof(character);
    }
};

class ScopedCoutSilencer final {
public:
    ScopedCoutSilencer()
        : previous_(std::cout.rdbuf(&buffer_)) {}

    ~ScopedCoutSilencer() {
        std::cout.rdbuf(previous_);
    }

private:
    NullBuffer buffer_;
    std::streambuf* previous_;
};

bool approximatelyEqual(double lhs, double rhs) {
    return std::abs(lhs - rhs) < 1.0e-12;
}

} // namespace

int main() {
    std::unique_ptr<PianoModel> piano;
    {
        ScopedCoutSilencer silence;
        piano = std::make_unique<PianoModel>();
    }

    constexpr int midi = 60;
    KeyModel& key = *piano->pianoKeys[midi - 21];
    StringModel& string = *key.hammer->pairedString_a;

    {
        ScopedCoutSilencer silence;
        piano->note_on(midi, 80.0);
    }

    if (!key.key_down || !key.key_active) {
        std::cerr << "note_on did not set key_down/key_active\n";
        return 1;
    }

    double z1 = 1.0;
    double z2 = -1.0;
    piano->test_sustainPedal_active = false;
    const double heldOutput = string.testDamper(1.0, z1, z2);
    if (!approximatelyEqual(heldOutput, 1.0)
        || !approximatelyEqual(z1, 0.0)
        || !approximatelyEqual(z2, 0.0)) {
        std::cerr << "Held key did not keep its damper lifted\n";
        return 1;
    }

    {
        ScopedCoutSilencer silence;
        piano->note_off(midi, 0.0);
    }

    if (key.key_down || !key.key_active) {
        std::cerr << "note_off stopped resonance or left key_down set\n";
        return 1;
    }

    z1 = 0.0;
    z2 = 0.0;
    const double releasedOutput = string.testDamper(1.0, z1, z2);
    if (!(releasedOutput < 1.0) || approximatelyEqual(z1, 0.0)) {
        std::cerr << "Released key did not engage its damper\n";
        return 1;
    }

    {
        ScopedCoutSilencer silence;
        piano->sustainPedal_control(1.0);
    }
    z1 = 1.0;
    z2 = -1.0;
    const double sustainedOutput = string.testDamper(1.0, z1, z2);
    if (!approximatelyEqual(sustainedOutput, 1.0)
        || !approximatelyEqual(z1, 0.0)
        || !approximatelyEqual(z2, 0.0)) {
        std::cerr << "Sustain pedal did not lift the released key damper\n";
        return 1;
    }

    {
        ScopedCoutSilencer silence;
        piano->sustainPedal_control(0.0);
    }
    z1 = 0.0;
    z2 = 0.0;
    const double pedalReleasedOutput = string.testDamper(1.0, z1, z2);
    if (!(pedalReleasedOutput < 1.0) || approximatelyEqual(z1, 0.0)) {
        std::cerr << "Pedal release did not re-engage the released key damper\n";
        return 1;
    }

    key.silence();
    if (key.key_down || key.key_active) {
        std::cerr << "silence did not reset key lifecycle state\n";
        return 1;
    }

    std::cout << "Damper/key/pedal lifecycle: PASS\n";
    return 0;
}
