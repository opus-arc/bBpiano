// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
// [AI-ASSISTED, HUMAN-UNDERSTOOD CODE]
// This file may have been written with the assistance of AI for explanation,
// discussion, review, implementation guidance, or non-critical engineering suggestions.
//
// However, every line included in this file has been reviewed, understood,
// and accepted by its author. Every implementation is expected to be explainable,
// reproducible, open to inspection, and subject to criticism and revision.
//
// AI is treated as an engineering assistant rather than an authority:
// the author remains responsible for the design decisions, assumptions,
// correctness, and final implementation contained in this file.
// —————————————————————————
// [本文件包含 AI 辅助下完成的代码]
// 本文件在编写过程中可能使用 AI 进行原理讲解、讨论、代码审阅、实现指导，
// 或提供非关键性的工程建议。
//
// 但最终保留在本文件中的每一行代码，均由作者亲自审阅、理解并确认。
// 所有实现都应能够由作者解释、复现、检查，并接受批评、修改与质疑。
//
// AI 在此仅作为工程辅助工具，而非技术权威；
// 本文件中的设计选择、假设、正确性以及最终实现，均由作者本人承担责任。
//
// Ziyang Tan
// 2026-04-06
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

#ifndef loss_filter_hpp
#define loss_filter_hpp

#include <algorithm>
#include <array>
#include <complex>
#include <cstddef>
#include <numbers>

// External physics-first prototype: sigma=b1+b2*f^2; C3-C5 scope.
class LossFilter {
    inline static constexpr int kMidiMin = 21;
    inline static constexpr int kMidiMax = 108;
    inline static constexpr std::size_t kSectionCount = 2;
    struct LossShelf { float b0; float b1; float a1; };
    struct LossPreset { int midi; double referenceF1; double phaseDelaySamples;
        double groupDelaySamples; std::array<LossShelf, kSectionCount> sections; };
public:
    LossPreset lossPreset;
private:
    std::array<float, kSectionCount> state_{};
public:
    explicit LossFilter(int midi) noexcept : lossPreset(kPresets[
        static_cast<std::size_t>(std::clamp(midi, kMidiMin, kMidiMax) - kMidiMin)]) {}
    [[nodiscard]] inline double get_group_delay() const noexcept { return lossPreset.groupDelaySamples; }
    [[nodiscard]] inline double get_phase_delay(double sampleRate, double frequency) const {
        const double omega = 2.0 * std::numbers::pi_v<double> * frequency / sampleRate;
        const std::complex<double> z1 = std::polar(1.0, -omega);
        std::complex<double> response{1.0, 0.0};
        for (const auto& c : lossPreset.sections)
            response *= (double(c.b0) + double(c.b1) * z1) / (1.0 + double(c.a1) * z1);
        return -std::arg(response) / omega;
    }
    inline void process(float& x) noexcept {
        for (std::size_t i = 0; i < kSectionCount; ++i) {
            const auto& c = lossPreset.sections[i];
            const float y = c.b0 * x + state_[i];
            state_[i] = c.b1 * x - c.a1 * y; x = y;
        }
    }
    inline void system_reset() noexcept { state_.fill(0.0f); }
    [[nodiscard]] inline float state_energy() const noexcept {
        float energy = 0.0f; for (float value : state_) energy += value * value;
        return energy / static_cast<float>(kSectionCount);
    }
private:
    inline static constexpr std::array<LossPreset, 88> kPresets = {{
        // MIDI 21; global; p95 T60 realization error 1.247%
        LossPreset{21, 27.5, 0.90716426692764396, 0.9071646747176394, {{
            LossShelf{0.509180427f, 0.508661687f, 0.0483981334f},
            LossShelf{0.524460435f, 0.52394706f, 0.0484074801f},
        }}},
        // MIDI 22; global; p95 T60 realization error 1.209%
        LossPreset{22, 29.13523509488062, 0.88551598492877714, 0.88551653056390844, {{
            LossShelf{0.515856087f, 0.515351951f, 0.0604129694f},
            LossShelf{0.530497193f, 0.529971659f, 0.0604688823f},
        }}},
        // MIDI 23; global; p95 T60 realization error 1.173%
        LossPreset{23, 30.867706328507751, 0.86378779457930455, 0.86378849770897626, {{
            LossShelf{0.522680938f, 0.522195697f, 0.0727941841f},
            LossShelf{0.536697924f, 0.536072493f, 0.0727704167f},
        }}},
        // MIDI 24; global; p95 T60 realization error 1.134%
        LossPreset{24, 32.703195662574828, 0.84231887734651623, 0.84231975921181745, {{
            LossShelf{0.529564381f, 0.529080033f, 0.085329771f},
            LossShelf{0.542902589f, 0.542405844f, 0.0853083879f},
        }}},
        // MIDI 25; global; p95 T60 realization error 1.100%
        LossPreset{25, 34.64782887210901, 0.82080496635912426, 0.82080605026171982, {{
            LossShelf{0.536565423f, 0.536063969f, 0.0981385782f},
            LossShelf{0.549319744f, 0.548812091f, 0.0981318802f},
        }}},
        // MIDI 26; global; p95 T60 realization error 1.060%
        LossPreset{26, 36.70809598967594, 0.79922932279552239, 0.79923063493358293, {{
            LossShelf{0.54370755f, 0.543204606f, 0.111300826f},
            LossShelf{0.55590862f, 0.555393159f, 0.111301772f},
        }}},
        // MIDI 27; global; p95 T60 realization error 1.015%
        LossPreset{27, 38.890872965260115, 0.77695290386536875, 0.77695446585983385, {{
            LossShelf{0.551802397f, 0.549461842f, 0.124579713f},
            LossShelf{0.562767565f, 0.561819613f, 0.124587208f},
        }}},
        // MIDI 28; global; p95 T60 realization error 0.980%
        LossPreset{28, 41.203444614108747, 0.75690961413037727, 0.75691146524411568, {{
            LossShelf{0.558250427f, 0.557452202f, 0.137990847f},
            LossShelf{0.569241643f, 0.568750918f, 0.137992606f},
        }}},
        // MIDI 29; global; p95 T60 realization error 0.937%
        LossPreset{29, 43.653528929125486, 0.73630926537057295, 0.73631143434880675, {{
            LossShelf{0.565373719f, 0.564894021f, 0.151573971f},
            LossShelf{0.576047182f, 0.575548828f, 0.151596025f},
        }}},
        // MIDI 30; global; p95 T60 realization error 0.901%
        LossPreset{30, 46.2493028389543, 0.71603221279778084, 0.71603473319048061, {{
            LossShelf{0.572652698f, 0.572174609f, 0.165191963f},
            LossShelf{0.582844019f, 0.58235693f, 0.165200919f},
        }}},
        // MIDI 31; global; p95 T60 realization error 0.855%
        LossPreset{31, 48.999429497718666, 0.69589490920829777, 0.69589781890593205, {{
            LossShelf{0.580047488f, 0.579477608f, 0.178995475f},
            LossShelf{0.589761376f, 0.589222431f, 0.178983793f},
        }}},
        // MIDI 32; global; p95 T60 realization error 0.808%
        LossPreset{32, 51.913087197493141, 0.6762882890346601, 0.67629162994047298, {{
            LossShelf{0.587343633f, 0.586869538f, 0.192821503f},
            LossShelf{0.596657455f, 0.596169353f, 0.192826778f},
        }}},
        // MIDI 33; global; p95 T60 realization error 0.757%
        LossPreset{33, 55, 0.65677078335117789, 0.65677460032584234, {{
            LossShelf{0.594782948f, 0.594311178f, 0.206880808f},
            LossShelf{0.603679717f, 0.603197217f, 0.206876919f},
        }}},
        // MIDI 34; global; p95 T60 realization error 0.703%
        LossPreset{34, 58.270470189761241, 0.63765584921914897, 0.63766019039365374, {{
            LossShelf{0.602217615f, 0.601748049f, 0.220966235f},
            LossShelf{0.610726357f, 0.610236168f, 0.220962524f},
        }}},
        // MIDI 35; global; p95 T60 realization error 0.641%
        LossPreset{35, 61.735412657015502, 0.61891035299060482, 0.61891527074279018, {{
            LossShelf{0.609672725f, 0.609206378f, 0.235127836f},
            LossShelf{0.617786109f, 0.617287874f, 0.235073969f},
        }}},
        // MIDI 36; global; p95 T60 realization error 0.604%
        LossPreset{36, 65.406391325149656, 0.46606000189624142, 0.46606333375157766, {{
            LossShelf{0.709973693f, 0.508443058f, 0.233750343f},
            LossShelf{0.706111848f, 0.527553976f, 0.233665884f},
        }}},
        // MIDI 37; global; p95 T60 realization error 0.592%
        LossPreset{37, 69.295657744218019, 0.33795031888478599, 0.3379514202006888, {{
            LossShelf{0.789180994f, 0.390327096f, 0.193514511f},
            LossShelf{0.798136592f, 0.395159841f, 0.193296418f},
        }}},
        // MIDI 38; global; p95 T60 realization error 0.586%
        LossPreset{38, 73.416191979351879, 0.32446968201521958, 0.32447051448061071, {{
            LossShelf{0.641469777f, 0.441583872f, 0.095186308f},
            LossShelf{0.992804527f, 0.972382426f, 0.965186954f},
        }}},
        // MIDI 39; global; p95 T60 realization error 0.584%
        LossPreset{39, 77.781745930520231, 0.26545615584581045, 0.26545572753212443, {{
            LossShelf{0.706958234f, 0.365230888f, 0.0835227966f},
            LossShelf{0.996231437f, 0.986721754f, 0.982953131f},
        }}},
        // MIDI 40; global; p95 T60 realization error 0.573%
        LossPreset{40, 82.406889228217494, 0.23121818140255349, 0.23121689314889357, {{
            LossShelf{0.745844185f, 0.317866057f, 0.0743207335f},
            LossShelf{0.996882737f, 0.988090754f, 0.98497349f},
        }}},
        // MIDI 41; global; p95 T60 realization error 0.388%
        LossPreset{41, 87.307057858250971, 0.20170706983317438, 0.20170534215666122, {{
            LossShelf{0.777087629f, 0.288775146f, 0.0759222656f},
            LossShelf{0.997336626f, 0.987483203f, 0.984819829f},
        }}},
        // MIDI 42; global; p95 T60 realization error 0.378%
        LossPreset{42, 92.4986056779086, 0.18298007617707771, 0.18297774073523468, {{
            LossShelf{0.798847556f, 0.261485577f, 0.0697784498f},
            LossShelf{0.996821523f, 0.988175392f, 0.984996915f},
        }}},
        // MIDI 43; global; p95 T60 realization error 0.363%
        LossPreset{43, 97.998858995437331, 0.16574132548992879, 0.16573837139231343, {{
            LossShelf{0.816609859f, 0.239585862f, 0.0650762916f},
            LossShelf{0.999991238f, 0.983839512f, 0.98383075f},
        }}},
        // MIDI 44; global; p95 T60 realization error 0.351%
        LossPreset{44, 103.82617439498628, 0.15340556480886153, 0.15340213410908995, {{
            LossShelf{0.830932021f, 0.223347992f, 0.0626475513f},
            LossShelf{0.99898237f, 0.986017644f, 0.985000014f},
        }}},
        // MIDI 45; global; p95 T60 realization error 0.342%
        LossPreset{45, 110, 0.14057175715261291, 0.14056772174027904, {{
            LossShelf{0.84485811f, 0.205899343f, 0.0586304404f},
            LossShelf{0.99999243f, 0.985005498f, 0.984997928f},
        }}},
        // MIDI 46; global; p95 T60 realization error 0.361%
        LossPreset{46, 116.54094037952248, 0.13194577695036819, 0.13194142342631751, {{
            LossShelf{0.853956044f, 0.199678212f, 0.0610868856f},
            LossShelf{0.999991536f, 0.98500824f, 0.984999716f},
        }}},
        // MIDI 47; global; p95 T60 realization error 0.434%
        LossPreset{47, 123.47082531403103, 0.122932070583298, 0.12292724873682467, {{
            LossShelf{0.864486277f, 0.188451856f, 0.0599704273f},
            LossShelf{0.998938501f, 0.985935986f, 0.984874487f},
        }}},
        // MIDI 48; global; p95 T60 realization error 0.511%
        LossPreset{48, 130.81278265029931, 0.11726177523485393, 0.11725675281697417, {{
            LossShelf{0.871660352f, 0.185513124f, 0.0638398975f},
            LossShelf{0.996441066f, 0.988557756f, 0.984998822f},
        }}},
        // MIDI 49; global; p95 T60 realization error 0.593%
        LossPreset{49, 138.59131548843604, 0.11023193956390301, 0.11022650456498929, {{
            LossShelf{0.970262229f, 0.0248138309f, 0.00100000005f},
            LossShelf{0.90311116f, 0.219625294f, 0.122736461f},
        }}},
        // MIDI 50; global; p95 T60 realization error 0.680%
        LossPreset{50, 146.83238395870379, 0.10448517554549977, 0.10447932426321985, {{
            LossShelf{0.958856881f, 0.0365479961f, 0.00100000005f},
            LossShelf{0.919616461f, 0.249251649f, 0.168868139f},
        }}},
        // MIDI 51; global; p95 T60 realization error 0.748%
        LossPreset{51, 155.56349186104046, 0.098940300779251977, 0.098934106275699582, {{
            LossShelf{0.957032382f, 0.0386828519f, 0.00100000005f},
            LossShelf{0.927161217f, 0.265097588f, 0.19225882f},
        }}},
        // MIDI 52; global; p95 T60 realization error 0.761%
        LossPreset{52, 164.81377845643496, 0.093876942006254085, 0.093870367412676708, {{
            LossShelf{0.956007719f, 0.0400018543f, 0.00100000005f},
            LossShelf{0.933470905f, 0.282402754f, 0.215873659f},
        }}},
        // MIDI 53; global; p95 T60 realization error 0.944%
        LossPreset{53, 174.61411571650194, 0.088216497057697338, 0.088209468102949093, {{
            LossShelf{0.956085384f, 0.0401989892f, 0.00100000005f},
            LossShelf{0.939637244f, 0.295529902f, 0.235167161f},
        }}},
        // MIDI 54; global; p95 T60 realization error 1.643%
        LossPreset{54, 184.9972113558172, 0.081874532355640517, 0.081866728552633966, {{
            LossShelf{0.948426247f, 0.048101902f, 0.00100000005f},
            LossShelf{0.953352094f, 0.394542992f, 0.347895116f},
        }}},
        // MIDI 55; global; p95 T60 realization error 0.806%
        LossPreset{55, 195.99771799087463, 0.079403861189346711, 0.079395778395922129, {{
            LossShelf{0.956778407f, 0.0400209539f, 0.00100000005f},
            LossShelf{0.948613226f, 0.32784009f, 0.276453346f},
        }}},
        // MIDI 56; global; p95 T60 realization error 1.107%
        LossPreset{56, 207.65234878997256, 0.074875933368343084, 0.074867620877405297, {{
            LossShelf{0.948016644f, 0.0731995255f, 0.0252859648f},
            LossShelf{0.962163746f, 0.395742744f, 0.357906491f},
        }}},
        // MIDI 57; global; p95 T60 realization error 1.902%
        LossPreset{57, 220, 0.068620238112223605, 0.068610706070286867, {{
            LossShelf{0.955820799f, 0.0414119139f, 0.00100000005f},
            LossShelf{0.961351275f, 0.414186478f, 0.375537753f},
        }}},
        // MIDI 58; global; p95 T60 realization error 1.408%
        LossPreset{58, 233.08188075904496, 0.067523041778097467, 0.067514426673541947, {{
            LossShelf{0.938335717f, 0.122939162f, 0.0650494993f},
            LossShelf{0.980198801f, 0.571277678f, 0.551476419f},
        }}},
        // MIDI 59; global; p95 T60 realization error 1.488%
        LossPreset{59, 246.94165062806206, 0.06373878282173423, 0.063729639280263137, {{
            LossShelf{0.94180274f, 0.120574035f, 0.0659451708f},
            LossShelf{0.981056929f, 0.582652271f, 0.563709199f},
        }}},
        // MIDI 60; global; p95 T60 realization error 2.298%
        LossPreset{60, 261.62556530059862, 0.056928784569556183, 0.056916971741435812, {{
            LossShelf{0.961704791f, 0.0361174941f, 0.00100000005f},
            LossShelf{0.969245374f, 0.445581794f, 0.414827168f},
        }}},
        // MIDI 61; global; p95 T60 realization error 3.162%
        LossPreset{61, 277.18263097687208, 0.051863156817875206, 0.05184992702763528, {{
            LossShelf{0.962329268f, 0.0356558673f, 0.00100000005f},
            LossShelf{0.974796534f, 0.495570719f, 0.470367283f},
        }}},
        // MIDI 62; global; p95 T60 realization error 1.672%
        LossPreset{62, 293.66476791740757, 0.052558724607467883, 0.052546743102426727, {{
            LossShelf{0.9568578f, 0.0852884501f, 0.0450923853f},
            LossShelf{0.979442894f, 0.502439857f, 0.481882781f},
        }}},
        // MIDI 63; global; p95 T60 realization error 1.752%
        LossPreset{63, 311.12698372208092, 0.049443333938828764, 0.049430413027113804, {{
            LossShelf{0.958348155f, 0.0859873146f, 0.0471233912f},
            LossShelf{0.981708527f, 0.52803278f, 0.509741306f},
        }}},
        // MIDI 64; global; p95 T60 realization error 0.761%
        LossPreset{64, 329.62755691286992, 0.05018487445041131, 0.050172868054033259, {{
            LossShelf{0.952816129f, 0.125764444f, 0.0812796876f},
            LossShelf{0.986026585f, 0.609295309f, 0.595321953f},
        }}},
        // MIDI 65; global; p95 T60 realization error 1.974%
        LossPreset{65, 349.22823143300388, 0.043864353550868297, 0.043849830886102087, {{
            LossShelf{0.964000463f, 0.0780902132f, 0.0445734337f},
            LossShelf{0.982624471f, 0.514371037f, 0.496995509f},
        }}},
        // MIDI 66; global; p95 T60 realization error 1.821%
        LossPreset{66, 369.9944227116344, 0.041445699304470202, 0.041429139620925384, {{
            LossShelf{0.971147656f, 0.0412886031f, 0.0147097185f},
            LossShelf{0.97836256f, 0.448060066f, 0.426422626f},
        }}},
        // MIDI 67; global; p95 T60 realization error 2.022%
        LossPreset{67, 391.99543598174927, 0.038569751150702061, 0.038551444027183399, {{
            LossShelf{0.973727226f, 0.0304478798f, 0.0063054515f},
            LossShelf{0.979394138f, 0.439392835f, 0.418787003f},
        }}},
        // MIDI 68; global; p95 T60 realization error 0.871%
        LossPreset{68, 415.30469757994513, 0.038647813187822573, 0.038630939741655525, {{
            LossShelf{0.965877533f, 0.0969027132f, 0.0648946837f},
            LossShelf{0.987163246f, 0.537800133f, 0.524963379f},
        }}},
        // MIDI 69; global; p95 T60 realization error 2.153%
        LossPreset{69, 440, 0.034213136651369561, 0.034192177687860578, {{
            LossShelf{0.977191687f, 0.0219166316f, 0.00100000005f},
            LossShelf{0.981147885f, 0.437750757f, 0.418898672f},
        }}},
        // MIDI 70; global; p95 T60 realization error 2.031%
        LossPreset{70, 466.16376151808993, 0.031707762542134407, 0.031684544575275947, {{
            LossShelf{0.977934003f, 0.0212783217f, 0.00100000005f},
            LossShelf{0.983647823f, 0.447832286f, 0.43148008f},
        }}},
        // MIDI 71; global; p95 T60 realization error 2.339%
        LossPreset{71, 493.88330125612413, 0.030318248694925849, 0.03029388714829187, {{
            LossShelf{0.979133189f, 0.0201799199f, 0.00100000005f},
            LossShelf{0.983928025f, 0.459363192f, 0.443291217f},
        }}},
        // MIDI 72; global; p95 T60 realization error 1.130%
        LossPreset{72, 523.25113060119725, 0.03068623959455101, 0.030665106224438483, {{
            LossShelf{0.972732961f, 0.0948506445f, 0.069271937f},
            LossShelf{0.989834726f, 0.542254806f, 0.532089531f},
        }}},
        // MIDI 73; global; p95 T60 realization error 1.764%
        LossPreset{73, 554.36526195374415, 0.027104148923911456, 0.027076669008337221, {{
            LossShelf{0.981550395f, 0.0179468431f, 0.00100000005f},
            LossShelf{0.985468745f, 0.444626749f, 0.430095524f},
        }}},
        // MIDI 74; global; p95 T60 realization error 2.059%
        LossPreset{74, 587.32953583481515, 0.025603578913498359, 0.025574067243458867, {{
            LossShelf{0.982260883f, 0.0173193868f, 0.00100000005f},
            LossShelf{0.986480057f, 0.468735278f, 0.455215335f},
        }}},
        // MIDI 75; global; p95 T60 realization error 2.483%
        LossPreset{75, 622.25396744416184, 0.024063781460123581, 0.02403193743460982, {{
            LossShelf{0.982919574f, 0.0167379603f, 0.00100000005f},
            LossShelf{0.987605214f, 0.499471009f, 0.487076193f},
        }}},
        // MIDI 76; global; p95 T60 realization error 1.836%
        LossPreset{76, 659.25511382573984, 0.022281722725889672, 0.022246635253493741, {{
            LossShelf{0.983857989f, 0.0158759598f, 0.00100000005f},
            LossShelf{0.989221215f, 0.463819087f, 0.453040302f},
        }}},
        // MIDI 77; global; p95 T60 realization error 2.560%
        LossPreset{77, 698.45646286600777, 0.02065037867038455, 0.02061290564519579, {{
            LossShelf{0.9846977f, 0.0151007511f, 0.00100000005f},
            LossShelf{0.990332127f, 0.482459813f, 0.47279191f},
        }}},
        // MIDI 78; global; p95 T60 realization error 2.057%
        LossPreset{78, 739.9888454232688, 0.020043458871884919, 0.020004698848767131, {{
            LossShelf{0.98559016f, 0.0142779136f, 0.00100000005f},
            LossShelf{0.989940584f, 0.493299633f, 0.483240217f},
        }}},
        // MIDI 79; global; p95 T60 realization error 2.348%
        LossPreset{79, 783.99087196349853, 0.018325303177284917, 0.018281455438323522, {{
            LossShelf{0.985981226f, 0.0139489817f, 0.00100000005f},
            LossShelf{0.991916418f, 0.506072938f, 0.497989357f},
        }}},
        // MIDI 80; global; p95 T60 realization error 2.481%
        LossPreset{80, 830.60939515989025, 0.017645326943717989, 0.017600578528719345, {{
            LossShelf{0.986990869f, 0.0129980929f, 0.00100000005f},
            LossShelf{0.991523027f, 0.503933728f, 0.495456755f},
        }}},
        // MIDI 81; global; p95 T60 realization error 2.342%
        LossPreset{81, 880, 0.016392722264951056, 0.016343443718932864, {{
            LossShelf{0.987505317f, 0.0125398571f, 0.00100000005f},
            LossShelf{0.992678523f, 0.5085181f, 0.501196623f},
        }}},
        // MIDI 82; global; p95 T60 realization error 20.144%
        LossPreset{82, 932.32752303617985, 0.0094970911571389555, 0.0094607660781394938, {{
            LossShelf{0.991669536f, 0.00827072188f, 0.00100000005f},
            LossShelf{0.996464968f, 0.579506099f, 0.575971127f},
        }}},
        // MIDI 83; global; p95 T60 realization error 2.250%
        LossPreset{83, 987.76660251224826, 0.014365968890251529, 0.014308583761182969, {{
            LossShelf{0.988678813f, 0.0114687765f, 0.00100000005f},
            LossShelf{0.994058967f, 0.518969297f, 0.513028204f},
        }}},
        // MIDI 84; global; p95 T60 realization error 2.225%
        LossPreset{84, 1046.5022612023945, 0.013574570699782013, 0.013514183071725415, {{
            LossShelf{0.989387631f, 0.0108065158f, 0.00100000005f},
            LossShelf{0.994288981f, 0.508853912f, 0.503142893f},
        }}},
        // MIDI 85; global; p95 T60 realization error 2.370%
        LossPreset{85, 1108.7305239074883, 0.01276677222496244, 0.012702772695009284, {{
            LossShelf{0.990014732f, 0.0102229388f, 0.00100000005f},
            LossShelf{0.994642735f, 0.502920568f, 0.497563332f},
        }}},
        // MIDI 86; global; p95 T60 realization error 1.076%
        LossPreset{86, 1174.6590716696303, 0.012941280600085283, 0.012880002647250057, {{
            LossShelf{0.991223514f, 0.00906440616f, 0.00100000005f},
            LossShelf{0.993042648f, 0.424177974f, 0.417220592f},
        }}},
        // MIDI 87; global; p95 T60 realization error 1.883%
        LossPreset{87, 1244.5079348883237, 0.012221098748060795, 0.012160903879548815, {{
            LossShelf{0.989729166f, 0.0512266159f, 0.0416597314f},
            LossShelf{0.995333135f, 0.543007791f, 0.538340986f},
        }}},
        // MIDI 88; global; p95 T60 realization error 69.952%
        LossPreset{88, 1318.5102276514797, 0.0017922484159481496, 0.001798657881747373, {{
            LossShelf{0.996675551f, 0.442208976f, 0.440436333f},
            LossShelf{0.999548852f, 0.985142946f, 0.984691739f},
        }}},
        // MIDI 89; global; p95 T60 realization error 30.988%
        LossPreset{89, 1396.9129257320155, 0.0047712965714784674, 0.0047233363066292609, {{
            LossShelf{0.995141327f, 0.00506559992f, 0.00100000005f},
            LossShelf{0.998724103f, 0.749601364f, 0.748325467f},
        }}},
        // MIDI 90; global; p95 T60 realization error 1.658%
        LossPreset{90, 1479.9776908465376, 0.010812578606363645, 0.010746295946221391, {{
            LossShelf{0.990956485f, 0.0596992671f, 0.0512560122f},
            LossShelf{0.995483577f, 0.628141522f, 0.6236251f},
        }}},
        // MIDI 91; global; p95 T60 realization error 1.057%
        LossPreset{91, 1567.9817439269971, 0.0098216176121961995, 0.0097395917746298487, {{
            LossShelf{0.993366241f, 0.00709735323f, 0.00100000005f},
            LossShelf{0.994622886f, 0.432546169f, 0.427169055f},
        }}},
        // MIDI 92; global; p95 T60 realization error 1.527%
        LossPreset{92, 1661.2187903197805, 0.00902255340091874, 0.0089306525999325211, {{
            LossShelf{0.992716491f, 0.0256401431f, 0.018873984f},
            LossShelf{0.996320605f, 0.527242482f, 0.523563087f},
        }}},
        // MIDI 93; global; p95 T60 realization error 28.820%
        LossPreset{93, 1760, 0.0055945008864219641, 0.005506444387682401, {{
            LossShelf{0.994621933f, 0.0057295477f, 0.00100000005f},
            LossShelf{0.998482108f, 0.669281781f, 0.667763829f},
        }}},
        // MIDI 94; global; p95 T60 realization error 1.350%
        LossPreset{94, 1864.6550460723597, 0.0082852901255033654, 0.0081898274417011689, {{
            LossShelf{0.9935866f, 0.0297955833f, 0.0238480493f},
            LossShelf{0.996219933f, 0.50925529f, 0.505475223f},
        }}},
        // MIDI 95; global; p95 T60 realization error 33.279%
        LossPreset{95, 1975.5332050244961, 0.0055404491964765375, 0.0054367339517353563, {{
            LossShelf{0.994855464f, 0.00554319937f, 0.00100000005f},
            LossShelf{0.998185754f, 0.72871834f, 0.726904094f},
        }}},
        // MIDI 96; global; p95 T60 realization error 2.385%
        LossPreset{96, 2093.004522404789, 0.060685177171099142, 0.060692289086111587, {{
            LossShelf{0.97908479f, 0.0214933921f, 0.00100000005f},
            LossShelf{0.925710559f, 0.922291398f, 0.848001957f},
        }}},
        // MIDI 97; global; p95 T60 realization error 2.467%
        LossPreset{97, 2217.4610478149766, 0.060749840930865204, 0.060774294471047235, {{
            LossShelf{0.980520725f, 0.0200777538f, 0.00100000005f},
            LossShelf{0.923189104f, 0.920074701f, 0.843263745f},
        }}},
        // MIDI 98; global; p95 T60 realization error 12.865%
        LossPreset{98, 2349.3181433392601, 0.016905258493571152, 0.016756016971397454, {{
            LossShelf{0.98256427f, 0.0179850217f, 0.00100000005f},
            LossShelf{0.99999243f, 0.985007584f, 0.985000014f},
        }}},
        // MIDI 99; global; p95 T60 realization error 2.520%
        LossPreset{99, 2489.0158697766474, 0.04549384227279344, 0.045464904972224253, {{
            LossShelf{0.981879115f, 0.018758826f, 0.00100000005f},
            LossShelf{0.947710276f, 0.935872495f, 0.883582771f},
        }}},
        // MIDI 100; global; p95 T60 realization error 22.404%
        LossPreset{100, 2637.0204553029598, 0.014840154729723729, 0.014673957114837209, {{
            LossShelf{0.984619975f, 0.0159272701f, 0.00100000005f},
            LossShelf{0.99999243f, 0.985007584f, 0.985000014f},
        }}},
        // MIDI 101; global; p95 T60 realization error 12.934%
        LossPreset{101, 2793.8258514640311, 0.01452019461730175, 0.014337431487034582, {{
            LossShelf{0.984981537f, 0.015616239f, 0.00100000005f},
            LossShelf{0.99999243f, 0.985007584f, 0.985000014f},
        }}},
        // MIDI 102; global; p95 T60 realization error 32.378%
        LossPreset{102, 2959.9553816930752, 0.010612211603816275, 0.010460440954997697, {{
            LossShelf{0.988836408f, 0.0116895735f, 0.00100000005f},
            LossShelf{0.99999243f, 0.985007584f, 0.985000014f},
        }}},
        // MIDI 103; global; p95 T60 realization error 30.049%
        LossPreset{103, 3135.9634878539946, 0.010844919341281791, 0.01067088188757064, {{
            LossShelf{0.988635302f, 0.0119341016f, 0.00100000005f},
            LossShelf{0.99999243f, 0.985007584f, 0.985000014f},
        }}},
        // MIDI 104; global; p95 T60 realization error 18.126%
        LossPreset{104, 3322.437580639561, 0.011659108656308941, 0.011449516347192946, {{
            LossShelf{0.987855554f, 0.012767286f, 0.00100000005f},
            LossShelf{0.99999243f, 0.985007584f, 0.985000014f},
        }}},
        // MIDI 105; global; p95 T60 realization error 16.822%
        LossPreset{105, 3520, 0.01138980654730897, 0.011159682642585593, {{
            LossShelf{0.988144815f, 0.0125085283f, 0.00100000005f},
            LossShelf{0.99999243f, 0.985007584f, 0.985000014f},
        }}},
        // MIDI 106; global; p95 T60 realization error 3.112%
        LossPreset{106, 3729.3100921447194, 0.01253811026852846, 0.012254592508744652, {{
            LossShelf{0.987052023f, 0.0136855589f, 0.00100000005f},
            LossShelf{0.99999243f, 0.985007584f, 0.985000014f},
        }}},
        // MIDI 107; global; p95 T60 realization error 12.815%
        LossPreset{107, 3951.0664100489921, 0.015514637692152404, 0.0154273199751744, {{
            LossShelf{0.981381297f, 0.19368723f, 0.175470486f},
            LossShelf{0.99999243f, 0.985007584f, 0.985000014f},
        }}},
        // MIDI 108; global; p95 T60 realization error 3.636%
        LossPreset{108, 4186.009044809578, 0.011231121314898345, 0.01090946437169898, {{
            LossShelf{0.988356352f, 0.012397076f, 0.00100000005f},
            LossShelf{0.99999243f, 0.985007584f, 0.985000014f},
        }}},
    }};
};

#endif

