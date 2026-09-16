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

#ifndef dispersion_filter_hpp
#define dispersion_filter_hpp

#include <stdio.h>

class DispersionFilter {
    
    inline static constexpr int kRT425DispersionSectionCount = 8;
    inline static constexpr std::size_t kRT425DispersionPresetCount = 237;
    
    struct DispersionAllpass {
        float a1 = 0.0f;
        float a2 = 0.0f;
    };
    
    struct DispersionPreset {
        int midi;
        double referenceF1;
        double B;
        double sampleRate;
        double loopDelaySamples;
        int order;
        int sectionCount;
        std::array<DispersionAllpass, kRT425DispersionSectionCount> sections;
    };
    
    DispersionPreset dispersionPreset;
    std::array<float, kRT425DispersionSectionCount> x1{};
    std::array<float, kRT425DispersionSectionCount> x2{};
    std::array<float, kRT425DispersionSectionCount> y1{};
    std::array<float, kRT425DispersionSectionCount> y2{};
    
public:
    DispersionFilter(double frequency) {
        dispersionPreset = getRT425DispersionPreset(frequency);
    }
    
    inline void process(float& x) {
        for (std::size_t i = 0; i < dispersionPreset.sectionCount; ++i) {
            const auto& c = dispersionPreset.sections[i];

            // H(z) = (a2 + a1 z^-1 + z^-2)
            //      / (1  + a1 z^-1 + a2 z^-2)
            // Equivalent all-pass recurrence:
            // y[n] = x[n-2] + a1(x[n-1] - y[n-1])
            //                 + a2(x[n]   - y[n-2])
            const float y = x2[i]
                + c.a1 * (x1[i] - y1[i])
                + c.a2 * (x - y2[i]);

            x2[i] = x1[i];
            x1[i] = x;

            y2[i] = y1[i];
            y1[i] = y;

            x = y;
        }
    }
    
    inline double computeFittedB(double f0_hz) {
        constexpr double k1 = -1.3333333333333333;
        constexpr double k2 =  1.3333333333333333;

        constexpr double c0 = -6.5760753629956712;
        constexpr double c1 =  1.9137732290077178;
        constexpr double c2 =  0.27614763151078042;
        constexpr double c3 = -0.28572738557770405;
        constexpr double c4 = -0.30709648310494486;

        const double x = std::log2(f0_hz / 440.0);

        double log_b =
              c0
            + c1 * x
            + c2 * x * x
            + c3 * std::pow(std::max(x - k1, 0.0), 2.0)
            + c4 * std::pow(std::max(x - k2, 0.0), 2.0);

        log_b = std::clamp(log_b,
                           std::log(5e-6),
                           std::log(2e-2));

        return std::exp(log_b);
    }

    
    inline constexpr const DispersionPreset& getRT425DispersionPreset(double f0_hz) {
        std::size_t bestIndex = 0;
        double bestDistance = kRT425DispersionPresets[0].referenceF1 - f0_hz;
        if (bestDistance < 0.0) bestDistance = -bestDistance;

        for (std::size_t i = 1; i < kRT425DispersionPresets.size(); ++i) {
            double distance = kRT425DispersionPresets[i].referenceF1 - f0_hz;
            if (distance < 0.0) distance = -distance;
            if (distance < bestDistance) {
                bestDistance = distance;
                bestIndex = i;
            }
        }
        return kRT425DispersionPresets[bestIndex];
    }
    
    inline double get_phase_delay(double sample_rate,
                                  double frequency) const {
        const double omega =
            2.0 * std::numbers::pi_v<double> *
            frequency / sample_rate;

        const std::complex<double> z1 =
            std::polar(1.0, -omega);
        const std::complex<double> z2 = z1 * z1;

        double phase = 0.0;

        const std::size_t section_count =
            std::min<std::size_t>(
                static_cast<std::size_t>(dispersionPreset.sectionCount),
                dispersionPreset.sections.size());

        for (std::size_t i = 0; i < section_count; ++i) {
            const auto& c = dispersionPreset.sections[i];

            const double a1 = static_cast<double>(c.a1);
            const double a2 = static_cast<double>(c.a2);

            const std::complex<double> response =
                (a2 + a1 * z1 + z2) /
                (1.0 + a1 * z1 + a2 * z2);

            phase += std::arg(response);
        }

        return -phase / omega;
    }
    
    inline static constexpr std::array<DispersionPreset, 237> kRT425DispersionPresets = {{
        // String 0, MIDI 21, f1 = 27.378200000000, B = 5.48502917594e-05, maxPartial = 20, loopDelaySamples = 1259.091945924830, order = 16, sectionCount = 8
        DispersionPreset{
            21,
            27.3782,
            5.4850291759431087e-05,
            44100,
            1259.0919459248303,
            16,
            8,
            {{
                DispersionAllpass{-1.9133333740676524, 0.91526674177755118},
                DispersionAllpass{-1.9134544267290932, 0.91584222526901193},
                DispersionAllpass{-1.8576349305213771, 0.86346178201828772},
                DispersionAllpass{-1.9152797639318762, 0.91855249479737033},
                DispersionAllpass{-1.8407683018885719, 0.84958112901701632},
                DispersionAllpass{-1.9289614411584064, 0.93541448553273854},
                DispersionAllpass{-1.9203454476964985, 0.92492484015279441},
                DispersionAllpass{-1.9451134674355965, 0.95426184260164959}
            }}
        },
        // String 1, MIDI 22, f1 = 29.010100000000, B = 5.36008001169e-05, maxPartial = 20, loopDelaySamples = 1243.808063478913, order = 16, sectionCount = 8
        DispersionPreset{
            22,
            29.010100000000001,
            5.3600800116909449e-05,
            44100,
            1243.8080634789128,
            16,
            8,
            {{
                DispersionAllpass{-1.8962559876937404, 0.89904472810953573},
                DispersionAllpass{-1.8952366411615964, 0.89888738978354921},
                DispersionAllpass{-1.8005593438039058, 0.81202565534130622},
                DispersionAllpass{-1.8944841087768176, 0.89989811072087111},
                DispersionAllpass{-1.7771128442919704, 0.79298593863384026},
                DispersionAllpass{-1.8976868868509884, 0.90583061345083393},
                DispersionAllpass{-1.6970718101967543, 0.72790075669892707},
                DispersionAllpass{-1.9111059056215314, 0.92383305349320288}
            }}
        },
        // String 2, MIDI 23, f1 = 30.739300000000, B = 5.25819809932e-05, maxPartial = 20, loopDelaySamples = 1109.858048196181, order = 16, sectionCount = 8
        DispersionPreset{
            23,
            30.7393,
            5.2581980993204519e-05,
            44100,
            1109.8580481961815,
            16,
            8,
            {{
                DispersionAllpass{-1.914996627375515, 0.91687553032884495},
                DispersionAllpass{-1.9146283059524174, 0.91712324781557197},
                DispersionAllpass{-1.9153556732696435, 0.91908853167044924},
                DispersionAllpass{-1.8506776002923504, 0.85746458871653164},
                DispersionAllpass{-1.8320526153325334, 0.84187583380720599},
                DispersionAllpass{-1.9190582137600039, 0.92457402305410763},
                DispersionAllpass{-1.9268284416639019, 0.93468126936828033},
                DispersionAllpass{-1.9430208946062955, 0.95400454038468463}
            }}
        },
        // String 3, MIDI 24, f1 = 32.571600000000, B = 5.17816694449e-05, maxPartial = 20, loopDelaySamples = 1122.592272942154, order = 16, sectionCount = 8
        DispersionPreset{
            24,
            32.571599999999997,
            5.17816694448717e-05,
            44100,
            1122.5922729421536,
            16,
            8,
            {{
                DispersionAllpass{-1.849891642414355, 0.85565212795487644},
                DispersionAllpass{-1.8383144447882047, 0.84683207171046737},
                DispersionAllpass{-1.839163819705071, 0.84641636626592376},
                DispersionAllpass{-1.6958077696243521, 0.72378393098505145},
                DispersionAllpass{-1.8415132165636137, 0.85135155147748809},
                DispersionAllpass{-1.8668317873442815, 0.88603603540072651},
                DispersionAllpass{-1.7153885869751346, 0.74147529064970463},
                DispersionAllpass{-1.853873001837046, 0.86641893535780556}
            }}
        },
        // String 4, MIDI 25, f1 = 34.513100000000, B = 5.11904279941e-05, maxPartial = 20, loopDelaySamples = 1057.597726674484, order = 16, sectionCount = 8
        DispersionPreset{
            25,
            34.513100000000001,
            5.1190427994123997e-05,
            44100,
            1057.5977266744842,
            16,
            8,
            {{
                DispersionAllpass{-1.8476883243899469, 0.85359267996059396},
                DispersionAllpass{-1.8099472921190307, 0.82112315590147666},
                DispersionAllpass{-1.8463397187034998, 0.85339479377833249},
                DispersionAllpass{-1.7009826765415725, 0.72984539878786914},
                DispersionAllpass{-1.8430409106143453, 0.85320994161427866},
                DispersionAllpass{-1.8621005107367474, 0.88324718465330831},
                DispersionAllpass{-1.7062468785590752, 0.73477937535578408},
                DispersionAllpass{-1.8466567648753875, 0.86070997108388492}
            }}
        },
        // String 5, MIDI 26, f1 = 36.570300000000, B = 5.08013251947e-05, maxPartial = 20, loopDelaySamples = 1002.295202860291, order = 16, sectionCount = 8
        DispersionPreset{
            26,
            36.570300000000003,
            5.0801325194743499e-05,
            44100,
            1002.2952028602911,
            16,
            8,
            {{
                DispersionAllpass{-1.8402156267284797, 0.84678560758680255},
                DispersionAllpass{-1.8353206387464363, 0.84389650289587681},
                DispersionAllpass{-1.8311977212815505, 0.84365602096128733},
                DispersionAllpass{-1.7770128518055293, 0.79081221961729531},
                DispersionAllpass{-1.7027448440672821, 0.73166764277816176},
                DispersionAllpass{-1.7044121585305076, 0.73266431737346605},
                DispersionAllpass{-1.8279462191112206, 0.85492756322092933},
                DispersionAllpass{-1.8218571136319708, 0.84067322009348755}
            }}
        },
        // String 6, MIDI 27, f1 = 38.750200000000, B = 5.06098193789e-05, maxPartial = 20, loopDelaySamples = 950.292050693972, order = 16, sectionCount = 8
        DispersionPreset{
            27,
            38.7502,
            5.0609819378936862e-05,
            44100,
            950.29205069397244,
            16,
            8,
            {{
                DispersionAllpass{-1.8076293580481806, 0.81703266233121452},
                DispersionAllpass{-1.8057509685463966, 0.81700303042724987},
                DispersionAllpass{-1.8082938841061709, 0.82387034188812147},
                DispersionAllpass{-1.7678438195290798, 0.78295672742906819},
                DispersionAllpass{-1.6910886320337781, 0.7180882918702991},
                DispersionAllpass{-1.7077438297687075, 0.73753714381764246},
                DispersionAllpass{-1.7930799308389362, 0.82162723716406838},
                DispersionAllpass{-1.7654278481196055, 0.79471806851576221}
            }}
        },
        // String 7, MIDI 28, f1 = 41.060000000000, B = 5.06137047955e-05, maxPartial = 20, loopDelaySamples = 895.918287344008, order = 16, sectionCount = 8
        DispersionPreset{
            28,
            41.060000000000002,
            5.0613704795535706e-05,
            44100,
            895.91828734400815,
            16,
            8,
            {{
                DispersionAllpass{-1.7947649594677253, 0.80541792483595953},
                DispersionAllpass{-1.7949911550744653, 0.80665027797435851},
                DispersionAllpass{-1.7667998139688865, 0.78187636368808699},
                DispersionAllpass{-1.8054782992038116, 0.81924853929874175},
                DispersionAllpass{-1.8007556719624886, 0.82061598928981883},
                DispersionAllpass{-1.7187518902672014, 0.76117259625332445},
                DispersionAllpass{-1.7201241138866565, 0.76207304594024927},
                DispersionAllpass{-1.7666491884800495, 0.79863128623524049}
            }}
        },
        // String 8, MIDI 29, f1 = 43.507500000000, B = 5.08130237243e-05, maxPartial = 20, loopDelaySamples = 844.839925779423, order = 16, sectionCount = 8
        DispersionPreset{
            29,
            43.5075,
            5.081302372431709e-05,
            44100,
            844.83992577942252,
            16,
            8,
            {{
                DispersionAllpass{-1.7614918471814245, 0.77576591545295581},
                DispersionAllpass{-1.7404390605067657, 0.75819280879008499},
                DispersionAllpass{-1.7071599321749937, 0.72976151332551797},
                DispersionAllpass{-1.782826089836731, 0.79682510166342757},
                DispersionAllpass{-1.792190179736, 0.81022198048771865},
                DispersionAllpass{-1.7128094546604291, 0.75709006453480243},
                DispersionAllpass{-1.7598136563690108, 0.791095177772418},
                DispersionAllpass{-1.7487194552621717, 0.78506240587308496}
            }}
        },
        // String 9, MIDI 30, f1 = 46.100800000000, B = 5.12100718314e-05, maxPartial = 20, loopDelaySamples = 799.810325651239, order = 16, sectionCount = 8
        DispersionPreset{
            30,
            46.1008,
            5.1210071831366423e-05,
            44100,
            799.81032565123883,
            16,
            8,
            {{
                DispersionAllpass{-1.7423991136229982, 0.7592007756456457},
                DispersionAllpass{-1.7171000427215517, 0.73799357627067674},
                DispersionAllpass{-1.6973328976538502, 0.7213291647385569},
                DispersionAllpass{-1.7567766603139194, 0.77468566476800949},
                DispersionAllpass{-1.7590006138225922, 0.78219456694367706},
                DispersionAllpass{-1.7016085055090904, 0.74490931700450469},
                DispersionAllpass{-1.7102074011119266, 0.75117452372589211},
                DispersionAllpass{-1.703116503514122, 0.7456183727925707}
            }}
        },
        // String 10, MIDI 31, f1 = 48.824800000000, B = 5.18035490072e-05, maxPartial = 20, loopDelaySamples = 757.069305055675, order = 16, sectionCount = 8
        DispersionPreset{
            31,
            48.824800000000003,
            5.180354900715938e-05,
            44100,
            757.0693050556747,
            16,
            8,
            {{
                DispersionAllpass{-1.7001533644270097, 0.72275827802605941},
                DispersionAllpass{-1.6997123665241072, 0.72280046689698663},
                DispersionAllpass{-1.6919165667012197, 0.72278321769644582},
                DispersionAllpass{-1.6966414371695615, 0.72271061849945384},
                DispersionAllpass{-1.6850167160660152, 0.72276497028826547},
                DispersionAllpass{-1.6958319652160967, 0.72279345087495395},
                DispersionAllpass{-1.6795877188914103, 0.7249015456301211},
                DispersionAllpass{-1.6901121992142487, 0.73774083026770487}
            }}
        },
        // String 11, MIDI 31, f1 = 48.848800000000, B = 5.18095062475e-05, maxPartial = 20, loopDelaySamples = 752.695413165866, order = 16, sectionCount = 8
        DispersionPreset{
            31,
            48.848799999999997,
            5.1809506247468992e-05,
            44100,
            752.69541316586628,
            16,
            8,
            {{
                DispersionAllpass{-1.7375422680376851, 0.75483699331140874},
                DispersionAllpass{-1.7290797958205815, 0.74970597423500274},
                DispersionAllpass{-1.7223318280937894, 0.74376574668493112},
                DispersionAllpass{-1.6981594299918408, 0.72377512853456738},
                DispersionAllpass{-1.7461732630271314, 0.77074264481377452},
                DispersionAllpass{-1.7290855663352305, 0.76398163141990183},
                DispersionAllpass{-1.7009663152857075, 0.75050046112877422},
                DispersionAllpass{-1.6871197125170578, 0.74035739223168384}
            }}
        },
        // String 12, MIDI 32, f1 = 51.735100000000, B = 5.26105452165e-05, maxPartial = 20, loopDelaySamples = 712.295404310633, order = 16, sectionCount = 8
        DispersionPreset{
            32,
            51.735100000000003,
            5.2610545216462892e-05,
            44100,
            712.29540431063322,
            16,
            8,
            {{
                DispersionAllpass{-1.6995359314437244, 0.72251967479630597},
                DispersionAllpass{-1.6994249395751824, 0.72252048529110391},
                DispersionAllpass{-1.6969047323406736, 0.72249551028781001},
                DispersionAllpass{-1.6918850738393643, 0.72255116439214651},
                DispersionAllpass{-1.6916350708494632, 0.72255263895810484},
                DispersionAllpass{-1.6810734231624644, 0.72247385741938996},
                DispersionAllpass{-1.6694807855498601, 0.72255683845132879},
                DispersionAllpass{-1.6691009848210154, 0.72255786935136213}
            }}
        },
        // String 13, MIDI 32, f1 = 51.760500000000, B = 5.26183069013e-05, maxPartial = 20, loopDelaySamples = 711.922728508512, order = 16, sectionCount = 8
        DispersionPreset{
            32,
            51.7605,
            5.2618306901262375e-05,
            44100,
            711.92272850851214,
            16,
            8,
            {{
                DispersionAllpass{-1.6995338100476376, 0.72251541486905946},
                DispersionAllpass{-1.6994148320237079, 0.7225162700010197},
                DispersionAllpass{-1.6969067291130795, 0.72249872424624961},
                DispersionAllpass{-1.6918647581613806, 0.72254408236626899},
                DispersionAllpass{-1.6916113474420211, 0.72254505247162804},
                DispersionAllpass{-1.6810643868266368, 0.72247764283700755},
                DispersionAllpass{-1.6694490275125169, 0.72255192566848514},
                DispersionAllpass{-1.6690140162415323, 0.72255221656290847}
            }}
        },
        // String 14, MIDI 33, f1 = 54.818900000000, B = 5.36364091048e-05, maxPartial = 20, loopDelaySamples = 669.582483320481, order = 16, sectionCount = 8
        DispersionPreset{
            33,
            54.818899999999999,
            5.3636409104809483e-05,
            44100,
            669.58248332048106,
            16,
            8,
            {{
                DispersionAllpass{-1.6998886356154994, 0.72250076398933083},
                DispersionAllpass{-1.6986196479966593, 0.72249995718346172},
                DispersionAllpass{-1.6964708163749407, 0.72250113721545262},
                DispersionAllpass{-1.6925027590464574, 0.72249911746886497},
                DispersionAllpass{-1.6870330656616701, 0.72250170722850726},
                DispersionAllpass{-1.6803800543890399, 0.72249874869609998},
                DispersionAllpass{-1.6630574657752661, 0.72250434834239696},
                DispersionAllpass{-1.6617629142088104, 0.72250454339428971}
            }}
        },
        // String 15, MIDI 33, f1 = 54.845800000000, B = 5.36460696058e-05, maxPartial = 20, loopDelaySamples = 669.237725113489, order = 16, sectionCount = 8
        DispersionPreset{
            33,
            54.845799999999997,
            5.3646069605784835e-05,
            44100,
            669.2377251134892,
            16,
            8,
            {{
                DispersionAllpass{-1.6998704085173169, 0.72250031037633289},
                DispersionAllpass{-1.6986701255145085, 0.72250002682156234},
                DispersionAllpass{-1.6963975300314591, 0.72250183438669946},
                DispersionAllpass{-1.6925538295050353, 0.72249793396558404},
                DispersionAllpass{-1.6869798139095549, 0.72250310866376033},
                DispersionAllpass{-1.6803419645649149, 0.72249890635724134},
                DispersionAllpass{-1.6610626376884539, 0.72250526303880358},
                DispersionAllpass{-1.6636181879262499, 0.72250503561281665}
            }}
        },
        // String 16, MIDI 34, f1 = 58.084000000000, B = 5.4892370406e-05, maxPartial = 20, loopDelaySamples = 629.452616537638, order = 16, sectionCount = 8
        DispersionPreset{
            34,
            58.084000000000003,
            5.4892370405986803e-05,
            44100,
            629.45261653763816,
            16,
            8,
            {{
                DispersionAllpass{-1.6998238858463566, 0.72249996920085247},
                DispersionAllpass{-1.6985432756557206, 0.72249997548437306},
                DispersionAllpass{-1.6956804183131877, 0.72250000130502678},
                DispersionAllpass{-1.6915620287948385, 0.7224999852466466},
                DispersionAllpass{-1.676954536735616, 0.7224999742722531},
                DispersionAllpass{-1.6611670146851052, 0.72250004295960946},
                DispersionAllpass{-1.6494032563724281, 0.7225000803698105},
                DispersionAllpass{-1.6847739431323165, 0.72250003226248205}
            }}
        },
        // String 17, MIDI 34, f1 = 58.112600000000, B = 5.49040885383e-05, maxPartial = 20, loopDelaySamples = 629.134364725318, order = 16, sectionCount = 8
        DispersionPreset{
            34,
            58.1126,
            5.4904088538290463e-05,
            44100,
            629.13436472531828,
            16,
            8,
            {{
                DispersionAllpass{-1.6998318200414111, 0.72250006292412972},
                DispersionAllpass{-1.6985172500669112, 0.72249995133306799},
                DispersionAllpass{-1.695717493137447, 0.72250004504206622},
                DispersionAllpass{-1.6914858389922647, 0.72249997654215325},
                DispersionAllpass{-1.6848524497714799, 0.72250008830126},
                DispersionAllpass{-1.6767831210523685, 0.72249990097834493},
                DispersionAllpass{-1.6613748942868403, 0.72250013022309978},
                DispersionAllpass{-1.6490256183656171, 0.72250025058372069}
            }}
        },
        // String 18, MIDI 35, f1 = 61.543700000000, B = 5.63943726166e-05, maxPartial = 20, loopDelaySamples = 591.636827626105, order = 16, sectionCount = 8
        DispersionPreset{
            35,
            61.543700000000001,
            5.6394372616627355e-05,
            44100,
            591.6368276261046,
            16,
            8,
            {{
                DispersionAllpass{-1.6998020259240711, 0.7224998623352421},
                DispersionAllpass{-1.6983076586599959, 0.72250008672099342},
                DispersionAllpass{-1.6950646926519575, 0.72249972488708281},
                DispersionAllpass{-1.6902011750156007, 0.72250024591525464},
                DispersionAllpass{-1.6826371005114473, 0.72249969895488075},
                DispersionAllpass{-1.6729625647857045, 0.7225003498881698},
                DispersionAllpass{-1.6565210641338461, 0.72249951952103586},
                DispersionAllpass{-1.6384548310381921, 0.72249967881841337}
            }}
        },
        // String 19, MIDI 35, f1 = 61.573900000000, B = 5.64082107169e-05, maxPartial = 20, loopDelaySamples = 591.354938210095, order = 16, sectionCount = 8
        DispersionPreset{
            35,
            61.573900000000002,
            5.6408210716939515e-05,
            44100,
            591.3549382100947,
            16,
            8,
            {{
                DispersionAllpass{-1.699807766409094, 0.72250000902807243},
                DispersionAllpass{-1.6982867731292648, 0.72250002555026283},
                DispersionAllpass{-1.6950910728230273, 0.72249999210497984},
                DispersionAllpass{-1.690129248394513, 0.72250002013265779},
                DispersionAllpass{-1.6827032896335805, 0.72249998551166672},
                DispersionAllpass{-1.6567571475563696, 0.7224999928873983},
                DispersionAllpass{-1.6378921157576238, 0.7224999987326084},
                DispersionAllpass{-1.6727566582761961, 0.72250000747918353}
            }}
        },
        // String 20, MIDI 36, f1 = 65.209400000000, B = 5.81608103117e-05, maxPartial = 20, loopDelaySamples = 556.031017825686, order = 16, sectionCount = 8
        DispersionPreset{
            36,
            65.209400000000002,
            5.8160810311740388e-05,
            44100,
            556.03101782568638,
            16,
            8,
            {{
                DispersionAllpass{-1.6997831383777113, 0.72249999928674924},
                DispersionAllpass{-1.6980341612335357, 0.72250002599876106},
                DispersionAllpass{-1.6944096371509345, 0.72249997861645987},
                DispersionAllpass{-1.6886775051380074, 0.7225000218610752},
                DispersionAllpass{-1.6802921680972294, 0.72249998704325602},
                DispersionAllpass{-1.6510185237876591, 0.72249999554585032},
                DispersionAllpass{-1.6685713139908427, 0.7225000143849678},
                DispersionAllpass{-1.6265448468281067, 0.72249999835237366}
            }}
        },
        // String 21, MIDI 36, f1 = 65.241400000000, B = 5.81769806251e-05, maxPartial = 20, loopDelaySamples = 555.737740601119, order = 16, sectionCount = 8
        DispersionPreset{
            36,
            65.241399999999999,
            5.8176980625087649e-05,
            44100,
            555.7377406011185,
            16,
            8,
            {{
                DispersionAllpass{-1.699782906183021, 0.72249999584122926},
                DispersionAllpass{-1.6980319200891458, 0.72250001543336673},
                DispersionAllpass{-1.6944034967409731, 0.72249998012256955},
                DispersionAllpass{-1.6886646391192768, 0.72250001501072059},
                DispersionAllpass{-1.6802703787728486, 0.72249998371930058},
                DispersionAllpass{-1.6509656401126338, 0.7224999962020181},
                DispersionAllpass{-1.6685344178670618, 0.72250000856562258},
                DispersionAllpass{-1.6264450746712018, 0.72250000300864126}
            }}
        },
        // String 22, MIDI 37, f1 = 69.093400000000, B = 6.0213806022e-05, maxPartial = 20, loopDelaySamples = 522.421292919666, order = 16, sectionCount = 8
        DispersionPreset{
            37,
            69.093400000000003,
            6.0213806022023323e-05,
            44100,
            522.42129291966637,
            16,
            8,
            {{
                DispersionAllpass{-1.6997540727960114, 0.72249999863106706},
                DispersionAllpass{-1.6977640972885732, 0.72250002491303678},
                DispersionAllpass{-1.6936484298982191, 0.72249997378518249},
                DispersionAllpass{-1.6871130710640365, 0.72250002399170321},
                DispersionAllpass{-1.6775882553550965, 0.72249998020191419},
                DispersionAllpass{-1.6641098258015663, 0.72250000454744168},
                DispersionAllpass{-1.6443366420552838, 0.72249999953850463},
                DispersionAllpass{-1.6146104407701138, 0.72250000216287624}
            }}
        },
        // String 23, MIDI 37, f1 = 69.127300000000, B = 6.02325090955e-05, maxPartial = 20, loopDelaySamples = 522.144638274499, order = 16, sectionCount = 8
        DispersionPreset{
            37,
            69.127300000000005,
            6.0232509095508895e-05,
            44100,
            522.1446382744989,
            16,
            8,
            {{
                DispersionAllpass{-1.699753811654926, 0.72249999716354718},
                DispersionAllpass{-1.6977617236119014, 0.7225000248190212},
                DispersionAllpass{-1.6936416875160016, 0.72249997134178467},
                DispersionAllpass{-1.687099313232338, 0.72250002930031232},
                DispersionAllpass{-1.677564274297199, 0.72249997652996945},
                DispersionAllpass{-1.6640707241206543, 0.72250000863154062},
                DispersionAllpass{-1.6442767009446595, 0.72249999431149192},
                DispersionAllpass{-1.6145067936866526, 0.72250000056028563}
            }}
        },
        // String 24, MIDI 38, f1 = 73.208800000000, B = 6.25796365424e-05, maxPartial = 20, loopDelaySamples = 490.722273316242, order = 16, sectionCount = 8
        DispersionPreset{
            38,
            73.208799999999997,
            6.2579636542420321e-05,
            44100,
            490.72227331624174,
            16,
            8,
            {{
                DispersionAllpass{-1.6997222082042525, 0.72249998999101395},
                DispersionAllpass{-1.697472957654615, 0.72250003372236005},
                DispersionAllpass{-1.6928211300124694, 0.72249995637853759},
                DispersionAllpass{-1.6854244864198544, 0.72250003844639576},
                DispersionAllpass{-1.6746429144009611, 0.7224999762746791},
                DispersionAllpass{-1.6593162871494396, 0.72250001059143498},
                DispersionAllpass{-1.6019621917471381, 0.72250000235598133},
                DispersionAllpass{-1.6369363753133999, 0.72249999505124052}
            }}
        },
        // String 25, MIDI 38, f1 = 73.244700000000, B = 6.26011052198e-05, maxPartial = 20, loopDelaySamples = 490.461593635967, order = 16, sectionCount = 8
        DispersionPreset{
            38,
            73.244699999999995,
            6.2601105219795922e-05,
            44100,
            490.46159363596712,
            16,
            8,
            {{
                DispersionAllpass{-1.6997219285650005, 0.72249999090446948},
                DispersionAllpass{-1.6974703869751795, 0.72250003137835728},
                DispersionAllpass{-1.6928138443013385, 0.72249996409847195},
                DispersionAllpass{-1.6854095829933415, 0.72250003367072535},
                DispersionAllpass{-1.6746169355636489, 0.72249998265540738},
                DispersionAllpass{-1.6592739723399299, 0.72250000507152756},
                DispersionAllpass{-1.6018505043012445, 0.72249999956087685},
                DispersionAllpass{-1.6368709203714384, 0.72249999892547068}
            }}
        },
        // String 26, MIDI 38, f1 = 73.280700000000, B = 6.26226478867e-05, maxPartial = 20, loopDelaySamples = 490.200342500390, order = 16, sectionCount = 8
        DispersionPreset{
            38,
            73.280699999999996,
            6.2622647886681856e-05,
            44100,
            490.20034250038987,
            16,
            8,
            {{
                DispersionAllpass{-1.6997216419180137, 0.72249998687163342},
                DispersionAllpass{-1.6974678186380923, 0.7225000310961639},
                DispersionAllpass{-1.692806531856065, 0.72249996191447108},
                DispersionAllpass{-1.6853946870598753, 0.72250003573990085},
                DispersionAllpass{-1.6745908872305837, 0.72249997953763512},
                DispersionAllpass{-1.6592317437130351, 0.72250000798528069},
                DispersionAllpass{-1.6368051076837071, 0.72249999634645545},
                DispersionAllpass{-1.6017399627306048, 0.72249999869994852}
            }}
        },
        // String 27, MIDI 39, f1 = 77.569200000000, B = 6.52890844683e-05, maxPartial = 20, loopDelaySamples = 460.747859948495, order = 16, sectionCount = 8
        DispersionPreset{
            39,
            77.569199999999995,
            6.5289084468286726e-05,
            44100,
            460.74785994849515,
            16,
            8,
            {{
                DispersionAllpass{-1.6996756416389316, 0.72248932527909093},
                DispersionAllpass{-1.6971697547281543, 0.7225049149550643},
                DispersionAllpass{-1.6919219658125895, 0.722486224941849},
                DispersionAllpass{-1.683651455495651, 0.722513344556871},
                DispersionAllpass{-1.6714846722044212, 0.72249387309271584},
                DispersionAllpass{-1.6288398134301234, 0.72249820673008669},
                DispersionAllpass{-1.5896460761869085, 0.72250197029315688},
                DispersionAllpass{-1.6543029763805999, 0.7225076904874197}
            }}
        },
        // String 28, MIDI 39, f1 = 77.607400000000, B = 6.53137165056e-05, maxPartial = 20, loopDelaySamples = 460.580974814853, order = 16, sectionCount = 8
        DispersionPreset{
            39,
            77.607399999999998,
            6.5313716505639545e-05,
            44100,
            460.58097481485282,
            16,
            8,
            {{
                DispersionAllpass{-1.6996874701458808, 0.72249999010609878},
                DispersionAllpass{-1.6971563025761545, 0.72250001555776844},
                DispersionAllpass{-1.6919200366463154, 0.72249997400481514},
                DispersionAllpass{-1.6835872199207869, 0.722500023731658},
                DispersionAllpass{-1.671430944005367, 0.72249998746126709},
                DispersionAllpass{-1.6288043951232187, 0.72249999904997941},
                DispersionAllpass{-1.588345008128073, 0.72250000104288692},
                DispersionAllpass{-1.6541064473981697, 0.72250000479111398}
            }}
        },
        // String 29, MIDI 39, f1 = 77.645500000000, B = 6.53382993587e-05, maxPartial = 20, loopDelaySamples = 460.334941475267, order = 16, sectionCount = 8
        DispersionPreset{
            39,
            77.645499999999998,
            6.5338299358686647e-05,
            44100,
            460.33494147526739,
            16,
            8,
            {{
                DispersionAllpass{-1.6996871700957972, 0.72249999304681123},
                DispersionAllpass{-1.6971535341248971, 0.72250001342669823},
                DispersionAllpass{-1.6919121701717135, 0.72249997925436704},
                DispersionAllpass{-1.6835711615439193, 0.72250002044234019},
                DispersionAllpass{-1.6714028763058959, 0.72249999258474651},
                DispersionAllpass{-1.6287332007804143, 0.72250000037423756},
                DispersionAllpass{-1.588226007531758, 0.72250000084658916},
                DispersionAllpass{-1.6540608990745695, 0.72250000384834279}
            }}
        },
        // String 30, MIDI 40, f1 = 82.189500000000, B = 6.83785649837e-05, maxPartial = 20, loopDelaySamples = 432.634625120599, order = 16, sectionCount = 8
        DispersionPreset{
            40,
            82.189499999999995,
            6.8378564983695274e-05,
            44100,
            432.63462512059914,
            16,
            8,
            {{
                DispersionAllpass{-1.6996504468642861, 0.7224997366317929},
                DispersionAllpass{-1.648596291584872, 0.72250016448205057},
                DispersionAllpass{-1.6201648846953298, 0.72249995774757159},
                DispersionAllpass{-1.6968217249763828, 0.72250006162970348},
                DispersionAllpass{-1.5740118529341287, 0.72250004030852077},
                DispersionAllpass{-1.6680314819149251, 0.72249980314284723},
                DispersionAllpass{-1.6816449343867013, 0.72250030513670405},
                DispersionAllpass{-1.6909670957460063, 0.72249984757414343}
            }}
        },
        // String 31, MIDI 40, f1 = 82.229900000000, B = 6.84065503468e-05, maxPartial = 20, loopDelaySamples = 432.402690240811, order = 16, sectionCount = 8
        DispersionPreset{
            40,
            82.229900000000001,
            6.8406550346842109e-05,
            44100,
            432.40269024081135,
            16,
            8,
            {{
                DispersionAllpass{-1.6996503559496678, 0.72249994328854539},
                DispersionAllpass{-1.6968185840303267, 0.72249994591564004},
                DispersionAllpass{-1.690958670522225, 0.72249994956870645},
                DispersionAllpass{-1.6816272718141707, 0.72250013528779844},
                DispersionAllpass{-1.668001074711349, 0.72249985541107153},
                DispersionAllpass{-1.6485464069925577, 0.7225001257628324},
                DispersionAllpass{-1.5738765565501149, 0.72250002401696178},
                DispersionAllpass{-1.6200877979988777, 0.72249996400074123}
            }}
        },
        // String 32, MIDI 40, f1 = 82.270200000000, B = 6.84344830608e-05, maxPartial = 20, loopDelaySamples = 432.169515895706, order = 16, sectionCount = 8
        DispersionPreset{
            40,
            82.270200000000003,
            6.8434483060773974e-05,
            44100,
            432.16951589570584,
            16,
            8,
            {{
                DispersionAllpass{-1.6996498461367375, 0.72249977441167545},
                DispersionAllpass{-1.6968158815409899, 0.72250009023140882},
                DispersionAllpass{-1.6909501436462777, 0.72249964784843779},
                DispersionAllpass{-1.6816111219355241, 0.72250047759470559},
                DispersionAllpass{-1.6679716377666665, 0.72249965729964283},
                DispersionAllpass{-1.6485004711984277, 0.72250027358568858},
                DispersionAllpass{-1.5737736580755568, 0.72250005276253393},
                DispersionAllpass{-1.6200120555668835, 0.72249992540824814}
            }}
        },
        // String 33, MIDI 41, f1 = 87.084900000000, B = 7.18902661023e-05, maxPartial = 20, loopDelaySamples = 405.821585044086, order = 16, sectionCount = 8
        DispersionPreset{
            41,
            87.084900000000005,
            7.1890266102344605e-05,
            44100,
            405.82158504408585,
            16,
            8,
            {{
                DispersionAllpass{-1.6995910059911867, 0.72248097293476587},
                DispersionAllpass{-1.6964941950239409, 0.72251649642661497},
                DispersionAllpass{-1.6899386616215408, 0.7224613079455493},
                DispersionAllpass{-1.6797092816462382, 0.72255087128638629},
                DispersionAllpass{-1.6644780452710799, 0.72246345856148375},
                DispersionAllpass{-1.6430897851975239, 0.72252350970737456},
                DispersionAllpass{-1.5623044207049954, 0.7225034701017945},
                DispersionAllpass{-1.6110727187442442, 0.72249448353011747}
            }}
        },
        // String 34, MIDI 41, f1 = 87.127700000000, B = 7.19220346268e-05, maxPartial = 20, loopDelaySamples = 405.689701804102, order = 16, sectionCount = 8
        DispersionPreset{
            41,
            87.127700000000004,
            7.1922034626807979e-05,
            44100,
            405.6897018041019,
            16,
            8,
            {{
                DispersionAllpass{-1.699600138431099, 0.72248981319927641},
                DispersionAllpass{-1.6964779394085983, 0.72251022976463086},
                DispersionAllpass{-1.6899300435636639, 0.72247703298041921},
                DispersionAllpass{-1.6796250758978037, 0.72252951510065111},
                DispersionAllpass{-1.6643988910871519, 0.72247735194142337},
                DispersionAllpass{-1.6428603318655368, 0.72251429188129657},
                DispersionAllpass{-1.6108938778173372, 0.72249681602879379},
                DispersionAllpass{-1.5607154806628356, 0.7225020526778726}
            }}
        },
        // String 35, MIDI 41, f1 = 87.170500000000, B = 7.19538215322e-05, maxPartial = 20, loopDelaySamples = 405.631132006559, order = 16, sectionCount = 8
        DispersionPreset{
            41,
            87.170500000000004,
            7.1953821532202286e-05,
            44100,
            405.63113200655948,
            16,
            8,
            {{
                DispersionAllpass{-1.6996109437452041, 0.72250065276065134},
                DispersionAllpass{-1.6642747669542131, 0.7225008529722774},
                DispersionAllpass{-1.6105984383221656, 0.72250005797730088},
                DispersionAllpass{-1.6899177869607482, 0.72250218515146669},
                DispersionAllpass{-1.5578880462695255, 0.72249998434376694},
                DispersionAllpass{-1.6424895740938525, 0.7224997415126404},
                DispersionAllpass{-1.6964503820568391, 0.72249833529032437},
                DispersionAllpass{-1.6794956869648237, 0.72249824732112666}
            }}
        },
        // String 36, MIDI 42, f1 = 92.271900000000, B = 7.58737199656e-05, maxPartial = 20, loopDelaySamples = 380.276416678670, order = 16, sectionCount = 8
        DispersionPreset{
            42,
            92.271900000000002,
            7.5873719965610862e-05,
            44100,
            380.27641667866976,
            16,
            8,
            {{
                DispersionAllpass{-1.6995236894303845, 0.72245586797854178},
                DispersionAllpass{-1.6961144703683908, 0.72248881798813736},
                DispersionAllpass{-1.6889252848111034, 0.72242818490721261},
                DispersionAllpass{-1.6778498192310525, 0.72266395254076499},
                DispersionAllpass{-1.6608583311759355, 0.72234767732428229},
                DispersionAllpass{-1.6378422104775594, 0.72260499994502747},
                DispersionAllpass{-1.6019276722641838, 0.72247830373051769},
                DispersionAllpass{-1.5543291339302912, 0.72251231938231386}
            }}
        },
        // String 37, MIDI 42, f1 = 92.317200000000, B = 7.59096902374e-05, maxPartial = 20, loopDelaySamples = 380.175830933929, order = 16, sectionCount = 8
        DispersionPreset{
            42,
            92.3172,
            7.5909690237425587e-05,
            44100,
            380.17583093392943,
            16,
            8,
            {{
                DispersionAllpass{-1.6995382370256278, 0.72246955850956973},
                DispersionAllpass{-1.6961040470233679, 0.72248983611468376},
                DispersionAllpass{-1.6889068515167407, 0.72244312991091297},
                DispersionAllpass{-1.6777376258555525, 0.72263147843560149},
                DispersionAllpass{-1.6607667547151461, 0.72237503703704742},
                DispersionAllpass{-1.6375330868257429, 0.72258265090691765},
                DispersionAllpass{-1.6017188989964548, 0.72248245435656644},
                DispersionAllpass{-1.5523407812850025, 0.72250976083533924}
            }}
        },
        // String 38, MIDI 42, f1 = 92.362500000000, B = 7.59456809191e-05, maxPartial = 20, loopDelaySamples = 380.464711788161, order = 16, sectionCount = 8
        DispersionPreset{
            42,
            92.362499999999997,
            7.594568091914677e-05,
            44100,
            380.46471178816137,
            16,
            8,
            {{
                DispersionAllpass{-1.6995733526965384, 0.72250394704214194},
                DispersionAllpass{-1.6773344002230828, 0.72252083266675493},
                DispersionAllpass{-1.600779249576247, 0.72249682141289007},
                DispersionAllpass{-1.6888317023561636, 0.72249093545378551},
                DispersionAllpass{-1.5434121835088184, 0.72250172017088266},
                DispersionAllpass{-1.6363654777982675, 0.72251696837829005},
                DispersionAllpass{-1.6960690420664002, 0.72249558552263438},
                DispersionAllpass{-1.6604118513838761, 0.72247408372159461}
            }}
        },
        // String 39, MIDI 43, f1 = 97.767800000000, B = 8.03865767433e-05, maxPartial = 20, loopDelaySamples = 356.633149185538, order = 16, sectionCount = 8
        DispersionPreset{
            43,
            97.767799999999994,
            8.0386576743260001e-05,
            44100,
            356.63314918553772,
            16,
            8,
            {{
                DispersionAllpass{-1.6995203930918481, 0.72249214916538951},
                DispersionAllpass{-1.6956380128112696, 0.72242535073391034},
                DispersionAllpass{-1.6878631429044297, 0.72249691621945467},
                DispersionAllpass{-1.6754719509849818, 0.72264618614598386},
                DispersionAllpass{-1.6567512716227191, 0.72231845201942646},
                DispersionAllpass{-1.5917671152472002, 0.72247443630354613},
                DispersionAllpass{-1.6312003894888274, 0.72262188276616612},
                DispersionAllpass{-1.5375268317932627, 0.72251010780947111}
            }}
        },
        // String 40, MIDI 43, f1 = 97.815800000000, B = 8.04273133708e-05, maxPartial = 20, loopDelaySamples = 356.719644256316, order = 16, sectionCount = 8
        DispersionPreset{
            43,
            97.815799999999996,
            8.0427313370761084e-05,
            44100,
            356.71964425631614,
            16,
            8,
            {{
                DispersionAllpass{-1.6995301899230222, 0.72250291898873609},
                DispersionAllpass{-1.6877766703746817, 0.72250223363256627},
                DispersionAllpass{-1.6752066588457046, 0.72257334267173479},
                DispersionAllpass{-1.6304340458314057, 0.72257366850058846},
                DispersionAllpass{-1.5910416408702337, 0.72248512295917477},
                DispersionAllpass{-1.5320935114597809, 0.72250608433211161},
                DispersionAllpass{-1.6956532204692791, 0.7224631676321156},
                DispersionAllpass{-1.6565031331386935, 0.72238528424353321}
            }}
        },
        // String 41, MIDI 43, f1 = 97.863900000000, B = 8.04681578501e-05, maxPartial = 20, loopDelaySamples = 357.777515063201, order = 16, sectionCount = 8
        DispersionPreset{
            43,
            97.863900000000001,
            8.0468157850107554e-05,
            44100,
            357.77751506320101,
            16,
            8,
            {{
                DispersionAllpass{-1.6995097732950764, 0.72249589807872483},
                DispersionAllpass{-1.6874373346007912, 0.72251604406394654},
                DispersionAllpass{-1.6274781247990702, 0.72249577126760867},
                DispersionAllpass{-1.6743438508175856, 0.72247512062855279},
                DispersionAllpass{-1.5064719060300134, 0.72249985134514982},
                DispersionAllpass{-1.5868643198568135, 0.72250064228277211},
                DispersionAllpass{-1.6955811777855634, 0.72250067318077704},
                DispersionAllpass{-1.6552417665868573, 0.72251551972467787}
            }}
        },
        // String 42, MIDI 44, f1 = 103.591000000000, B = 8.54961485645e-05, maxPartial = 20, loopDelaySamples = 335.872297812232, order = 16, sectionCount = 8
        DispersionPreset{
            44,
            103.59099999999999,
            8.5496148564465589e-05,
            44100,
            335.87229781223164,
            16,
            8,
            {{
                DispersionAllpass{-1.6994500572575866, 0.72248610434361837},
                DispersionAllpass{-1.4861727957139217, 0.72250155343364197},
                DispersionAllpass{-1.686214651964524, 0.72254323488270111},
                DispersionAllpass{-1.6506033614108195, 0.72242305059927703},
                DispersionAllpass{-1.6717903184913379, 0.72249928899741278},
                DispersionAllpass{-1.5748383405596014, 0.72249133729327908},
                DispersionAllpass{-1.6951359001704673, 0.72249509737931072},
                DispersionAllpass{-1.6202021554724892, 0.72254895304678834}
            }}
        },
        // String 43, MIDI 44, f1 = 103.642000000000, B = 8.55423948328e-05, maxPartial = 20, loopDelaySamples = 333.873440930550, order = 16, sectionCount = 8
        DispersionPreset{
            44,
            103.642,
            8.5542394832813175e-05,
            44100,
            333.87344093054986,
            16,
            8,
            {{
                DispersionAllpass{-1.6994419887033188, 0.72246336517022969},
                DispersionAllpass{-1.6951965112815213, 0.72238871486230394},
                DispersionAllpass{-1.6868054748213002, 0.72256675998133002},
                DispersionAllpass{-1.673137206665875, 0.72265476129924766},
                DispersionAllpass{-1.6525736329939948, 0.7221895395988871},
                DispersionAllpass{-1.5250511249727354, 0.72251757777097103},
                DispersionAllpass{-1.6249420657629625, 0.72273838413826086},
                DispersionAllpass{-1.5815192306075698, 0.7224529405997292}
            }}
        },
        // String 44, MIDI 44, f1 = 103.693000000000, B = 8.55886671645e-05, maxPartial = 20, loopDelaySamples = 333.769280255619, order = 16, sectionCount = 8
        DispersionPreset{
            44,
            103.693,
            8.5588667164501184e-05,
            44100,
            333.76928025561881,
            16,
            8,
            {{
                DispersionAllpass{-1.6994525760346113, 0.72247342168650996},
                DispersionAllpass{-1.6952023370077203, 0.72240446338371767},
                DispersionAllpass{-1.686762664957256, 0.72256233020063265},
                DispersionAllpass{-1.6730425253053702, 0.72263522821223869},
                DispersionAllpass{-1.6524702387080794, 0.72221562360314162},
                DispersionAllpass{-1.5233241690811417, 0.72251494039303943},
                DispersionAllpass{-1.6246439390263561, 0.72271366291564465},
                DispersionAllpass{-1.5812228432719242, 0.72245729537034598}
            }}
        },
        // String 45, MIDI 45, f1 = 109.761000000000, B = 9.1281010195e-05, maxPartial = 20, loopDelaySamples = 313.152304760619, order = 16, sectionCount = 8
        DispersionPreset{
            45,
            109.761,
            9.1281010195021513e-05,
            44100,
            313.1523047606189,
            16,
            8,
            {{
                DispersionAllpass{-1.6993499041263611, 0.7224262800618152},
                DispersionAllpass{-1.6948602022241928, 0.72248953369228208},
                DispersionAllpass{-1.6855538524501397, 0.72263086333043869},
                DispersionAllpass{-1.6702935951316489, 0.72252219703998977},
                DispersionAllpass{-1.6478818553009935, 0.72215720967204566},
                DispersionAllpass{-1.5697679770254473, 0.7224465060572709},
                DispersionAllpass{-1.6173234956946712, 0.72278171679309566},
                DispersionAllpass{-1.5041172522928443, 0.72251471178200266}
            }}
        },
        // String 46, MIDI 45, f1 = 109.815000000000, B = 9.13333387973e-05, maxPartial = 20, loopDelaySamples = 312.193563528595, order = 16, sectionCount = 8
        DispersionPreset{
            45,
            109.815,
            9.133333879731857e-05,
            44100,
            312.19356352859484,
            16,
            8,
            {{
                DispersionAllpass{-1.6983576210234557, 0.72156434287344184},
                DispersionAllpass{-1.5203290169470087, 0.72256016432681125},
                DispersionAllpass{-1.6862773709539502, 0.72298589679193948},
                DispersionAllpass{-1.64857342543633, 0.72182340723895599},
                DispersionAllpass{-1.6710324512350825, 0.72271021648619471},
                DispersionAllpass{-1.620039261873286, 0.72319367935124201},
                DispersionAllpass{-1.6948302436930878, 0.7223319480271434},
                DispersionAllpass{-1.5719294531419841, 0.72238477304644355}
            }}
        },
        // String 47, MIDI 45, f1 = 109.869000000000, B = 9.13856970458e-05, maxPartial = 20, loopDelaySamples = 312.658259643609, order = 16, sectionCount = 8
        DispersionPreset{
            45,
            109.869,
            9.1385697045794626e-05,
            44100,
            312.65825964360914,
            16,
            8,
            {{
                DispersionAllpass{-1.6993471622238581, 0.72242309106193414},
                DispersionAllpass{-1.5700778884857087, 0.72243280752719952},
                DispersionAllpass{-1.6479590329253133, 0.72211390139039955},
                DispersionAllpass{-1.6856019799280912, 0.7226480707844668},
                DispersionAllpass{-1.6176624635257206, 0.72282476662202055},
                DispersionAllpass{-1.6703602925574446, 0.72253044864265425},
                DispersionAllpass{-1.6948711644381611, 0.72249049690126932},
                DispersionAllpass{-1.5069040929666637, 0.72251642820905093}
            }}
        },
        // String 48, MIDI 46, f1 = 116.295000000000, B = 9.78293782259e-05, maxPartial = 20, loopDelaySamples = 292.770689778910, order = 16, sectionCount = 8
        DispersionPreset{
            46,
            116.295,
            9.7829378225853023e-05,
            44100,
            292.77068977890963,
            16,
            8,
            {{
                DispersionAllpass{-1.6991747042226448, 0.72231245158226309},
                DispersionAllpass{-1.6945495846457694, 0.72255828527951915},
                DispersionAllpass{-1.6845167663384695, 0.72272748681734889},
                DispersionAllpass{-1.6678396287724033, 0.72240935083876923},
                DispersionAllpass{-1.64374470179143, 0.72194431403813319},
                DispersionAllpass{-1.5598408973891811, 0.72240222936917364},
                DispersionAllpass{-1.6115978011961241, 0.7230612279248918},
                DispersionAllpass{-1.4968791961025825, 0.72252809665258377}
            }}
        },
        // String 49, MIDI 46, f1 = 116.352000000000, B = 9.788843876e-05, maxPartial = 20, loopDelaySamples = 292.741681683799, order = 16, sectionCount = 8
        DispersionPreset{
            46,
            116.352,
            9.7888438760023548e-05,
            44100,
            292.74168168379907,
            16,
            8,
            {{
                DispersionAllpass{-1.69921666625821, 0.72235051970058961},
                DispersionAllpass{-1.6945371344868905, 0.7225685019679694},
                DispersionAllpass{-1.6844342893950197, 0.72271252010707399},
                DispersionAllpass{-1.6677092070424711, 0.72241050768354198},
                DispersionAllpass{-1.6435256738421131, 0.72198049801792752},
                DispersionAllpass{-1.5592432688053162, 0.72241767159415238},
                DispersionAllpass{-1.6110186540022904, 0.72298476829340175},
                DispersionAllpass{-1.4937605377218486, 0.72252220608090656}
            }}
        },
        // String 50, MIDI 46, f1 = 116.410000000000, B = 9.79485700547e-05, maxPartial = 20, loopDelaySamples = 292.672119932138, order = 16, sectionCount = 8
        DispersionPreset{
            46,
            116.41,
            9.7948570054725719e-05,
            44100,
            292.6721199321384,
            16,
            8,
            {{
                DispersionAllpass{-1.6992361870323918, 0.72236882374894862},
                DispersionAllpass{-1.6945270019065022, 0.72257519282578453},
                DispersionAllpass{-1.6843781903530073, 0.72270712755887956},
                DispersionAllpass{-1.6676095096674657, 0.72241309023611811},
                DispersionAllpass{-1.6433515045192462, 0.72200271993447063},
                DispersionAllpass{-1.5587584436288109, 0.72242763695319401},
                DispersionAllpass{-1.6105931400202065, 0.72293825191344185},
                DispersionAllpass{-1.4914537113390145, 0.7225188550228353}
            }}
        },
        // String 51, MIDI 47, f1 = 123.218000000000, B = 0.000105251332886, maxPartial = 20, loopDelaySamples = 273.300945006237, order = 16, sectionCount = 8
        DispersionPreset{
            47,
            123.218,
            0.00010525133288627102,
            44100,
            273.30094500623699,
            16,
            8,
            {{
                DispersionAllpass{-1.6988359037991079, 0.72205646432762816},
                DispersionAllpass{-1.6941341246634067, 0.72251184975348925},
                DispersionAllpass{-1.6834353254315544, 0.72271347596089586},
                DispersionAllpass{-1.6654647164736505, 0.72219406213789727},
                DispersionAllpass{-1.640054407927108, 0.72176097810948237},
                DispersionAllpass{-1.6070210894769543, 0.72373429684054502},
                DispersionAllpass{-1.4946588386186752, 0.7225715534930568},
                DispersionAllpass{-1.5504078606395442, 0.72228844375337731}
            }}
        },
        // String 52, MIDI 47, f1 = 123.279000000000, B = 0.0001053189776, maxPartial = 20, loopDelaySamples = 273.128658360900, order = 16, sectionCount = 8
        DispersionPreset{
            47,
            123.279,
            0.0001053189775995625,
            44100,
            273.12865836089952,
            16,
            8,
            {{
                DispersionAllpass{-1.6988061978463305, 0.72203112198299124},
                DispersionAllpass{-1.6941211239670126, 0.7225008066874421},
                DispersionAllpass{-1.6834293239656721, 0.72271076624103281},
                DispersionAllpass{-1.6654517432413036, 0.72218841806694345},
                DispersionAllpass{-1.6400463165653356, 0.72175952391853038},
                DispersionAllpass{-1.6070466617834585, 0.72376375865461395},
                DispersionAllpass{-1.4948822768543124, 0.72257340039636753},
                DispersionAllpass{-1.5503461033445094, 0.72228251391939602}
            }}
        },
        // String 53, MIDI 47, f1 = 123.339000000000, B = 0.000105385552047, maxPartial = 20, loopDelaySamples = 272.798537184367, order = 16, sectionCount = 8
        DispersionPreset{
            47,
            123.339,
            0.00010538555204720589,
            44100,
            272.79853718436721,
            16,
            8,
            {{
                DispersionAllpass{-1.698267977015999, 0.72156689246415839},
                DispersionAllpass{-1.6939446465680537, 0.72230333988479523},
                DispersionAllpass{-1.6835345814637588, 0.72271030441895068},
                DispersionAllpass{-1.6656369293089741, 0.72219324891691328},
                DispersionAllpass{-1.6404519147348491, 0.72181129425638935},
                DispersionAllpass{-1.6080890692560086, 0.72418773372665912},
                DispersionAllpass{-1.4988394776876774, 0.72261279627560748},
                DispersionAllpass{-1.5505470506851502, 0.72220567274232828}
            }}
        },
        // String 54, MIDI 48, f1 = 130.553000000000, B = 0.000113672304487, maxPartial = 20, loopDelaySamples = 255.471629704666, order = 16, sectionCount = 8
        DispersionPreset{
            48,
            130.553,
            0.00011367230448717747,
            44100,
            255.47162970466638,
            16,
            8,
            {{
                DispersionAllpass{-1.6992679113545341, 0.72247143435115924},
                DispersionAllpass{-1.6938781448301872, 0.72272122601240285},
                DispersionAllpass{-1.6820062743298052, 0.72267644636570028},
                DispersionAllpass{-1.662537128128325, 0.72203574417765315},
                DispersionAllpass{-1.6351425530341999, 0.72157518777232377},
                DispersionAllpass{-1.5394337380711038, 0.72240542376395134},
                DispersionAllpass{-1.5992417005948607, 0.72341271130789264},
                DispersionAllpass{-1.4791767897353492, 0.72252924890426184}
            }}
        },
        // String 55, MIDI 48, f1 = 130.617000000000, B = 0.000113748353151, maxPartial = 20, loopDelaySamples = 255.322294921457, order = 16, sectionCount = 8
        DispersionPreset{
            48,
            130.61699999999999,
            0.00011374835315072723,
            44100,
            255.32229492145728,
            16,
            8,
            {{
                DispersionAllpass{-1.6992686719413708, 0.722472447265469},
                DispersionAllpass{-1.6938734932184851, 0.72272052452918423},
                DispersionAllpass{-1.6819925966658482, 0.72267367909944702},
                DispersionAllpass{-1.6625129124783797, 0.72203234712596309},
                DispersionAllpass{-1.635106964483565, 0.72157380161933715},
                DispersionAllpass{-1.5991913039320953, 0.72341566824462589},
                DispersionAllpass{-1.5393451754407743, 0.72240373272570879},
                DispersionAllpass{-1.4791093829666888, 0.72253137501680098}
            }}
        },
        // String 56, MIDI 48, f1 = 130.682000000000, B = 0.000113825636128, maxPartial = 20, loopDelaySamples = 255.171686807419, order = 16, sectionCount = 8
        DispersionPreset{
            48,
            130.68199999999999,
            0.00011382563612761529,
            44100,
            255.17168680741855,
            16,
            8,
            {{
                DispersionAllpass{-1.6992709701629334, 0.72247479321807329},
                DispersionAllpass{-1.693869718711789, 0.72272078789742411},
                DispersionAllpass{-1.6819790758694015, 0.72267159400636705},
                DispersionAllpass{-1.662488643435849, 0.72203000190807765},
                DispersionAllpass{-1.6350703931421857, 0.72157380444548214},
                DispersionAllpass{-1.599136567300063, 0.72341881574082301},
                DispersionAllpass{-1.5392548032959674, 0.72240544655553385},
                DispersionAllpass{-1.4790145788928251, 0.72253098021675799}
            }}
        },
        // String 57, MIDI 49, f1 = 138.325000000000, B = 0.000123240254445, maxPartial = 20, loopDelaySamples = 238.643565049429, order = 16, sectionCount = 8
        DispersionPreset{
            49,
            138.32499999999999,
            0.00012324025444501751,
            44100,
            238.64356504942862,
            16,
            8,
            {{
                DispersionAllpass{-1.6996023367435482, 0.72280421473253553},
                DispersionAllpass{-1.6935833515553504, 0.72290689133673214},
                DispersionAllpass{-1.6805536479812679, 0.72265510157922985},
                DispersionAllpass{-1.6595043236637244, 0.72186682547856207},
                DispersionAllpass{-1.6300088411128413, 0.72132324735597519},
                DispersionAllpass{-1.5912125472418996, 0.7230429369343262},
                DispersionAllpass{-1.5281820440707996, 0.72250704000781718},
                DispersionAllpass{-1.4632996443650548, 0.72252091040785527}
            }}
        },
        // String 58, MIDI 49, f1 = 138.393000000000, B = 0.000123326964343, maxPartial = 20, loopDelaySamples = 238.499960299486, order = 16, sectionCount = 8
        DispersionPreset{
            49,
            138.393,
            0.00012332696434276942,
            44100,
            238.49996029948585,
            16,
            8,
            {{
                DispersionAllpass{-1.6995406089277243, 0.72275153135502934},
                DispersionAllpass{-1.693548035742692, 0.72287605557712875},
                DispersionAllpass{-1.6805463940652456, 0.72265068938994814},
                DispersionAllpass{-1.6595078433487624, 0.72187961661445244},
                DispersionAllpass{-1.6300063612437707, 0.72134442417332645},
                DispersionAllpass{-1.5911884242940308, 0.72306566885060009},
                DispersionAllpass{-1.5280790787332748, 0.72249287756788505},
                DispersionAllpass{-1.4632983866941962, 0.72252213049265057}
            }}
        },
        // String 59, MIDI 49, f1 = 138.461000000000, B = 0.000123413726851, maxPartial = 20, loopDelaySamples = 238.343739528363, order = 16, sectionCount = 8
        DispersionPreset{
            49,
            138.46100000000001,
            0.00012341372685106358,
            44100,
            238.34373952836273,
            16,
            8,
            {{
                DispersionAllpass{-1.6994006344276238, 0.72263157470345363},
                DispersionAllpass{-1.6934640135159524, 0.72279764922699818},
                DispersionAllpass{-1.6805336806201032, 0.72262726299953883},
                DispersionAllpass{-1.6300821690521912, 0.7213996454535434},
                DispersionAllpass{-1.6595488311989413, 0.72190084438490165},
                DispersionAllpass{-1.5912731627170567, 0.72314232067977713},
                DispersionAllpass{-1.5279829457293865, 0.72245049176403198},
                DispersionAllpass{-1.4636552144422028, 0.72253538126505701}
            }}
        },
        // String 60, MIDI 50, f1 = 146.559000000000, B = 0.000134127214547, maxPartial = 20, loopDelaySamples = 223.120896326160, order = 16, sectionCount = 8
        DispersionPreset{
            50,
            146.559,
            0.00013412721454709035,
            44100,
            223.12089632615999,
            16,
            8,
            {{
                DispersionAllpass{-1.7000062554416631, 0.72320402613650969},
                DispersionAllpass{-1.6933522228970372, 0.72322596691706587},
                DispersionAllpass{-1.6789468260060878, 0.72275591492470925},
                DispersionAllpass{-1.6556937625423287, 0.72158561922959263},
                DispersionAllpass{-1.6230616478726738, 0.72046938025496166},
                DispersionAllpass{-1.580238044105039, 0.7215058116505163},
                DispersionAllpass{-1.5153657056993557, 0.7228137678101666},
                DispersionAllpass{-1.4374544793077411, 0.72255485414249943}
            }}
        },
        // String 61, MIDI 50, f1 = 146.631000000000, B = 0.000134225903552, maxPartial = 20, loopDelaySamples = 222.996616105690, order = 16, sectionCount = 8
        DispersionPreset{
            50,
            146.631,
            0.00013422590355159281,
            44100,
            222.99661610568981,
            16,
            8,
            {{
                DispersionAllpass{-1.6999681308986339, 0.7231719343611599},
                DispersionAllpass{-1.693318202477293, 0.72320039654942958},
                DispersionAllpass{-1.6789127629739726, 0.72273802195871251},
                DispersionAllpass{-1.6556466905588976, 0.72157060448093668},
                DispersionAllpass{-1.6229864637193003, 0.720450952929929},
                DispersionAllpass{-1.5801253004108617, 0.72148201257997047},
                DispersionAllpass{-1.5152544179347665, 0.72282596414514677},
                DispersionAllpass{-1.4371351378970667, 0.72255482054282394}
            }}
        },
        // String 62, MIDI 50, f1 = 146.703000000000, B = 0.00013432465389, maxPartial = 20, loopDelaySamples = 222.871850593142, order = 16, sectionCount = 8
        DispersionPreset{
            50,
            146.703,
            0.00013432465389000837,
            44100,
            222.87185059314237,
            16,
            8,
            {{
                DispersionAllpass{-1.699977108317535, 0.72318018078983493},
                DispersionAllpass{-1.6933175748292251, 0.72320538011557411},
                DispersionAllpass{-1.6788934280911059, 0.72273688077185094},
                DispersionAllpass{-1.6556009379415728, 0.72156169714033003},
                DispersionAllpass{-1.6229073263826397, 0.720433616683677},
                DispersionAllpass{-1.5800085137381028, 0.7214583005591485},
                DispersionAllpass{-1.5151379429939431, 0.72283661871745009},
                DispersionAllpass{-1.4368220569620063, 0.72255583858906858}
            }}
        },
        // String 63, MIDI 51, f1 = 155.284000000000, B = 0.000146539302358, maxPartial = 20, loopDelaySamples = 208.683440501069, order = 16, sectionCount = 8
        DispersionPreset{
            51,
            155.28399999999999,
            0.00014653930235788255,
            44100,
            208.68344050106879,
            16,
            8,
            {{
                DispersionAllpass{-1.6956547913974169, 0.71954239264000919},
                DispersionAllpass{-1.6896182431643645, 0.72041564640061573},
                DispersionAllpass{-1.675324915511454, 0.72090408070003553},
                DispersionAllpass{-1.6506901284189239, 0.72013731662806679},
                DispersionAllpass{-1.6149401834921524, 0.71872142219267576},
                DispersionAllpass{-1.5675526032478102, 0.71899475824076076},
                DispersionAllpass{-1.4073737153978221, 0.723391672909661},
                DispersionAllpass{-1.4998470869878111, 0.72180569465036915}
            }}
        },
        // String 64, MIDI 51, f1 = 155.360000000000, B = 0.000146651489165, maxPartial = 20, loopDelaySamples = 208.692730454535, order = 16, sectionCount = 8
        DispersionPreset{
            51,
            155.36000000000001,
            0.00014665148916490561,
            44100,
            208.69273045453497,
            16,
            8,
            {{
                DispersionAllpass{-1.6994069773649378, 0.72275790733929823},
                DispersionAllpass{-1.6761454577071619, 0.72206670299373543},
                DispersionAllpass{-1.6921038428796635, 0.72275102752656639},
                DispersionAllpass{-1.6502314535453715, 0.72038871312576247},
                DispersionAllpass{-1.6137758091028496, 0.71848474487077885},
                DispersionAllpass{-1.566213889735748, 0.718734185187626},
                DispersionAllpass{-1.4991768965328622, 0.72262744087387942},
                DispersionAllpass{-1.4012689625533328, 0.72294842857264963}
            }}
        },
        // String 65, MIDI 51, f1 = 155.436000000000, B = 0.000146763747292, maxPartial = 20, loopDelaySamples = 208.541627129566, order = 16, sectionCount = 8
        DispersionPreset{
            51,
            155.43600000000001,
            0.00014676374729231839,
            44100,
            208.54162712956611,
            16,
            8,
            {{
                DispersionAllpass{-1.6986820990563269, 0.72213716091965396},
                DispersionAllpass{-1.6915763696307524, 0.72226597656485925},
                DispersionAllpass{-1.6759079938496657, 0.72178650682014078},
                DispersionAllpass{-1.6502400222657747, 0.7202900259984093},
                DispersionAllpass{-1.6139177781503635, 0.71849159465477053},
                DispersionAllpass{-1.5663565917745681, 0.71873888441173273},
                DispersionAllpass{-1.4991262953926909, 0.72237371729759303},
                DispersionAllpass{-1.4024948809235613, 0.72307889475645937}
            }}
        },
        // String 66, MIDI 52, f1 = 164.528000000000, B = 0.000160715952042, maxPartial = 20, loopDelaySamples = 193.264390337575, order = 16, sectionCount = 8
        DispersionPreset{
            52,
            164.52799999999999,
            0.00016071595204200263,
            44100,
            193.2643903375747,
            16,
            8,
            {{
                DispersionAllpass{-1.6570001989112939, 0.68687916060408538},
                DispersionAllpass{-1.6650950284973376, 0.69821163941229569},
                DispersionAllpass{-1.6627811184439205, 0.70756515299319267},
                DispersionAllpass{-1.6454630654380598, 0.71230033143747351},
                DispersionAllpass{-1.6133621380976466, 0.71338280685727917},
                DispersionAllpass{-1.5691256836152723, 0.71425051018963048},
                DispersionAllpass{-1.5181105667281005, 0.72325348386786892},
                DispersionAllpass{-1.4702643085967664, 0.76657236648310412}
            }}
        },
        // String 67, MIDI 52, f1 = 164.609000000000, B = 0.000160844981601, maxPartial = 20, loopDelaySamples = 193.185658804788, order = 16, sectionCount = 8
        DispersionPreset{
            52,
            164.60900000000001,
            0.00016084498160109391,
            44100,
            193.18565880478843,
            16,
            8,
            {{
                DispersionAllpass{-1.6579202567202473, 0.68765407266228595},
                DispersionAllpass{-1.6652975231714613, 0.69843890973378608},
                DispersionAllpass{-1.6626575288019849, 0.70755772706034958},
                DispersionAllpass{-1.6451675980361908, 0.71218907150479249},
                DispersionAllpass{-1.6129210031232339, 0.71320511546190801},
                DispersionAllpass{-1.5684959419782929, 0.71399594766157237},
                DispersionAllpass{-1.5171962679466477, 0.72287619394231128},
                DispersionAllpass{-1.4689027772228616, 0.76602767690441675}
            }}
        },
        // String 68, MIDI 52, f1 = 164.689000000000, B = 0.000160972501471, maxPartial = 20, loopDelaySamples = 193.204175666428, order = 16, sectionCount = 8
        DispersionPreset{
            52,
            164.68899999999999,
            0.00016097250147062826,
            44100,
            193.20417566642794,
            16,
            8,
            {{
                DispersionAllpass{-1.6598462815211048, 0.68921754242223787},
                DispersionAllpass{-1.6695686096919578, 0.70211636275858602},
                DispersionAllpass{-1.6649249583406127, 0.70996391351099886},
                DispersionAllpass{-1.6453730910316171, 0.71319932991654167},
                DispersionAllpass{-1.611647039244102, 0.71325823372866382},
                DispersionAllpass{-1.565885767826624, 0.71324102137977496},
                DispersionAllpass{-1.5130718842053683, 0.7211529557286368},
                DispersionAllpass{-1.4633209809489867, 0.76350640817579241}
            }}
        },
        // String 69, MIDI 53, f1 = 174.322000000000, B = 0.000176942405464, maxPartial = 20, loopDelaySamples = 187.145211551865, order = 16, sectionCount = 8
        DispersionPreset{
            53,
            174.322,
            0.00017694240546449472,
            44100,
            187.14521155186532,
            16,
            8,
            {{
                DispersionAllpass{-1.4880411266958335, 0.55430814327679312},
                DispersionAllpass{-1.459260527008126, 0.54416268523531852},
                DispersionAllpass{-1.4689836370452201, 0.54363566889921955},
                DispersionAllpass{-1.4092700659570541, 0.60796637626833838},
                DispersionAllpass{-1.4511408527276501, 0.58847813162995455},
                DispersionAllpass{-1.433765800364011, 0.54532151640925575},
                DispersionAllpass{-1.4366169995788545, 0.53949831409764071},
                DispersionAllpass{-1.3138792797633008, 0.62583076787024849}
            }}
        },
        // String 70, MIDI 53, f1 = 174.408000000000, B = 0.000177090563099, maxPartial = 20, loopDelaySamples = 180.683763482437, order = 16, sectionCount = 8
        DispersionPreset{
            53,
            174.40799999999999,
            0.00017709056309902615,
            44100,
            180.68376348243714,
            16,
            8,
            {{
                DispersionAllpass{-1.6850767412928693, 0.71064485707153213},
                DispersionAllpass{-1.6796645679361077, 0.7124991498110238},
                DispersionAllpass{-1.6648951458646135, 0.71374238679343172},
                DispersionAllpass{-1.6377693231396548, 0.71279246211458558},
                DispersionAllpass{-1.5977570227682536, 0.71016873961734805},
                DispersionAllpass{-1.5461601636057998, 0.70870745338537411},
                DispersionAllpass{-1.4857849873001818, 0.71584317552346743},
                DispersionAllpass{-1.4201221847517922, 0.75532790767371616}
            }}
        },
        // String 71, MIDI 53, f1 = 174.493000000000, B = 0.000177237096677, maxPartial = 20, loopDelaySamples = 180.298173808339, order = 16, sectionCount = 8
        DispersionPreset{
            53,
            174.49299999999999,
            0.0001772370966770329,
            44100,
            180.29817380833902,
            16,
            8,
            {{
                DispersionAllpass{-1.6315914341694868, 0.66581971659121997},
                DispersionAllpass{-1.6486062658143481, 0.68445174790324503},
                DispersionAllpass{-1.6434954704605809, 0.69182915647581855},
                DispersionAllpass{-1.625567937429838, 0.69619007981496839},
                DispersionAllpass{-1.5522224883773439, 0.7025760387741341},
                DispersionAllpass{-1.5951120513594854, 0.6991272751444646},
                DispersionAllpass{-1.500503275099563, 0.71403035282116378},
                DispersionAllpass{-1.447740415183272, 0.75903037747402924}
            }}
        },
        // String 72, MIDI 54, f1 = 184.699000000000, B = 0.000195190980298, maxPartial = 20, loopDelaySamples = 168.147599396064, order = 16, sectionCount = 8
        DispersionPreset{
            54,
            184.69900000000001,
            0.0001951909802984074,
            44100,
            168.14759939606415,
            16,
            8,
            {{
                DispersionAllpass{-1.6462002395273194, 0.67807056540682031},
                DispersionAllpass{-1.6501652659484751, 0.68688056313390655},
                DispersionAllpass{-1.618004969372544, 0.69373766617600563},
                DispersionAllpass{-1.6406622276284131, 0.69180011028555954},
                DispersionAllpass{-1.5837637004013247, 0.69516741940784643},
                DispersionAllpass{-1.5380788916124704, 0.69850644981642018},
                DispersionAllpass{-1.4826948712831109, 0.71046936062632082},
                DispersionAllpass{-1.4232484267406817, 0.75582576112449806}
            }}
        },
        // String 73, MIDI 54, f1 = 184.790000000000, B = 0.000195354145194, maxPartial = 20, loopDelaySamples = 169.860649004000, order = 16, sectionCount = 8
        DispersionPreset{
            54,
            184.78999999999999,
            0.00019535414519352071,
            44100,
            169.8606490039997,
            16,
            8,
            {{
                DispersionAllpass{-1.5996404219444849, 0.64003371854852664},
                DispersionAllpass{-1.517378174941916, 0.67866077093930721},
                DispersionAllpass{-1.4554148653204146, 0.68802538985532546},
                DispersionAllpass{-1.6129921141251489, 0.65585137186283116},
                DispersionAllpass{-1.5624252224022661, 0.67316826073654346},
                DispersionAllpass{-1.5866905179562314, 0.66393906631868338},
                DispersionAllpass{-1.6000988495479349, 0.65689633192264618},
                DispersionAllpass{-1.3802117036171229, 0.72672526445916863}
            }}
        },
        // String 74, MIDI 54, f1 = 184.881000000000, B = 0.00019551736416, maxPartial = 20, loopDelaySamples = 170.427377829654, order = 16, sectionCount = 8
        DispersionPreset{
            54,
            184.881,
            0.00019551736416016424,
            44100,
            170.42737782965423,
            16,
            8,
            {{
                DispersionAllpass{-1.5837478618444487, 0.62728135043846756},
                DispersionAllpass{-1.5081558913887421, 0.67001564753436738},
                DispersionAllpass{-1.6011776180693646, 0.64623403832312321},
                DispersionAllpass{-1.4439379553566793, 0.67842853015155602},
                DispersionAllpass{-1.5731795639626587, 0.65171493604150865},
                DispersionAllpass{-1.5528772220629481, 0.66387468820205187},
                DispersionAllpass{-1.5842611638879724, 0.64395846438394966},
                DispersionAllpass{-1.3616717441761952, 0.71366978872274489}
            }}
        },
        // String 75, MIDI 55, f1 = 195.694000000000, B = 0.000215293756402, maxPartial = 20, loopDelaySamples = 156.361226339137, order = 16, sectionCount = 8
        DispersionPreset{
            55,
            195.69399999999999,
            0.00021529375640180309,
            44100,
            156.36122633913686,
            16,
            8,
            {{
                DispersionAllpass{-1.6718225389559913, 0.69957924511856373},
                DispersionAllpass{-1.6658341603445133, 0.70143830177066124},
                DispersionAllpass{-1.6504112068734018, 0.7031520205015066},
                DispersionAllpass{-1.6224594791795313, 0.70322529725427407},
                DispersionAllpass{-1.5809934133322967, 0.70201672890229561},
                DispersionAllpass{-1.5269258589535561, 0.70229743814161749},
                DispersionAllpass{-1.4627935451286642, 0.71161006926003367},
                DispersionAllpass{-1.3935885540884496, 0.75524833245481571}
            }}
        },
        // String 76, MIDI 55, f1 = 195.790000000000, B = 0.000215472703217, maxPartial = 20, loopDelaySamples = 156.060042255043, order = 16, sectionCount = 8
        DispersionPreset{
            55,
            195.78999999999999,
            0.00021547270321706786,
            44100,
            156.06004225504307,
            16,
            8,
            {{
                DispersionAllpass{-1.680451407677737, 0.7068503247074156},
                DispersionAllpass{-1.6731566026661959, 0.70786163448852268},
                DispersionAllpass{-1.6559154268009055, 0.70839218308573859},
                DispersionAllpass{-1.6262708154488186, 0.7073370720071227},
                DispersionAllpass{-1.5835778534689657, 0.70525264009013178},
                DispersionAllpass{-1.5289200347894629, 0.70501601773665534},
                DispersionAllpass{-1.464906220245052, 0.71421237712768448},
                DispersionAllpass{-1.3967844716703859, 0.7581077487449801}
            }}
        },
        // String 77, MIDI 55, f1 = 195.886000000000, B = 0.000215651708872, maxPartial = 20, loopDelaySamples = 155.908443113414, order = 16, sectionCount = 8
        DispersionPreset{
            55,
            195.886,
            0.00021565170887217493,
            44100,
            155.90844311341397,
            16,
            8,
            {{
                DispersionAllpass{-1.6825443938249449, 0.70862006777689834},
                DispersionAllpass{-1.6749133113822827, 0.709410399325337},
                DispersionAllpass{-1.6572220845791852, 0.70964409024530573},
                DispersionAllpass{-1.6271948155292215, 0.70833407547943217},
                DispersionAllpass{-1.5842477974451425, 0.7060748168752351},
                DispersionAllpass{-1.5294761360046747, 0.70575067927518353},
                DispersionAllpass{-1.4654888614010599, 0.71494344159907408},
                DispersionAllpass{-1.3976055181707783, 0.75890399700265831}
            }}
        },
        // String 78, MIDI 56, f1 = 207.344000000000, B = 0.000237436182662, maxPartial = 20, loopDelaySamples = 146.327321742439, order = 16, sectionCount = 8
        DispersionPreset{
            56,
            207.34399999999999,
            0.00023743618266172512,
            44100,
            146.3273217424391,
            16,
            8,
            {{
                DispersionAllpass{-1.6633592043905396, 0.69260449336318086},
                DispersionAllpass{-1.6555495286256547, 0.69359098323048507},
                DispersionAllpass{-1.6374825220236362, 0.69432442286296236},
                DispersionAllpass{-1.6063270288080884, 0.69363933632099051},
                DispersionAllpass{-1.5604221179421192, 0.69160716392298427},
                DispersionAllpass{-1.4996805602258871, 0.690522748259912},
                DispersionAllpass{-1.3388979128685243, 0.73765230597954556},
                DispersionAllpass{-1.4253340316104646, 0.6974806032266605}
            }}
        },
        // String 79, MIDI 56, f1 = 207.445000000000, B = 0.000237631879263, maxPartial = 20, loopDelaySamples = 145.856997561918, order = 16, sectionCount = 8
        DispersionPreset{
            56,
            207.44499999999999,
            0.00023763187926342927,
            44100,
            145.85699756191804,
            16,
            8,
            {{
                DispersionAllpass{-1.6809861020566832, 0.70742559572816266},
                DispersionAllpass{-1.6706568437411808, 0.70683166391698582},
                DispersionAllpass{-1.6486858218783329, 0.70505317339919127},
                DispersionAllpass{-1.613827715147617, 0.70188523488650556},
                DispersionAllpass{-1.5654151706253607, 0.69802848962408071},
                DispersionAllpass{-1.5036623018220892, 0.69599697405219574},
                DispersionAllpass{-1.3460662639605652, 0.74392483778649388},
                DispersionAllpass{-1.4298150635299924, 0.70289593379625848}
            }}
        },
        // String 80, MIDI 56, f1 = 207.547000000000, B = 0.000237829578076, maxPartial = 20, loopDelaySamples = 146.500762920978, order = 16, sectionCount = 8
        DispersionPreset{
            56,
            207.547,
            0.00023782957807628698,
            44100,
            146.50076292097825,
            16,
            8,
            {{
                DispersionAllpass{-1.6237740258329887, 0.65962753375042782},
                DispersionAllpass{-1.5596125286456479, 0.68805129433243717},
                DispersionAllpass{-1.6379271340308843, 0.67706677812295613},
                DispersionAllpass{-1.6303680059107035, 0.68560796914990352},
                DispersionAllpass{-1.3332603351239953, 0.73246389534583956},
                DispersionAllpass{-1.6039508965136517, 0.68860650701726289},
                DispersionAllpass{-1.4986714165526642, 0.68722515999115663},
                DispersionAllpass{-1.4229097496673357, 0.69364912500693876}
            }}
        },
        // String 81, MIDI 57, f1 = 219.687000000000, B = 0.000261819889584, maxPartial = 20, loopDelaySamples = 138.489775525237, order = 16, sectionCount = 8
        DispersionPreset{
            57,
            219.68700000000001,
            0.00026181988958400476,
            44100,
            138.48977552523732,
            16,
            8,
            {{
                DispersionAllpass{-1.5590763527837435, 0.60833135895264756},
                DispersionAllpass{-1.5685089041705977, 0.6199728255038377},
                DispersionAllpass{-1.5786455631257996, 0.64106178210317344},
                DispersionAllpass{-1.5166822170981695, 0.65255287648433391},
                DispersionAllpass{-1.5594834326812028, 0.65003548480053397},
                DispersionAllpass{-1.450909473619046, 0.65194998215377098},
                DispersionAllpass{-1.3611964606438658, 0.65439107716458711},
                DispersionAllpass{-1.2376991780979838, 0.68104977637253039}
            }}
        },
        // String 82, MIDI 57, f1 = 219.794000000000, B = 0.0002620353641, maxPartial = 20, loopDelaySamples = 138.576534102016, order = 16, sectionCount = 8
        DispersionPreset{
            57,
            219.79400000000001,
            0.00026203536410030967,
            44100,
            138.57653410201564,
            16,
            8,
            {{
                DispersionAllpass{-1.5556885274449233, 0.60582955852411036},
                DispersionAllpass{-1.5603225407808159, 0.61342297991334849},
                DispersionAllpass{-1.5727974390196591, 0.63631515884703094},
                DispersionAllpass{-1.5105559310370185, 0.64644892655650321},
                DispersionAllpass{-1.552114040697391, 0.6437704480622195},
                DispersionAllpass{-1.4471819988662167, 0.64721446152846673},
                DispersionAllpass{-1.358485603024979, 0.650505569527839},
                DispersionAllpass{-1.2334497080101592, 0.67646889316782532}
            }}
        },
        // String 83, MIDI 57, f1 = 219.902000000000, B = 0.000262252923233, maxPartial = 20, loopDelaySamples = 136.674488819611, order = 16, sectionCount = 8
        DispersionPreset{
            57,
            219.90199999999999,
            0.00026225292323324228,
            44100,
            136.67448881961079,
            16,
            8,
            {{
                DispersionAllpass{-1.6232471132882313, 0.65925630520348044},
                DispersionAllpass{-1.6367665215708855, 0.67680975310777614},
                DispersionAllpass{-1.626559808970611, 0.68467391764953844},
                DispersionAllpass{-1.5958621747141473, 0.68651934648396296},
                DispersionAllpass{-1.5456821723910352, 0.68450442180270943},
                DispersionAllpass{-1.4768580288838296, 0.68175238398354276},
                DispersionAllpass{-1.390232055031704, 0.68550609804649965},
                DispersionAllpass{-1.2838633894130078, 0.72071550600400591}
            }}
        },
        // String 84, MIDI 58, f1 = 232.768000000000, B = 0.000288675971504, maxPartial = 20, loopDelaySamples = 128.102175216788, order = 16, sectionCount = 8
        DispersionPreset{
            58,
            232.768,
            0.00028867597150438904,
            44100,
            128.10217521678803,
            16,
            8,
            {{
                DispersionAllpass{-1.6060992549627628, 0.64545799607295995},
                DispersionAllpass{-1.618969011597301, 0.662628423604121},
                DispersionAllpass{-1.5764353521755197, 0.67389597650463906},
                DispersionAllpass{-1.5213893048484084, 0.6712657468652441},
                DispersionAllpass{-1.6089611333219538, 0.67152985536955745},
                DispersionAllpass{-1.44415363232986, 0.66655868617668179},
                DispersionAllpass{-1.344350888344787, 0.66663666226527407},
                DispersionAllpass{-1.2141602172802239, 0.6950769264162403}
            }}
        },
        // String 85, MIDI 58, f1 = 232.882000000000, B = 0.000288914539892, maxPartial = 20, loopDelaySamples = 128.347141357696, order = 16, sectionCount = 8
        DispersionPreset{
            58,
            232.88200000000001,
            0.00028891453989226564,
            44100,
            128.34714135769553,
            16,
            8,
            {{
                DispersionAllpass{-1.5924542786172862, 0.63426009335520284},
                DispersionAllpass{-1.6171646986875037, 0.66086907633540204},
                DispersionAllpass{-1.6047170872389256, 0.66815589997011249},
                DispersionAllpass{-1.5697218124967927, 0.66866261271558436},
                DispersionAllpass{-1.5133098429354861, 0.66482812675598024},
                DispersionAllpass{-1.3344864439309536, 0.65866005892810398},
                DispersionAllpass{-1.4353455072269867, 0.65937704899744909},
                DispersionAllpass{-1.2002358554167045, 0.68499157643045916}
            }}
        },
        // String 86, MIDI 58, f1 = 232.997000000000, B = 0.000289155279504, maxPartial = 20, loopDelaySamples = 127.754280316080, order = 16, sectionCount = 8
        DispersionPreset{
            58,
            232.99700000000001,
            0.00028915527950406771,
            44100,
            127.75428031607981,
            16,
            8,
            {{
                DispersionAllpass{-1.6290113686427201, 0.66424049113875649},
                DispersionAllpass{-1.6306375642538566, 0.67324164056036706},
                DispersionAllpass{-1.6152809218938406, 0.67853462074707216},
                DispersionAllpass{-1.5792159186294252, 0.67850837115811824},
                DispersionAllpass{-1.5222208371949655, 0.67447205519437325},
                DispersionAllpass{-1.444347172843687, 0.66919172214770617},
                DispersionAllpass{-1.3449817872805956, 0.66941596509326318},
                DispersionAllpass{-1.2167908018716567, 0.69898175609637947}
            }}
        },
        // String 87, MIDI 59, f1 = 246.628000000000, B = 0.000318244582449, maxPartial = 20, loopDelaySamples = 120.203542616815, order = 16, sectionCount = 8
        DispersionPreset{
            59,
            246.62799999999999,
            0.00031824458244894441,
            44100,
            120.20354261681456,
            16,
            8,
            {{
                DispersionAllpass{-1.5582253889565809, 0.60757292857719636},
                DispersionAllpass{-1.5752060642848409, 0.62656904254669921},
                DispersionAllpass{-1.5766143049199184, 0.64391729337910097},
                DispersionAllpass{-1.5482617425279523, 0.65083344638882357},
                DispersionAllpass{-1.491155598578118, 0.64964577181542549},
                DispersionAllpass{-1.4058687462233443, 0.64361016825917572},
                DispersionAllpass{-1.2906079290882184, 0.63894435101440261},
                DispersionAllpass{-1.1285630668861866, 0.65524210758316936}
            }}
        },
        // String 88, MIDI 59, f1 = 246.749000000000, B = 0.000318507688255, maxPartial = 20, loopDelaySamples = 119.641440648116, order = 16, sectionCount = 8
        DispersionPreset{
            59,
            246.749,
            0.00031850768825534171,
            44100,
            119.64144064811619,
            16,
            8,
            {{
                DispersionAllpass{-1.6015948149744095, 0.64195692820678696},
                DispersionAllpass{-1.6113416266476879, 0.65739309380935429},
                DispersionAllpass{-1.5591338347465162, 0.66523593964906202},
                DispersionAllpass{-1.4966999481699668, 0.66041775102344213},
                DispersionAllpass{-1.5972994538254059, 0.66475114767506827},
                DispersionAllpass{-1.4095157615802445, 0.65286329802690923},
                DispersionAllpass{-1.2955194198011959, 0.64873984167677601},
                DispersionAllpass{-1.1402348693140085, 0.66941872649606049}
            }}
        },
        // String 89, MIDI 59, f1 = 246.871000000000, B = 0.000318773054865, maxPartial = 20, loopDelaySamples = 121.898871752334, order = 16, sectionCount = 8
        DispersionPreset{
            59,
            246.87100000000001,
            0.00031877305486527742,
            44100,
            121.8988717523342,
            16,
            8,
            {{
                DispersionAllpass{-1.4546582775928514, 0.53323840450431492},
                DispersionAllpass{-1.4773100792739591, 0.56004077853355261},
                DispersionAllpass{-1.471027161259407, 0.54197919409876105},
                DispersionAllpass{-1.4775967115580466, 0.58096216617437191},
                DispersionAllpass{-1.4463700816460312, 0.59441682875774826},
                DispersionAllpass{-1.3738398215542749, 0.59573001114868163},
                DispersionAllpass{-1.0724658661688002, 0.58326974645717466},
                DispersionAllpass{-1.2576642543535257, 0.58933772551499597}
            }}
        },
        // String 90, MIDI 60, f1 = 261.314000000000, B = 0.000350796675381, maxPartial = 20, loopDelaySamples = 111.506486297326, order = 16, sectionCount = 8
        DispersionPreset{
            60,
            261.31400000000002,
            0.00035079667538140852,
            44100,
            111.50648629732592,
            16,
            8,
            {{
                DispersionAllpass{-1.6337035684906067, 0.66847262299461718},
                DispersionAllpass{-1.6228929846228088, 0.66982057131443062},
                DispersionAllpass{-1.5955946071613647, 0.66936446154929008},
                DispersionAllpass{-1.546846732130253, 0.66478892451963423},
                DispersionAllpass{-1.3758248555898394, 0.6448246544298788},
                DispersionAllpass{-1.4741930655885864, 0.65600357632097295},
                DispersionAllpass{-1.0706757481966922, 0.65128268176437731},
                DispersionAllpass{-1.2479526794152564, 0.63669650304835679}
            }}
        },
        // String 91, MIDI 60, f1 = 261.442000000000, B = 0.000351085832472, maxPartial = 20, loopDelaySamples = 111.434239897690, order = 16, sectionCount = 8
        DispersionPreset{
            60,
            261.44200000000001,
            0.0003510858324722655,
            44100,
            111.43423989768988,
            16,
            8,
            {{
                DispersionAllpass{-1.6347735092549034, 0.66935905046982236},
                DispersionAllpass{-1.6235269897489195, 0.67041748648629385},
                DispersionAllpass{-1.5958003262324478, 0.66967749216284478},
                DispersionAllpass{-1.546787830483032, 0.66493248051001219},
                DispersionAllpass{-1.37549649376809, 0.64482863216109454},
                DispersionAllpass{-1.4739756889566973, 0.65605662712103119},
                DispersionAllpass{-1.0701368144996588, 0.65125960679498229},
                DispersionAllpass{-1.2475274150168909, 0.63667516678345903}
            }}
        },
        // String 92, MIDI 60, f1 = 261.571000000000, B = 0.00035137734298, maxPartial = 20, loopDelaySamples = 111.405070081207, order = 16, sectionCount = 8
        DispersionPreset{
            60,
            261.57100000000003,
            0.00035137734297986819,
            44100,
            111.40507008120738,
            16,
            8,
            {{
                DispersionAllpass{-1.6294602886863494, 0.66497067908025198},
                DispersionAllpass{-1.6202392900122067, 0.66741840538504826},
                DispersionAllpass{-1.5942827004018776, 0.66792247803892701},
                DispersionAllpass{-1.5461681199481849, 0.66384681486425301},
                DispersionAllpass{-1.3751746344569062, 0.64409954786103663},
                DispersionAllpass{-1.4736797179449466, 0.65526238715340757},
                DispersionAllpass{-1.068686657435389, 0.64993088610145844},
                DispersionAllpass{-1.2468946012386013, 0.63582234855040143}
            }}
        },
        // String 93, MIDI 61, f1 = 276.874000000000, B = 0.000386625622281, maxPartial = 20, loopDelaySamples = 104.516019058010, order = 16, sectionCount = 8
        DispersionPreset{
            61,
            276.87400000000002,
            0.00038662562228112331,
            44100,
            104.51601905800976,
            16,
            8,
            {{
                DispersionAllpass{-1.5676005500999943, 0.61509215943110374},
                DispersionAllpass{-1.5774956957290267, 0.63116152210338561},
                DispersionAllpass{-1.5177263023025307, 0.640156612298318},
                DispersionAllpass{-1.442407816686021, 0.63283030901911752},
                DispersionAllpass{-1.5621876896276514, 0.63982955821553011},
                DispersionAllpass{-1.3345633673139712, 0.61981773210074009},
                DispersionAllpass{-1.1890342343142553, 0.60560927861424574},
                DispersionAllpass{-0.97647686546572376, 0.60320519189804767}
            }}
        },
        // String 94, MIDI 61, f1 = 277.010000000000, B = 0.000386944763298, maxPartial = 20, loopDelaySamples = 104.536528948268, order = 16, sectionCount = 8
        DispersionPreset{
            61,
            277.00999999999999,
            0.00038694476329759826,
            44100,
            104.53652894826806,
            16,
            8,
            {{
                DispersionAllpass{-1.5709142762973718, 0.61780101545096489},
                DispersionAllpass{-1.5754808787732482, 0.63002245616759234},
                DispersionAllpass{-1.5574420817473047, 0.63659356358499541},
                DispersionAllpass{-1.5122943345549185, 0.63609632525528992},
                DispersionAllpass{-1.3303214175736631, 0.61600512886352188},
                DispersionAllpass{-1.4375060656001577, 0.62879948623918458},
                DispersionAllpass{-1.1849344989642048, 0.60172287870657881},
                DispersionAllpass{-0.97110776503315965, 0.59796478070112513}
            }}
        },
        // String 95, MIDI 61, f1 = 277.146000000000, B = 0.000387264007188, maxPartial = 20, loopDelaySamples = 104.482192524003, order = 16, sectionCount = 8
        DispersionPreset{
            61,
            277.14600000000002,
            0.00038726400718821016,
            44100,
            104.48219252400266,
            16,
            8,
            {{
                DispersionAllpass{-1.5689316895345327, 0.61628826624741995},
                DispersionAllpass{-1.5712762056166276, 0.62659733964979625},
                DispersionAllpass{-1.5552997837057834, 0.63436230272174532},
                DispersionAllpass{-1.5121287382459405, 0.6352293724182112},
                DispersionAllpass{-1.3311555907430093, 0.61612231001028406},
                DispersionAllpass{-1.438207364377601, 0.62865747034110531},
                DispersionAllpass{-1.1855305910331553, 0.60183492971023},
                DispersionAllpass{-0.9713131162956139, 0.59790035396509633}
            }}
        },
        // String 96, MIDI 62, f1 = 293.361000000000, B = 0.000426058309186, maxPartial = 20, loopDelaySamples = 97.154595533226, order = 16, sectionCount = 8
        DispersionPreset{
            62,
            293.36099999999999,
            0.00042605830918566789,
            44100,
            97.154595533225546,
            16,
            8,
            {{
                DispersionAllpass{-1.6153861020595486, 0.65382921280811668},
                DispersionAllpass{-1.5047303042299636, 0.64290352400362905},
                DispersionAllpass{-1.1336344283442303, 0.59318303837290232},
                DispersionAllpass{-1.6001233042365395, 0.65352131249583434},
                DispersionAllpass{-1.2945963598242545, 0.61211133052090938},
                DispersionAllpass{-1.5649388819554455, 0.65059818005021386},
                DispersionAllpass{-1.4158403532786978, 0.62980149132772467},
                DispersionAllpass{-0.89872301937110444, 0.58351078727282191}
            }}
        },
        // String 97, MIDI 62, f1 = 293.505000000000, B = 0.000426409277174, maxPartial = 20, loopDelaySamples = 97.118782965074, order = 16, sectionCount = 8
        DispersionPreset{
            62,
            293.505,
            0.00042640927717448165,
            44100,
            97.118782965074161,
            16,
            8,
            {{
                DispersionAllpass{-1.6106470581023387, 0.64995863803326559},
                DispersionAllpass{-1.5635530258104549, 0.64894965721514442},
                DispersionAllpass{-1.5042909165235216, 0.64193522811500825},
                DispersionAllpass{-1.4157594162852822, 0.62914253135522202},
                DispersionAllpass{-1.597053332722596, 0.65073981692694927},
                DispersionAllpass{-1.2945103611114173, 0.61152572444726871},
                DispersionAllpass{-1.1332840462134732, 0.59247770153803125},
                DispersionAllpass{-0.8978002009735716, 0.58231131590001894}
            }}
        },
        // String 98, MIDI 62, f1 = 293.649000000000, B = 0.000426760357817, maxPartial = 20, loopDelaySamples = 97.051327625675, order = 16, sectionCount = 8
        DispersionPreset{
            62,
            293.649,
            0.0004267603578166185,
            44100,
            97.051327625675199,
            16,
            8,
            {{
                DispersionAllpass{-1.6124456339855293, 0.65143010988201511},
                DispersionAllpass{-1.5980719581906386, 0.65169892548538155},
                DispersionAllpass{-1.5638544394985381, 0.64942956620046866},
                DispersionAllpass{-1.5042226877087435, 0.64216988012609322},
                DispersionAllpass{-1.4155175765741463, 0.6292776392021292},
                DispersionAllpass{-1.2941609523698989, 0.61162361470476623},
                DispersionAllpass{-1.1328446523359674, 0.59257300413068925},
                DispersionAllpass{-0.89727653771667915, 0.58247523058045592}
            }}
        },
        // String 99, MIDI 63, f1 = 310.829000000000, B = 0.000469448275366, maxPartial = 20, loopDelaySamples = 91.054310187106, order = 16, sectionCount = 8
        DispersionPreset{
            63,
            310.82900000000001,
            0.00046944827536626285,
            44100,
            91.054310187105585,
            16,
            8,
            {{
                DispersionAllpass{-1.522404024999741, 0.58047988400722261},
                DispersionAllpass{-1.5199551219694072, 0.58717222430080385},
                DispersionAllpass{-1.250554629944395, 0.57619568677435118},
                DispersionAllpass{-1.5077651833369354, 0.59882996025648461},
                DispersionAllpass{-1.0739083330373593, 0.55045381833174722},
                DispersionAllpass{-1.4622517223809575, 0.60178257491251386},
                DispersionAllpass{-1.3775517804433277, 0.59393705606897074},
                DispersionAllpass{-0.81432984948629727, 0.51891986564766901}
            }}
        },
        // String 100, MIDI 63, f1 = 310.982000000000, B = 0.00046983553171, maxPartial = 20, loopDelaySamples = 90.791705225400, order = 16, sectionCount = 8
        DispersionPreset{
            63,
            310.98200000000003,
            0.00046983553170952373,
            44100,
            90.791705225399767,
            16,
            8,
            {{
                DispersionAllpass{-1.5523723323316492, 0.60347969657300049},
                DispersionAllpass{-1.5292454377835663, 0.61990516478365265},
                DispersionAllpass{-1.2493395811074544, 0.58389535052086228},
                DispersionAllpass{-1.5545057142190886, 0.61528505914874099},
                DispersionAllpass{-1.0716048028519318, 0.55813636788813381},
                DispersionAllpass{-1.4719435092605362, 0.61594304474846495},
                DispersionAllpass{-1.3799720822110186, 0.60375080306473972},
                DispersionAllpass{-0.81070639378355502, 0.52944344797863785}
            }}
        },
        // String 101, MIDI 63, f1 = 311.134000000000, B = 0.000470220379953, maxPartial = 20, loopDelaySamples = 90.770986484393, order = 16, sectionCount = 8
        DispersionPreset{
            63,
            311.13400000000001,
            0.00047022037995252336,
            44100,
            90.770986484392751,
            16,
            8,
            {{
                DispersionAllpass{-1.5419465200943689, 0.59530780495036573},
                DispersionAllpass{-1.5260499147708051, 0.6162271003703077},
                DispersionAllpass{-1.2502643894112055, 0.58302733175488386},
                DispersionAllpass{-1.548052955175155, 0.60964251366026845},
                DispersionAllpass{-1.0723509553749935, 0.55719461365553691},
                DispersionAllpass{-1.4711858720466438, 0.61380936628999017},
                DispersionAllpass{-1.3805011802910745, 0.60251918944694771},
                DispersionAllpass{-0.81118559010879632, 0.5278920667114082}
            }}
        },
        // String 102, MIDI 64, f1 = 329.337000000000, B = 0.000517187775597, maxPartial = 20, loopDelaySamples = 84.331710552738, order = 16, sectionCount = 8
        DispersionPreset{
            64,
            329.33699999999999,
            0.00051718777559689097,
            44100,
            84.331710552737945,
            16,
            8,
            {{
                DispersionAllpass{-1.5893340148775108, 0.63314225618404618},
                DispersionAllpass{-1.5714018455120653, 0.63246539268220803},
                DispersionAllpass{-1.4586364774450353, 0.61826357813372512},
                DispersionAllpass{-1.5299026923399239, 0.62838307995541998},
                DispersionAllpass{-1.3529176143565653, 0.60095810720539111},
                DispersionAllpass{-1.2077173115065125, 0.57631859164166865},
                DispersionAllpass{-1.0136176628601345, 0.54533023782485623},
                DispersionAllpass{-0.73416627028721049, 0.50941100125915817}
            }}
        },
        // String 103, MIDI 64, f1 = 329.499000000000, B = 0.000517613535847, maxPartial = 20, loopDelaySamples = 84.521217696624, order = 16, sectionCount = 8
        DispersionPreset{
            64,
            329.49900000000002,
            0.00051761353584727128,
            44100,
            84.521217696624092,
            16,
            8,
            {{
                DispersionAllpass{-1.5533392210249519, 0.60474550496639778},
                DispersionAllpass{-1.5338316716519667, 0.6016464011657443},
                DispersionAllpass{-1.5000513129545372, 0.60031754842168905},
                DispersionAllpass{-1.4419249864851422, 0.59703179061253742},
                DispersionAllpass{-1.3462959475840026, 0.58534274106069895},
                DispersionAllpass{-1.2068195678748839, 0.56373504993490686},
                DispersionAllpass{-1.0154844933804092, 0.53311878993860518},
                DispersionAllpass{-0.74094655131965981, 0.49470572042606492}
            }}
        },
        // String 104, MIDI 64, f1 = 329.661000000000, B = 0.000518039432066, maxPartial = 20, loopDelaySamples = 84.372251159234, order = 16, sectionCount = 8
        DispersionPreset{
            64,
            329.661,
            0.00051803943206598496,
            44100,
            84.372251159234011,
            16,
            8,
            {{
                DispersionAllpass{-1.5644769398886087, 0.61334370737531163},
                DispersionAllpass{-1.5519746513884958, 0.61584415780810009},
                DispersionAllpass{-1.5164201056196063, 0.61508047879344274},
                DispersionAllpass{-1.450010985656599, 0.60751253678490291},
                DispersionAllpass{-1.3479686794279271, 0.59202414533637016},
                DispersionAllpass{-1.2051792265031847, 0.56836997949033441},
                DispersionAllpass{-1.0124204094768721, 0.53721533744407135},
                DispersionAllpass{-0.73579787351396797, 0.49939241256323219}
            }}
        },
        // String 105, MIDI 65, f1 = 348.948000000000, B = 0.000569708650485, maxPartial = 20, loopDelaySamples = 78.441907392112, order = 16, sectionCount = 8
        DispersionPreset{
            65,
            348.94799999999998,
            0.00056970865048497213,
            44100,
            78.441907392111503,
            16,
            8,
            {{
                DispersionAllpass{-1.5691354957328902, 0.617225352814324},
                DispersionAllpass{-1.5513650214482433, 0.61722295839247499},
                DispersionAllpass{-1.5085237601022459, 0.61360050933326249},
                DispersionAllpass{-1.4332278244418235, 0.60312304404642814},
                DispersionAllpass{-1.3202679352019215, 0.58433725995741037},
                DispersionAllpass{-1.1643865427385274, 0.55669785365339741},
                DispersionAllpass{-0.95677930662081156, 0.52044087169558884},
                DispersionAllpass{-0.66712830245803134, 0.47636109929088749}
            }}
        },
        // String 106, MIDI 65, f1 = 349.119000000000, B = 0.000570175235724, maxPartial = 20, loopDelaySamples = 78.352038629528, order = 16, sectionCount = 8
        DispersionPreset{
            65,
            349.11900000000003,
            0.00057017523572402582,
            44100,
            78.352038629528479,
            16,
            8,
            {{
                DispersionAllpass{-1.5819425325729473, 0.62744785827690064},
                DispersionAllpass{-1.4347170363121158, 0.60669097953895912},
                DispersionAllpass{-1.5132287557805415, 0.61906020517931004},
                DispersionAllpass{-1.5604478079506821, 0.62526207708154857},
                DispersionAllpass{-1.3197592396708784, 0.58675174390313167},
                DispersionAllpass{-1.162725779109733, 0.55852037871242821},
                DispersionAllpass{-0.95435739322544355, 0.52209634229387558},
                DispersionAllpass{-0.66334065235345963, 0.47803738535126172}
            }}
        },
        // String 107, MIDI 65, f1 = 349.291000000000, B = 0.000570644698742, maxPartial = 20, loopDelaySamples = 78.497756830785, order = 16, sectionCount = 8
        DispersionPreset{
            65,
            349.291,
            0.00057064469874168445,
            44100,
            78.497756830785136,
            16,
            8,
            {{
                DispersionAllpass{-1.5118754315915599, 0.57244606698774281},
                DispersionAllpass{-1.4914468323126833, 0.59363999754387975},
                DispersionAllpass{-1.5170820858767944, 0.58694218527310249},
                DispersionAllpass{-1.4274616369026158, 0.58976372679731182},
                DispersionAllpass{-1.3213567784509981, 0.57486483541051092},
                DispersionAllpass{-1.1694469887085277, 0.54919052719605721},
                DispersionAllpass{-0.68019055903662018, 0.46991453856353826},
                DispersionAllpass{-0.96468972140945075, 0.51356896652095185}
            }}
        },
        // String 108, MIDI 66, f1 = 369.726000000000, B = 0.000627477151542, maxPartial = 20, loopDelaySamples = 72.935685349136, order = 16, sectionCount = 8
        DispersionPreset{
            66,
            369.726,
            0.00062747715154224781,
            44100,
            72.935685349135511,
            16,
            8,
            {{
                DispersionAllpass{-1.5044984961870398, 0.5670522903675016},
                DispersionAllpass{-1.5043713484226886, 0.57838349300009773},
                DispersionAllpass{-1.4053807643548204, 0.57833554000274223},
                DispersionAllpass{-1.474561361726882, 0.58353588100347997},
                DispersionAllpass{-1.2924873117916, 0.56195781484142127},
                DispersionAllpass{-1.1321848423231056, 0.53453538121356214},
                DispersionAllpass{-0.91805839184923388, 0.49707060835454858},
                DispersionAllpass{-0.62366842125606048, 0.45221407803012648}
            }}
        },
        // String 109, MIDI 66, f1 = 369.908000000000, B = 0.000627992655821, maxPartial = 20, loopDelaySamples = 72.876346354241, order = 16, sectionCount = 8
        DispersionPreset{
            66,
            369.90800000000002,
            0.00062799265582099805,
            44100,
            72.876346354241377,
            16,
            8,
            {{
                DispersionAllpass{-1.5069613382410607, 0.56884233198958112},
                DispersionAllpass{-1.5097458836286677, 0.58258090752676983},
                DispersionAllpass{-1.4063356654092471, 0.58029801049576579},
                DispersionAllpass{-1.4778874404042739, 0.5868115614211239},
                DispersionAllpass{-1.2918851032167145, 0.5630217970115271},
                DispersionAllpass{-1.1306500501088681, 0.53505587557780387},
                DispersionAllpass{-0.915945111139704, 0.49725295916728623},
                DispersionAllpass{-0.62119914595285464, 0.45217218265032338}
            }}
        },
        // String 110, MIDI 66, f1 = 370.090000000000, B = 0.000628508323661, maxPartial = 20, loopDelaySamples = 72.873792480707, order = 16, sectionCount = 8
        DispersionPreset{
            66,
            370.08999999999997,
            0.00062850832366120868,
            44100,
            72.873792480707152,
            16,
            8,
            {{
                DispersionAllpass{-1.5150131251514205, 0.57545466224491648},
                DispersionAllpass{-1.4582936778347464, 0.57127359115770693},
                DispersionAllpass{-1.4928960391619936, 0.57135292428152007},
                DispersionAllpass{-1.3956413595227641, 0.56882366652360217},
                DispersionAllpass{-1.2890747507092319, 0.55581873052612829},
                DispersionAllpass{-1.1330379381437892, 0.53092793840263586},
                DispersionAllpass{-0.62786763005498603, 0.45141513030389663},
                DispersionAllpass{-0.92135485587114041, 0.49519169904630578}
            }}
        },
        // String 111, MIDI 67, f1 = 391.742000000000, B = 0.000691013485156, maxPartial = 20, loopDelaySamples = 67.524651087525, order = 16, sectionCount = 8
        DispersionPreset{
            67,
            391.74200000000002,
            0.00069101348515588257,
            44100,
            67.524651087525314,
            16,
            8,
            {{
                DispersionAllpass{-1.5323111521905055, 0.58867100763038394},
                DispersionAllpass{-1.5177291071797727, 0.59199668470793532},
                DispersionAllpass{-1.4730851327227519, 0.59011603912401789},
                DispersionAllpass{-1.3897438706241125, 0.57914233822751515},
                DispersionAllpass{-1.2635610617319224, 0.55833684853041665},
                DispersionAllpass{-1.09012635076232, 0.52756139979807615},
                DispersionAllpass{-0.86185646114988335, 0.48738670826920238},
                DispersionAllpass{-0.55016854336680043, 0.43963708860053946}
            }}
        },
        // String 112, MIDI 67, f1 = 391.934000000000, B = 0.000691577940119, maxPartial = 20, loopDelaySamples = 67.442858028485, order = 16, sectionCount = 8
        DispersionPreset{
            67,
            391.93400000000003,
            0.00069157794011855437,
            44100,
            67.442858028485475,
            16,
            8,
            {{
                DispersionAllpass{-1.5600621458539565, 0.61050109088885574},
                DispersionAllpass{-1.5341812487188102, 0.60701079548101955},
                DispersionAllpass{-1.4788712260518986, 0.59867754408303964},
                DispersionAllpass{-1.3889059838131261, 0.58367853079510723},
                DispersionAllpass{-1.258917936602733, 0.56053642764815081},
                DispersionAllpass{-1.0832816399010092, 0.52833216477222744},
                DispersionAllpass{-0.85392681689190475, 0.48723878176921587},
                DispersionAllpass{-0.54227240468662141, 0.43905961918212338}
            }}
        },
        // String 113, MIDI 67, f1 = 392.127000000000, B = 0.000692145514009, maxPartial = 20, loopDelaySamples = 67.433781247286, order = 16, sectionCount = 8
        DispersionPreset{
            67,
            392.12700000000001,
            0.0006921455140092512,
            44100,
            67.433781247285722,
            16,
            8,
            {{
                DispersionAllpass{-1.5347731517231655, 0.59060199496397447},
                DispersionAllpass{-1.4731773233366396, 0.59057215274219832},
                DispersionAllpass{-1.5187751645658383, 0.59303467192324355},
                DispersionAllpass{-1.389316973308957, 0.57931996871320723},
                DispersionAllpass{-1.2627685782899376, 0.55835549669248408},
                DispersionAllpass{-1.0890214696170362, 0.52746960232715057},
                DispersionAllpass{-0.54848656979006727, 0.43939668279142763},
                DispersionAllpass{-0.86046451420293002, 0.48721216976832021}
            }}
        },
        // String 114, MIDI 68, f1 = 415.068000000000, B = 0.000760878787671, maxPartial = 20, loopDelaySamples = 62.452832810577, order = 16, sectionCount = 8
        DispersionPreset{
            68,
            415.06799999999998,
            0.00076087878767108698,
            44100,
            62.452832810576894,
            16,
            8,
            {{
                DispersionAllpass{-1.5653315697617132, 0.61485840264819247},
                DispersionAllpass{-1.5354545863361599, 0.61003594696183894},
                DispersionAllpass{-1.4730616266331713, 0.59949394130640821},
                DispersionAllpass{-1.3741970033076809, 0.58215793156666806},
                DispersionAllpass{-1.2339195499598952, 0.55695220675161183},
                DispersionAllpass{-1.0460876686438869, 0.52292381516760966},
                DispersionAllpass{-0.80136779737748054, 0.47979172005591936},
                DispersionAllpass{-0.46964335604299329, 0.42822870147002728}
            }}
        },
        // String 115, MIDI 68, f1 = 415.272000000000, B = 0.000761501183212, maxPartial = 20, loopDelaySamples = 62.411485926239, order = 16, sectionCount = 8
        DispersionPreset{
            68,
            415.27199999999999,
            0.00076150118321163189,
            44100,
            62.411485926239408,
            16,
            8,
            {{
                DispersionAllpass{-1.5653244917948326, 0.61485424134899835},
                DispersionAllpass{-1.5354318869732011, 0.61003081222449196},
                DispersionAllpass{-1.4730068291330138, 0.59948725195119079},
                DispersionAllpass{-1.0457903055466926, 0.5228971269531244},
                DispersionAllpass{-0.80091939704712101, 0.47974535312721844},
                DispersionAllpass{-0.46900224352832182, 0.42814562254874722},
                DispersionAllpass{-1.3740895483038493, 0.58214819801825102},
                DispersionAllpass{-1.2337332702201997, 0.55693639304278442}
            }}
        },
        // String 116, MIDI 68, f1 = 415.476000000000, B = 0.000762123774488, maxPartial = 20, loopDelaySamples = 62.370223096014, order = 16, sectionCount = 8
        DispersionPreset{
            68,
            415.476,
            0.00076212377448834864,
            44100,
            62.370223096014406,
            16,
            8,
            {{
                DispersionAllpass{-1.565311030521364, 0.61484510631643341},
                DispersionAllpass{-1.4729408607662371, 0.59947171841737024},
                DispersionAllpass{-1.5354007086431025, 0.61001922635954298},
                DispersionAllpass{-1.2335404195643329, 0.55691070870629711},
                DispersionAllpass{-1.0454939208276044, 0.52286341488151578},
                DispersionAllpass{-1.3739708880726385, 0.5821279664215594},
                DispersionAllpass{-0.80047844161093307, 0.47969560565658675},
                DispersionAllpass{-0.46837079892579692, 0.42806182263741532}
            }}
        },
        // String 117, MIDI 69, f1 = 439.784000000000, B = 0.000837699486175, maxPartial = 20, loopDelaySamples = 57.737530112252, order = 16, sectionCount = 8
        DispersionPreset{
            69,
            439.78399999999999,
            0.00083769948617548108,
            44100,
            57.737530112252138,
            16,
            8,
            {{
                DispersionAllpass{-1.5680997769249021, 0.61722665478622174},
                DispersionAllpass{-1.5358166934841762, 0.61208054557348779},
                DispersionAllpass{-1.4686231770360845, 0.60097374695346228},
                DispersionAllpass{-1.3623564067927374, 0.58286406592920248},
                DispersionAllpass{-1.0092623441063335, 0.52096492638878888},
                DispersionAllpass{-0.74523450519305601, 0.47509254127517681},
                DispersionAllpass{-0.38986255005236348, 0.4187149988419967},
                DispersionAllpass{-1.2115599034405879, 0.55659819994647486}
            }}
        },
        // String 118, MIDI 69, f1 = 440.000000000000, B = 0.000838383303279, maxPartial = 20, loopDelaySamples = 57.698814885439, order = 16, sectionCount = 8
        DispersionPreset{
            69,
            440,
            0.00083838330327871036,
            44100,
            57.698814885439084,
            16,
            8,
            {{
                DispersionAllpass{-1.5681394149008787, 0.61725959971287281},
                DispersionAllpass{-1.5358343872944598, 0.61211063951996147},
                DispersionAllpass{-1.4685969547326536, 0.6009988264999967},
                DispersionAllpass{-1.3622621382433462, 0.58288213287782376},
                DispersionAllpass{-1.2113683901553876, 0.55660629474078105},
                DispersionAllpass{-0.74473280908625683, 0.47505868430485282},
                DispersionAllpass{-1.0089370094683032, 0.52095745810731353},
                DispersionAllpass{-0.38915268034793588, 0.41863650652372303}
            }}
        },
        // String 119, MIDI 69, f1 = 440.216000000000, B = 0.000839067334502, maxPartial = 20, loopDelaySamples = 57.660126147436, order = 16, sectionCount = 8
        DispersionPreset{
            69,
            440.21600000000001,
            0.00083906733450199965,
            44100,
            57.660126147435868,
            16,
            8,
            {{
                DispersionAllpass{-1.5681854964083857, 0.6172976546079344},
                DispersionAllpass{-1.5358575920688098, 0.61214543303126756},
                DispersionAllpass{-1.4685748465024888, 0.60102802282793877},
                DispersionAllpass{-1.3621705761559408, 0.58290385308016046},
                DispersionAllpass{-1.2111779320478524, 0.55661764958341198},
                DispersionAllpass{-1.0086102290968486, 0.52095243885104714},
                DispersionAllpass{-0.74422721548979553, 0.47502593164990614},
                DispersionAllpass{-0.38843845171522012, 0.41855812499001749}
            }}
        },
        // String 120, MIDI 70, f1 = 465.994000000000, B = 0.000922226240456, maxPartial = 20, loopDelaySamples = 53.320673919218, order = 16, sectionCount = 8
        DispersionPreset{
            70,
            465.99400000000003,
            0.00092222624045597843,
            44100,
            53.320673919217604,
            16,
            8,
            {{
                DispersionAllpass{-1.5753969440111788, 0.62321266434171474},
                DispersionAllpass{-1.5401609360348396, 0.6176388602528784},
                DispersionAllpass{-1.4670812961797193, 0.60576624849526561},
                DispersionAllpass{-1.3517724501229922, 0.58658317121493886},
                DispersionAllpass{-1.1882205425503831, 0.55882740839229084},
                DispersionAllpass{-0.9686864287946203, 0.52098201832278668},
                DispersionAllpass{-0.68254218649433929, 0.47158910635066159},
                DispersionAllpass{-0.30216892060298423, 0.40959033754121998}
            }}
        },
        // String 121, MIDI 70, f1 = 466.223000000000, B = 0.000922978427881, maxPartial = 20, loopDelaySamples = 53.284460379949, order = 16, sectionCount = 8
        DispersionPreset{
            70,
            466.22300000000001,
            0.0009229784278811439,
            44100,
            53.284460379948811,
            16,
            8,
            {{
                DispersionAllpass{-1.5754766298797904, 0.62327784976633671},
                DispersionAllpass{-1.5402114934092046, 0.61769900200288752},
                DispersionAllpass{-1.4670744467683516, 0.60581722816382688},
                DispersionAllpass{-1.18801098227197, 0.55885088475521549},
                DispersionAllpass{-0.96832275974010196, 0.52098499237328499},
                DispersionAllpass{-1.3516798862581332, 0.5866219334093109},
                DispersionAllpass{-0.30139678664392183, 0.40951359364340351},
                DispersionAllpass{-0.6819841631151633, 0.47156117130775999}
            }}
        },
        // String 122, MIDI 70, f1 = 466.452000000000, B = 0.0009237308501, maxPartial = 20, loopDelaySamples = 53.248281123308, order = 16, sectionCount = 8
        DispersionPreset{
            70,
            466.452,
            0.00092373085009990236,
            44100,
            53.248281123307841,
            16,
            8,
            {{
                DispersionAllpass{-1.5755479066592541, 0.62333627116092871},
                DispersionAllpass{-1.5402570065063477, 0.61775410735670278},
                DispersionAllpass{-1.4670684455881029, 0.60586657656063569},
                DispersionAllpass{-1.3515932542336604, 0.58666272310877432},
                DispersionAllpass{-0.96796343028120879, 0.52099227188049346},
                DispersionAllpass{-1.1878085793852999, 0.55887859901170656},
                DispersionAllpass{-0.68142630061454579, 0.47153600937396939},
                DispersionAllpass{-0.30062271566576421, 0.4094379331502121}
            }}
        },
        // String 123, MIDI 71, f1 = 493.767000000000, B = 0.00101514960484, maxPartial = 20, loopDelaySamples = 49.195757516436, order = 16, sectionCount = 8
        DispersionPreset{
            71,
            493.767,
            0.0010151496048357825,
            44100,
            49.195757516435528,
            16,
            8,
            {{
                DispersionAllpass{-1.5854592531542819, 0.63146464924212531},
                DispersionAllpass{-1.5466939220646896, 0.62531624186511836},
                DispersionAllpass{-1.3409213033737988, 0.59190745710529369},
                DispersionAllpass{-1.4666821207854384, 0.61243802887559218},
                DispersionAllpass{-1.162889327616186, 0.56238240425940678},
                DispersionAllpass{-0.92413989889616033, 0.52203658301027434},
                DispersionAllpass{-0.61396028917011747, 0.46880163285660642},
                DispersionAllpass{-0.20801088687733685, 0.40078408553428352}
            }}
        },
        // String 124, MIDI 71, f1 = 494.010000000000, B = 0.00101597762888, maxPartial = 20, loopDelaySamples = 49.161914160796, order = 16, sectionCount = 8
        DispersionPreset{
            71,
            494.00999999999999,
            0.0010159776288841188,
            44100,
            49.161914160796108,
            16,
            8,
            {{
                DispersionAllpass{-1.5855514334086815, 0.63154047429489024},
                DispersionAllpass{-1.4666789748083604, 0.61249875825209565},
                DispersionAllpass{-1.5467537506277076, 0.62538652344542001},
                DispersionAllpass{-1.3408243670509923, 0.59195581134578801},
                DispersionAllpass{-1.1626638675370045, 0.5624152258957561},
                DispersionAllpass{-0.61335488256654525, 0.468780373324172},
                DispersionAllpass{-0.92374500354760758, 0.52204804715575404},
                DispersionAllpass{-0.2071869895161052, 0.40071090008982918}
            }}
        },
        // String 125, MIDI 71, f1 = 494.252000000000, B = 0.00101680250172, maxPartial = 20, loopDelaySamples = 49.128237482006, order = 16, sectionCount = 8
        DispersionPreset{
            71,
            494.25200000000001,
            0.0010168025017225687,
            44100,
            49.128237482006277,
            16,
            8,
            {{
                DispersionAllpass{-1.5856474685746311, 0.63161938088625524},
                DispersionAllpass{-1.5468173067401105, 0.6254598704565385},
                DispersionAllpass{-1.4666794442534856, 0.61256264096283575},
                DispersionAllpass{-1.3407306751793202, 0.59200744443415609},
                DispersionAllpass{-0.20636262759219254, 0.40063812805704585},
                DispersionAllpass{-0.61274826090610335, 0.46876024492381546},
                DispersionAllpass{-0.92335020411094548, 0.52206175094479457},
                DispersionAllpass{-1.1624404093178919, 0.56245108648482622}
            }}
        },
        // String 126, MIDI 72, f1 = 523.195000000000, B = 0.00111728567555, maxPartial = 20, loopDelaySamples = 45.347272528017, order = 16, sectionCount = 8
        DispersionPreset{
            72,
            523.19500000000005,
            0.0011172856755547671,
            44100,
            45.34727252801698,
            16,
            8,
            {{
                DispersionAllpass{-1.59814848680976, 0.64192332134992747},
                DispersionAllpass{-1.5551459877858365, 0.63499608043199496},
                DispersionAllpass{-1.4669571443283973, 0.62077920378586804},
                DispersionAllpass{-1.3291849392866211, 0.59854659291095869},
                DispersionAllpass{-0.87516942963776501, 0.52390137657698232},
                DispersionAllpass{-1.1349465432961723, 0.56695824004293838},
                DispersionAllpass{-0.10777500265279866, 0.39236743576272148},
                DispersionAllpass{-0.53933161845384037, 0.46665535378455197}
            }}
        },
        // String 127, MIDI 72, f1 = 523.452000000000, B = 0.00111819403272, maxPartial = 20, loopDelaySamples = 45.315760858872, order = 16, sectionCount = 8
        DispersionPreset{
            72,
            523.452,
            0.0011181940327211753,
            44100,
            45.315760858872402,
            16,
            8,
            {{
                DispersionAllpass{-1.5982679327254168, 0.64202210598488507},
                DispersionAllpass{-1.5552260860497165, 0.6350872604279002},
                DispersionAllpass{-1.4669619106970064, 0.62085737363838689},
                DispersionAllpass{-1.3290805119184723, 0.5986085764648289},
                DispersionAllpass{-1.1346968407668268, 0.56700133181653112},
                DispersionAllpass{-0.87473349856003579, 0.5239205454910556},
                DispersionAllpass{-0.53867160909759315, 0.46663993277355287},
                DispersionAllpass{-0.10689935498338504, 0.39229803732932639}
            }}
        },
        // String 128, MIDI 72, f1 = 523.709000000000, B = 0.00111910267122, maxPartial = 20, loopDelaySamples = 45.284280998652, order = 16, sectionCount = 8
        DispersionPreset{
            72,
            523.70899999999995,
            0.0011191026712183991,
            44100,
            45.284280998651589,
            16,
            8,
            {{
                DispersionAllpass{-1.5983899194257949, 0.64212295647265283},
                DispersionAllpass{-1.5553084234984209, 0.63518044441689614},
                DispersionAllpass{-1.4669680865995387, 0.62093725268416422},
                DispersionAllpass{-1.3289762880278293, 0.59867172358938558},
                DispersionAllpass{-1.1344463140377696, 0.5670450160522762},
                DispersionAllpass{-0.53801019291579777, 0.4666246121649188},
                DispersionAllpass{-0.87429627963762235, 0.52393996151524214},
                DispersionAllpass{-0.10602256766135294, 0.39222863364071026}
            }}
        },
        // String 129, MIDI 73, f1 = 554.376000000000, B = 0.00122953029546, maxPartial = 14, loopDelaySamples = 47.923638793902, order = 12, sectionCount = 6
        DispersionPreset{
            73,
            554.37599999999998,
            0.0012295302954572266,
            44100,
            47.923638793902484,
            12,
            6,
            {{
                DispersionAllpass{-1.4674396222275561, 0.5418608618496159},
                DispersionAllpass{-1.4160854788341446, 0.53269884156789027},
                DispersionAllpass{-1.3085108326113193, 0.51296816755541719},
                DispersionAllpass{-1.1377513835069293, 0.48125342351018446},
                DispersionAllpass{-0.89482476311622638, 0.43719843550278714},
                DispersionAllpass{-0.55265371789776552, 0.38276254998233372},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        },
        // String 130, MIDI 73, f1 = 554.649000000000, B = 0.00123053101872, maxPartial = 14, loopDelaySamples = 47.892739371443, order = 12, sectionCount = 6
        DispersionPreset{
            73,
            554.649,
            0.0012305310187171968,
            44100,
            47.892739371443291,
            12,
            6,
            {{
                DispersionAllpass{-1.4673091086751773, 0.54176658393846888},
                DispersionAllpass{-1.4159248640210635, 0.53259726025751075},
                DispersionAllpass{-1.3082937161165371, 0.51285370677883357},
                DispersionAllpass{-1.1374571208366633, 0.48112231605138278},
                DispersionAllpass{-0.89444575334066445, 0.43705116210639194},
                DispersionAllpass{-0.55219375995227027, 0.38260925489343212},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        },
        // String 131, MIDI 73, f1 = 554.921000000000, B = 0.00123152838419, maxPartial = 14, loopDelaySamples = 47.861986013732, order = 12, sectionCount = 6
        DispersionPreset{
            73,
            554.92100000000005,
            0.0012315283841869743,
            44100,
            47.861986013732093,
            12,
            6,
            {{
                DispersionAllpass{-1.4671751829139446, 0.5416697352114882},
                DispersionAllpass{-1.415762389201253, 0.53249376634880585},
                DispersionAllpass{-1.3080773783113944, 0.5127385769756988},
                DispersionAllpass{-1.1371659663306832, 0.48099185779917403},
                DispersionAllpass{-0.89407065006566311, 0.43690534506303841},
                DispersionAllpass{-0.55173692179807521, 0.38245747118482748},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        },
        // String 132, MIDI 74, f1 = 587.416000000000, B = 0.00135287306594, maxPartial = 14, loopDelaySamples = 44.388135294889, order = 12, sectionCount = 6
        DispersionPreset{
            74,
            587.41600000000005,
            0.0013528730659360278,
            44100,
            44.388135294888812,
            12,
            6,
            {{
                DispersionAllpass{-1.4536636886356304, 0.53198078242188584},
                DispersionAllpass{-1.3988836523002504, 0.52207712934731998},
                DispersionAllpass{-1.2849513163891118, 0.50113142226384455},
                DispersionAllpass{-0.85211047930817485, 0.42277948462922205},
                DispersionAllpass{-1.1054632121611265, 0.46801159195180864},
                DispersionAllpass{-0.49685511085534317, 0.36755429442796761},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        },
        // String 133, MIDI 74, f1 = 587.705000000000, B = 0.00135397161187, maxPartial = 14, loopDelaySamples = 44.358927319672, order = 12, sectionCount = 6
        DispersionPreset{
            74,
            587.70500000000004,
            0.0013539716118675233,
            44100,
            44.358927319671821,
            12,
            6,
            {{
                DispersionAllpass{-1.4535743341872946, 0.53191762709687718},
                DispersionAllpass{-1.3987631811814392, 0.52200768488978788},
                DispersionAllpass{-1.2847737367837282, 0.50105231328172772},
                DispersionAllpass{-1.1052068682211005, 0.46792261568444582},
                DispersionAllpass{-0.85175200599902035, 0.4226823893119625},
                DispersionAllpass{-0.49635199492874243, 0.36744474636086438},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        },
        // String 134, MIDI 74, f1 = 587.994000000000, B = 0.00135507049604, maxPartial = 14, loopDelaySamples = 44.329752191872, order = 12, sectionCount = 6
        DispersionPreset{
            74,
            587.99400000000003,
            0.0013550704960375276,
            44100,
            44.329752191872494,
            12,
            6,
            {{
                DispersionAllpass{-1.4534826499312314, 0.53185274395132032},
                DispersionAllpass{-1.3986410819701829, 0.52193680462479797},
                DispersionAllpass{-1.2845956127150087, 0.50097222876818637},
                DispersionAllpass{-0.85139503434720609, 0.42258547031661908},
                DispersionAllpass{-1.1049511700054413, 0.46783321014093593},
                DispersionAllpass{-0.49585006029511908, 0.36733559735363602},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        },
        // String 135, MIDI 75, f1 = 622.426000000000, B = 0.00148839369246, maxPartial = 14, loopDelaySamples = 41.048374432446, order = 12, sectionCount = 6
        DispersionPreset{
            75,
            622.42600000000004,
            0.0014883936924585063,
            44100,
            41.048374432445584,
            12,
            6,
            {{
                DispersionAllpass{-1.4466862557226103, 0.52716961667471673},
                DispersionAllpass{-1.3879863918088873, 0.51656436113825366},
                DispersionAllpass{-1.266718802898682, 0.49454358168411211},
                DispersionAllpass{-1.0766468097191029, 0.46024821736765015},
                DispersionAllpass{-0.80869199982115902, 0.41366995278852875},
                DispersionAllpass{-0.43319536019100585, 0.35592390530854495},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        },
        // String 136, MIDI 75, f1 = 622.732000000000, B = 0.00148959970642, maxPartial = 14, loopDelaySamples = 41.020873190053, order = 12, sectionCount = 6
        DispersionPreset{
            75,
            622.73199999999997,
            0.0014895997064160145,
            44100,
            41.020873190053408,
            12,
            6,
            {{
                DispersionAllpass{-1.4466609985177346, 0.52715390001309625},
                DispersionAllpass{-1.3879230693074136, 0.51654190641429631},
                DispersionAllpass{-1.266583690427068, 0.4945105034747922},
                DispersionAllpass{-1.0764066778507793, 0.46020265616205835},
                DispersionAllpass{-0.8083054627696945, 0.41360784255720917},
                DispersionAllpass{-0.4326158522591369, 0.35583219876095951},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        },
        // String 137, MIDI 75, f1 = 623.037000000000, B = 0.00149080214701, maxPartial = 14, loopDelaySamples = 40.993477458913, order = 12, sectionCount = 6
        DispersionPreset{
            75,
            623.03700000000003,
            0.0014908021470083413,
            44100,
            40.993477458913226,
            12,
            6,
            {{
                DispersionAllpass{-1.4466402248757351, 0.52714144069493751},
                DispersionAllpass{-1.3878648594280989, 0.51652323846743242},
                DispersionAllpass{-1.2664536507800312, 0.4944817433971177},
                DispersionAllpass{-1.0761693956347009, 0.46016109798013105},
                DispersionAllpass{-0.80791805337033662, 0.41354832566100747},
                DispersionAllpass{-0.43203356143756794, 0.35574152721953711},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        },
        // String 138, MIDI 76, f1 = 659.521000000000, B = 0.00163726439948, maxPartial = 14, loopDelaySamples = 37.908257409799, order = 12, sectionCount = 6
        DispersionPreset{
            76,
            659.52099999999996,
            0.0016372643994772614,
            44100,
            37.908257409798878,
            12,
            6,
            {{
                DispersionAllpass{-1.4474863865614633, 0.5281081460361654},
                DispersionAllpass{-1.3836472492557863, 0.51659432387688264},
                DispersionAllpass{-1.2525445326352116, 0.49304708947589398},
                DispersionAllpass{-1.047980515139902, 0.45678128356151054},
                DispersionAllpass{-0.76027658261278208, 0.40750658244713539},
                DispersionAllpass{-0.36055195515808858, 0.3455062181099523},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        },
        // String 139, MIDI 76, f1 = 659.845000000000, B = 0.00163858821327, maxPartial = 14, loopDelaySamples = 37.882470025800, order = 12, sectionCount = 6
        DispersionPreset{
            76,
            659.84500000000003,
            0.0016385882132663129,
            44100,
            37.882470025800302,
            12,
            6,
            {{
                DispersionAllpass{-1.4475237975366391, 0.52813882411939705},
                DispersionAllpass{-1.3836342143733946, 0.51661568249337519},
                DispersionAllpass{-1.2524347420576463, 0.49305215399043928},
                DispersionAllpass{-1.0477298263333721, 0.45676481302262451},
                DispersionAllpass{-0.75983987071629311, 0.40746148858653147},
                DispersionAllpass{-0.3599007546418298, 0.3454199682413514},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        },
        // String 140, MIDI 76, f1 = 660.169000000000, B = 0.00163991243101, maxPartial = 14, loopDelaySamples = 37.856716744002, order = 12, sectionCount = 6
        DispersionPreset{
            76,
            660.16899999999998,
            0.0016399124310108822,
            44100,
            37.85671674400249,
            12,
            6,
            {{
                DispersionAllpass{-1.0474809631346271, 0.45674720440838368},
                DispersionAllpass{-1.3836164487927076, 0.51663281077826662},
                DispersionAllpass{-1.4475544301870484, 0.52816447805094191},
                DispersionAllpass{-1.2523237002275416, 0.49305450279825747},
                DispersionAllpass{-0.75940637544046286, 0.40741630629745312},
                DispersionAllpass{-0.35925259422824979, 0.34533401400337549},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        },
        // String 141, MIDI 77, f1 = 698.828000000000, B = 0.00180078986428, maxPartial = 14, loopDelaySamples = 34.966659091796, order = 12, sectionCount = 6
        DispersionPreset{
            77,
            698.82799999999997,
            0.0018007898642832853,
            44100,
            34.96665909179621,
            12,
            6,
            {{
                DispersionAllpass{-1.4542162962607197, 0.53346787954181962},
                DispersionAllpass{-1.3838167209734558, 0.520730092637265},
                DispersionAllpass{-1.2401335208799127, 0.49501636538160737},
                DispersionAllpass{-1.017351463465223, 0.45584888554064457},
                DispersionAllpass{-0.7060801881291725, 0.40275774076185128},
                DispersionAllpass{-0.2804789856585444, 0.33552709712605466},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        },
        // String 142, MIDI 77, f1 = 699.171000000000, B = 0.00180224253962, maxPartial = 14, loopDelaySamples = 34.942558337027, order = 12, sectionCount = 6
        DispersionPreset{
            77,
            699.17100000000005,
            0.0018022425396217285,
            44100,
            34.942558337026817,
            12,
            6,
            {{
                DispersionAllpass{-1.4542925002331106, 0.53352782587004632},
                DispersionAllpass{-1.3838314910587317, 0.52077833980949284},
                DispersionAllpass{-1.2400308846380608, 0.49504374500067122},
                DispersionAllpass{-1.0170807623539262, 0.45584840835504331},
                DispersionAllpass{-0.70559702312921457, 0.40272153249576764},
                DispersionAllpass{-0.27977307748339136, 0.33544394515141435},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        },
        // String 143, MIDI 77, f1 = 699.515000000000, B = 0.0018036998933, maxPartial = 14, loopDelaySamples = 34.918412127011, order = 12, sectionCount = 6
        DispersionPreset{
            77,
            699.51499999999999,
            0.0018036998933031727,
            44100,
            34.918412127011138,
            12,
            6,
            {{
                DispersionAllpass{-1.4543696637493428, 0.53358849764023231},
                DispersionAllpass{-1.3838469358401233, 0.52082726277803815},
                DispersionAllpass{-1.2399283727158594, 0.49507170961307156},
                DispersionAllpass{-1.0168093624587689, 0.4558483895180539},
                DispersionAllpass{-0.7051120274261713, 0.40268552557186243},
                DispersionAllpass{-0.27906452556041506, 0.33536066413378945},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        },
        // String 144, MIDI 78, f1 = 740.477000000000, B = 0.00198038017483, maxPartial = 14, loopDelaySamples = 32.216814075527, order = 12, sectionCount = 6
        DispersionPreset{
            78,
            740.47699999999998,
            0.0019803801748292742,
            44100,
            32.21681407552655,
            12,
            6,
            {{
                DispersionAllpass{-1.4653309949001778, 0.54217792774478868},
                DispersionAllpass{-1.3869590956531781, 0.52789427675433898},
                DispersionAllpass{-1.2281169499915581, 0.49941499921135846},
                DispersionAllpass{-0.64620364361653149, 0.39892868701119005},
                DispersionAllpass{-0.19416218817323797, 0.3259024532559387},
                DispersionAllpass{-0.98392305333080665, 0.45659051337573792},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        },
        // String 145, MIDI 78, f1 = 740.841000000000, B = 0.00198197792694, maxPartial = 14, loopDelaySamples = 32.194260099215, order = 12, sectionCount = 6
        DispersionPreset{
            78,
            740.84100000000001,
            0.0019819779269423863,
            44100,
            32.194260099214603,
            12,
            6,
            {{
                DispersionAllpass{-1.4654444196639671, 0.54226668449974791},
                DispersionAllpass{-1.3869983731864077, 0.52796849765971698},
                DispersionAllpass{-1.2280157582597406, 0.49946311140558824},
                DispersionAllpass{-0.98362564337752367, 0.45660424891823392},
                DispersionAllpass{-0.64566925429445654, 0.39890033009163156},
                DispersionAllpass{-0.19340269624676801, 0.32582254622024298},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        },
        // String 146, MIDI 78, f1 = 741.205000000000, B = 0.00198357616323, maxPartial = 14, loopDelaySamples = 32.171734648502, order = 12, sectionCount = 6
        DispersionPreset{
            78,
            741.20500000000004,
            0.0019835761632297278,
            44100,
            32.171734648502465,
            12,
            6,
            {{
                DispersionAllpass{-1.4655545595062214, 0.54235300241814643},
                DispersionAllpass{-1.3870345644450686, 0.52804019924672529},
                DispersionAllpass{-1.2279124392889296, 0.49950879052236402},
                DispersionAllpass{-0.64513715508875547, 0.39887117886595458},
                DispersionAllpass{-0.19264611915302646, 0.32574265824323462},
                DispersionAllpass{-0.98332823625988142, 0.45661614939175416},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        },
        // String 147, MIDI 79, f1 = 784.608000000000, B = 0.00217758878306, maxPartial = 14, loopDelaySamples = 29.648759448690, order = 12, sectionCount = 6
        DispersionPreset{
            79,
            784.60799999999995,
            0.0021775887830562946,
            44100,
            29.648759448690416,
            12,
            6,
            {{
                DispersionAllpass{-1.4804849159291278, 0.5540734507101257},
                DispersionAllpass{-1.3926024794069261, 0.53789465429567207},
                DispersionAllpass{-1.2158897205231995, 0.50602967868457494},
                DispersionAllpass{-0.58040822334999898, 0.39598126015515484},
                DispersionAllpass{-0.9471328236369656, 0.45883439955940519},
                DispersionAllpass{-0.10207210979801069, 0.316745833067065},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        },
        // String 148, MIDI 79, f1 = 784.994000000000, B = 0.00217934456215, maxPartial = 14, loopDelaySamples = 29.627689520407, order = 12, sectionCount = 6
        DispersionPreset{
            79,
            784.99400000000003,
            0.0021793445621545167,
            44100,
            29.627689520407205,
            12,
            6,
            {{
                DispersionAllpass{-1.4806309748631505, 0.55418848885537886},
                DispersionAllpass{-1.3926605429699259, 0.53799194112532522},
                DispersionAllpass{-1.2157835620974935, 0.50609534267124401},
                DispersionAllpass{-0.94680423048255236, 0.45885982961366045},
                DispersionAllpass{-0.57982296170722769, 0.3959601630573969},
                DispersionAllpass{-0.10126617149671936, 0.31667056397519672},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        },
        // String 149, MIDI 79, f1 = 785.379000000000, B = 0.002181096321, maxPartial = 14, loopDelaySamples = 29.606694846165, order = 12, sectionCount = 6
        DispersionPreset{
            79,
            785.37900000000002,
            0.0021810963210037405,
            44100,
            29.60669484616534,
            12,
            6,
            {{
                DispersionAllpass{-1.480778647884595, 0.55430474864403845},
                DispersionAllpass{-1.3927202003193744, 0.53809049375789098},
                DispersionAllpass{-1.2156787258144368, 0.50616225482867139},
                DispersionAllpass{-0.94647631256359055, 0.45888628286126659},
                DispersionAllpass{-0.57923785867220845, 0.39593973093599355},
                DispersionAllpass{-0.10046075311753819, 0.31659567452939469},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        },
        // String 150, MIDI 80, f1 = 831.370000000000, B = 0.00239411982375, maxPartial = 8, loopDelaySamples = 36.489210110771, order = 6, sectionCount = 3
        DispersionPreset{
            80,
            831.37,
            0.0023941198237539172,
            44100,
            36.489210110770742,
            6,
            3,
            {{
                DispersionAllpass{-1.2998353983589208, 0.42947033414947972},
                DispersionAllpass{-1.1771481820539134, 0.40885021242317554},
                DispersionAllpass{-0.89553407588307099, 0.36426283732652009},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        },
        // String 151, MIDI 80, f1 = 831.778000000000, B = 0.00239604273152, maxPartial = 8, loopDelaySamples = 36.466772447387, order = 6, sectionCount = 3
        DispersionPreset{
            80,
            831.77800000000002,
            0.0023960427315172735,
            44100,
            36.466772447387484,
            6,
            3,
            {{
                DispersionAllpass{-1.2998035803825381, 0.42945442565713554},
                DispersionAllpass{-1.1770306773136692, 0.4088194328369934},
                DispersionAllpass{-0.89523258316851384, 0.36419770538226226},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        },
        // String 152, MIDI 80, f1 = 832.187000000000, B = 0.00239797093173, maxPartial = 8, loopDelaySamples = 36.444303124668, order = 6, sectionCount = 3
        DispersionPreset{
            80,
            832.18700000000001,
            0.0023979709317304625,
            44100,
            36.444303124667684,
            6,
            3,
            {{
                DispersionAllpass{-1.2997717057568881, 0.42943849374573512},
                DispersionAllpass{-1.1769128913023488, 0.40878859173865439},
                DispersionAllpass{-0.89493036829768924, 0.36413243971215287},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        },
        // String 153, MIDI 81, f1 = 880.918000000000, B = 0.00263182535301, maxPartial = 8, loopDelaySamples = 33.923941236889, order = 6, sectionCount = 3
        DispersionPreset{
            81,
            880.91800000000001,
            0.002631825353009612,
            44100,
            33.923941236889164,
            6,
            3,
            {{
                DispersionAllpass{-1.2963591845836719, 0.42780640706319534},
                DispersionAllpass{-1.1631301975318293, 0.40538582489645075},
                DispersionAllpass{-0.8589956944032876, 0.3566429865699986},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        },
        // String 154, MIDI 81, f1 = 881.351000000000, B = 0.00263393953793, maxPartial = 8, loopDelaySamples = 33.902859634595, order = 6, sectionCount = 3
        DispersionPreset{
            81,
            881.351,
            0.0026339395379290798,
            44100,
            33.902859634595224,
            6,
            3,
            {{
                DispersionAllpass{-1.2963325991325965, 0.4277944856642461},
                DispersionAllpass{-0.85867678920659085, 0.35657893788192951},
                DispersionAllpass{-1.16300995240015, 0.40535809105266296},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        },
        // String 155, MIDI 81, f1 = 881.784000000000, B = 0.00263605435616, maxPartial = 8, loopDelaySamples = 33.881800413395, order = 6, sectionCount = 3
        DispersionPreset{
            81,
            881.78399999999999,
            0.0026360543561619763,
            44100,
            33.881800413394984,
            6,
            3,
            {{
                DispersionAllpass{-1.2963057886703409, 0.42778241058115768},
                DispersionAllpass{-1.1628896456996849, 0.4053302735544736},
                DispersionAllpass{-0.85835798845673794, 0.35651489295348193},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        },
        // String 156, MIDI 82, f1 = 933.564000000000, B = 0.00289347887727, maxPartial = 8, loopDelaySamples = 31.511652314686, order = 6, sectionCount = 3
        DispersionPreset{
            82,
            933.56399999999996,
            0.0028934788772695488,
            44100,
            31.511652314685811,
            6,
            3,
            {{
                DispersionAllpass{-1.2935228152917673, 0.42663128358837965},
                DispersionAllpass{-1.1487426602638622, 0.40228891306319986},
                DispersionAllpass{-0.82026877272329801, 0.34914861539425418},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        },
        // String 157, MIDI 82, f1 = 934.023000000000, B = 0.00289580057957, maxPartial = 8, loopDelaySamples = 31.491881461918, order = 6, sectionCount = 3
        DispersionPreset{
            82,
            934.02300000000002,
            0.0028958005795701332,
            44100,
            31.491881461917757,
            6,
            3,
            {{
                DispersionAllpass{-1.2935015355736552, 0.42662342933481856},
                DispersionAllpass{-1.1486191894499376, 0.40226430070621738},
                DispersionAllpass{-0.8199314401644161, 0.34908580746889156},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        },
        // String 158, MIDI 82, f1 = 934.481000000000, B = 0.00289811791441, maxPartial = 8, loopDelaySamples = 31.472174095841, order = 6, sectionCount = 3
        DispersionPreset{
            82,
            934.48099999999999,
            0.0028981179144138163,
            44100,
            31.472174095841329,
            6,
            3,
            {{
                DispersionAllpass{-1.2934804761936682, 0.42661571419665806},
                DispersionAllpass{-1.1484959934276993, 0.40223979725118231},
                DispersionAllpass{-0.8195947841451936, 0.34902315424215491},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        },
        // String 159, MIDI 83, f1 = 989.356000000000, B = 0.00318071697105, maxPartial = 8, loopDelaySamples = 29.250367292691, order = 6, sectionCount = 3
        DispersionPreset{
            83,
            989.35599999999999,
            0.0031807169710454416,
            44100,
            29.25036729269064,
            6,
            3,
            {{
                DispersionAllpass{-1.2913263877153862, 0.4259483475780167},
                DispersionAllpass{-1.1339381044295984, 0.39956624920334122},
                DispersionAllpass{-0.77928995891889152, 0.34180736585884469},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        },
        // String 160, MIDI 83, f1 = 989.842000000000, B = 0.00318326327074, maxPartial = 8, loopDelaySamples = 29.231853770199, order = 6, sectionCount = 3
        DispersionPreset{
            83,
            989.84199999999998,
            0.0031832632707381259,
            44100,
            29.231853770199329,
            6,
            3,
            {{
                DispersionAllpass{-1.2913107041946581, 0.4259448037102439},
                DispersionAllpass{-1.1338108816592825, 0.3995448974531195},
                DispersionAllpass{-0.77893309171438152, 0.34174596014873393},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        },
        // String 161, MIDI 83, f1 = 990.328000000000, B = 0.00318581032684, maxPartial = 8, loopDelaySamples = 29.213359671174, order = 6, sectionCount = 3
        DispersionPreset{
            83,
            990.32799999999997,
            0.0031858103268383163,
            44100,
            29.21335967117437,
            6,
            3,
            {{
                DispersionAllpass{-1.2912950082641972, 0.42594125305451935},
                DispersionAllpass{-1.1336836393695493, 0.39952353901249521},
                DispersionAllpass{-0.77857625473478476, 0.3416845768699176},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        },
        // String 162, MIDI 84, f1 = 1048.480000000000, B = 0.00349598741746, maxPartial = 8, loopDelaySamples = 27.131488229460, order = 6, sectionCount = 3
        DispersionPreset{
            84,
            1048.48,
            0.003495987417455864,
            44100,
            27.131488229460111,
            6,
            3,
            {{
                DispersionAllpass{-1.2897916416874817, 0.42577943247766481},
                DispersionAllpass{-1.1186337153434858, 0.39722919574153942},
                DispersionAllpass{-0.73588584501062626, 0.33463283158295093},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        },
        // String 163, MIDI 84, f1 = 1049.000000000000, B = 0.00349880902676, maxPartial = 8, loopDelaySamples = 27.113976541190, order = 6, sectionCount = 3
        DispersionPreset{
            84,
            1049,
            0.0034988090267594118,
            44100,
            27.113976541190169,
            6,
            3,
            {{
                DispersionAllpass{-1.2897815234249577, 0.42578032604223942},
                DispersionAllpass{-1.1185005818615961, 0.39721096068842188},
                DispersionAllpass{-0.73550406248349143, 0.33457231553650174},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        },
        // String 164, MIDI 84, f1 = 1049.510000000000, B = 0.00350157719315, maxPartial = 8, loopDelaySamples = 27.096819459068, order = 6, sectionCount = 3
        DispersionPreset{
            84,
            1049.51,
            0.0035015771931539356,
            44100,
            27.096819459068158,
            6,
            3,
            {{
                DispersionAllpass{-1.2897716987380827, 0.42578127117800074},
                DispersionAllpass{-0.73512959920051468, 0.3345130132618771},
                DispersionAllpass{-1.1183700512928079, 0.39719313822995717},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        },
        // String 165, MIDI 85, f1 = 1111.140000000000, B = 0.00384199490486, maxPartial = 6, loopDelaySamples = 28.697633281009, order = 4, sectionCount = 2
        DispersionPreset{
            85,
            1111.1400000000001,
            0.0038419949048638536,
            44100,
            28.697633281008663,
            4,
            2,
            {{
                DispersionAllpass{-0.9789392675864349, 0.34540281844145015},
                DispersionAllpass{-1.219618800430599, 0.38414645004954473},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        },
        // String 166, MIDI 85, f1 = 1111.690000000000, B = 0.00384507943094, maxPartial = 6, loopDelaySamples = 28.680291960070, order = 4, sectionCount = 2
        DispersionPreset{
            85,
            1111.6900000000001,
            0.0038450794309375725,
            44100,
            28.680291960069852,
            4,
            2,
            {{
                DispersionAllpass{-1.2195624138986687, 0.3841197330348084},
                DispersionAllpass{-0.97872568005871641, 0.34534915331079796},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        },
        // String 167, MIDI 85, f1 = 1112.230000000000, B = 0.00384810759018, maxPartial = 6, loopDelaySamples = 28.663284243010, order = 4, sectionCount = 2
        DispersionPreset{
            85,
            1112.23,
            0.0038481075901828548,
            44100,
            28.663284243010189,
            4,
            2,
            {{
                DispersionAllpass{-0.97851591520853221, 0.34529647210112957},
                DispersionAllpass{-1.2195070725132824, 0.38409352102991362},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        },
        // String 168, MIDI 86, f1 = 1177.550000000000, B = 0.00421196195612, maxPartial = 6, loopDelaySamples = 26.732846542177, order = 4, sectionCount = 2
        DispersionPreset{
            86,
            1177.55,
            0.0042119619561208719,
            44100,
            26.732846542176816,
            4,
            2,
            {{
                DispersionAllpass{-1.2125112619424752, 0.38076302540099882},
                DispersionAllpass{-0.95274002352093257, 0.33885904924919452},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        },
        // String 169, MIDI 86, f1 = 1178.120000000000, B = 0.00421511273474, maxPartial = 6, loopDelaySamples = 26.717043134563, order = 4, sectionCount = 2
        DispersionPreset{
            86,
            1178.1199999999999,
            0.0042151127347369224,
            44100,
            26.717043134562616,
            4,
            2,
            {{
                DispersionAllpass{-1.2124476803470181, 0.38073263307466193},
                DispersionAllpass{-0.95251172867647182, 0.33880236287995547},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        },
        // String 170, MIDI 86, f1 = 1178.700000000000, B = 0.00421831830757, maxPartial = 6, loopDelaySamples = 26.700980092702, order = 4, sectionCount = 2
        DispersionPreset{
            86,
            1178.7,
            0.0042183183075681406,
            44100,
            26.70098009270221,
            4,
            2,
            {{
                DispersionAllpass{-1.2123828424318193, 0.38070161974174449},
                DispersionAllpass{-0.9522793759132091, 0.3387446440547886},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        },
        // String 171, MIDI 87, f1 = 1247.920000000000, B = 0.00459703934735, maxPartial = 6, loopDelaySamples = 24.902575659149, order = 4, sectionCount = 2
        DispersionPreset{
            87,
            1247.9200000000001,
            0.0045970393473528538,
            44100,
            24.902575659149289,
            4,
            2,
            {{
                DispersionAllpass{-1.2043479904228793, 0.37684886048931793},
                DispersionAllpass{-0.92415227346661943, 0.33180562941508929},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        },
        // String 172, MIDI 87, f1 = 1248.530000000000, B = 0.00460033996311, maxPartial = 6, loopDelaySamples = 24.887711347419, order = 4, sectionCount = 2
        DispersionPreset{
            87,
            1248.53,
            0.0046003399631090302,
            44100,
            24.887711347419355,
            4,
            2,
            {{
                DispersionAllpass{-1.2042745211289043, 0.37681354212468215},
                DispersionAllpass{-0.92390107076397543, 0.33174404588399736},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        },
        // String 173, MIDI 87, f1 = 1249.150000000000, B = 0.00460369397778, maxPartial = 6, loopDelaySamples = 24.872619859510, order = 4, sectionCount = 2
        DispersionPreset{
            87,
            1249.1500000000001,
            0.004603693977777587,
            44100,
            24.872619859509854,
            4,
            2,
            {{
                DispersionAllpass{-1.2041998379025756, 0.37677764595166924},
                DispersionAllpass{-0.9236456896226557, 0.33168145633456869},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        },
        // String 174, MIDI 88, f1 = 1322.500000000000, B = 0.00499511454054, maxPartial = 6, loopDelaySamples = 23.198059208008, order = 4, sectionCount = 2
        DispersionPreset{
            88,
            1322.5,
            0.0049951145405383321,
            44100,
            23.198059208008026,
            4,
            2,
            {{
                DispersionAllpass{-1.1950535100831949, 0.37237587603966715},
                DispersionAllpass{-0.89304770101447917, 0.32423660624042178},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        },
        // String 175, MIDI 88, f1 = 1323.150000000000, B = 0.0049985326872, maxPartial = 6, loopDelaySamples = 23.184144052633, order = 4, sectionCount = 2
        DispersionPreset{
            88,
            1323.1500000000001,
            0.0049985326872045224,
            44100,
            23.184144052632654,
            4,
            2,
            {{
                DispersionAllpass{-1.1949697785754065, 0.37233554535692298},
                DispersionAllpass{-0.89277332579023727, 0.32417033032746267},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        },
        // String 176, MIDI 88, f1 = 1323.800000000000, B = 0.00500194990155, maxPartial = 6, loopDelaySamples = 23.170244150539, order = 4, sectionCount = 2
        DispersionPreset{
            88,
            1323.8,
            0.0050019499015450716,
            44100,
            23.170244150538878,
            4,
            2,
            {{
                DispersionAllpass{-1.1948860151944261, 0.37229520200113442},
                DispersionAllpass{-0.89249889002425398, 0.32410405033299439},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        },
        // String 177, MIDI 89, f1 = 1401.530000000000, B = 0.0054035704787, maxPartial = 6, loopDelaySamples = 21.611491183314, order = 4, sectionCount = 2
        DispersionPreset{
            89,
            1401.53,
            0.0054035704786987411,
            44100,
            21.611491183313895,
            4,
            2,
            {{
                DispersionAllpass{-1.1845598493294163, 0.36732331081897235},
                DispersionAllpass{-0.85931505461244384, 0.31615515075832284},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        },
        // String 178, MIDI 89, f1 = 1402.220000000000, B = 0.00540707071469, maxPartial = 6, loopDelaySamples = 21.598518021131, order = 4, sectionCount = 2
        DispersionPreset{
            89,
            1402.22,
            0.0054070707146882764,
            44100,
            21.598518021131216,
            4,
            2,
            {{
                DispersionAllpass{-1.1844655842435603, 0.36727796449486561},
                DispersionAllpass{-0.85901742836707828, 0.31608445899193294},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        },
        // String 179, MIDI 89, f1 = 1402.910000000000, B = 0.00541056976775, maxPartial = 6, loopDelaySamples = 21.585559144860, order = 4, sectionCount = 2
        DispersionPreset{
            89,
            1402.9100000000001,
            0.0054105697677488025,
            44100,
            21.585559144859793,
            4,
            2,
            {{
                DispersionAllpass{-0.85871975273200307, 0.31601376607694304},
                DispersionAllpass{-1.1843712736039527, 0.3672325969322936},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        },
        // String 180, MIDI 90, f1 = 1485.290000000000, B = 0.00581956120694, maxPartial = 6, loopDelaySamples = 20.134880255434, order = 4, sectionCount = 2
        DispersionPreset{
            90,
            1485.29,
            0.0058195612069360638,
            44100,
            20.134880255434076,
            4,
            2,
            {{
                DispersionAllpass{-1.1728023482629517, 0.36167617360411797},
                DispersionAllpass{-0.82284354301340823, 0.30757022100396392},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        },
        // String 181, MIDI 90, f1 = 1486.020000000000, B = 0.00582310558937, maxPartial = 6, loopDelaySamples = 20.122829099539, order = 4, sectionCount = 2
        DispersionPreset{
            90,
            1486.02,
            0.0058231055893683322,
            44100,
            20.122829099539388,
            4,
            2,
            {{
                DispersionAllpass{-1.1726972571919643, 0.36162580533504696},
                DispersionAllpass{-0.82252285984910745, 0.30749545019450919},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        },
        // String 182, MIDI 90, f1 = 1486.750000000000, B = 0.00582664853505, maxPartial = 6, loopDelaySamples = 20.110791142194, order = 4, sectionCount = 2
        DispersionPreset{
            90,
            1486.75,
            0.0058266485350518971,
            44100,
            20.110791142193808,
            4,
            2,
            {{
                DispersionAllpass{-1.172592172556316, 0.36157545158956816},
                DispersionAllpass{-0.82220212195737741, 0.30742069575235792},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        },
        // String 183, MIDI 91, f1 = 1574.060000000000, B = 0.00623982035574, maxPartial = 6, loopDelaySamples = 18.760919461928, order = 4, sectionCount = 2
        DispersionPreset{
            91,
            1574.0599999999999,
            0.0062398203557380589,
            44100,
            18.76091946192799,
            4,
            2,
            {{
                DispersionAllpass{-1.1597282530566202, 0.35542998160783285},
                DispersionAllpass{-0.78354447455153975, 0.29850291047886951},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        },
        // String 184, MIDI 91, f1 = 1574.830000000000, B = 0.0062433692165, maxPartial = 6, loopDelaySamples = 18.749759769900, order = 4, sectionCount = 2
        DispersionPreset{
            91,
            1574.8299999999999,
            0.006243369216496159,
            44100,
            18.749759769899711,
            4,
            2,
            {{
                DispersionAllpass{-1.1596123950672121, 0.35537482854735497},
                DispersionAllpass{-0.78320115705027216, 0.29842455792389783},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        },
        // String 185, MIDI 91, f1 = 1575.600000000000, B = 0.00624691639121, maxPartial = 6, loopDelaySamples = 18.738612396046, order = 4, sectionCount = 2
        DispersionPreset{
            91,
            1575.5999999999999,
            0.0062469163912102438,
            44100,
            18.738612396046371,
            4,
            2,
            {{
                DispersionAllpass{-1.1594964348556318, 0.35531961913858434},
                DispersionAllpass{-0.78285781443564251, 0.29834619191929457},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        },
        // String 186, MIDI 92, f1 = 1668.120000000000, B = 0.00666070895619, maxPartial = 6, loopDelaySamples = 17.482815315055, order = 4, sectionCount = 2
        DispersionPreset{
            92,
            1668.1199999999999,
            0.0066607089561924035,
            44100,
            17.482815315054538,
            4,
            2,
            {{
                DispersionAllpass{-1.1453002446107872, 0.34859275824313224},
                DispersionAllpass{-0.74135741788659504, 0.28898858026054791},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        },
        // String 187, MIDI 92, f1 = 1668.940000000000, B = 0.0066642651179, maxPartial = 6, loopDelaySamples = 17.472381693579, order = 4, sectionCount = 2
        DispersionPreset{
            92,
            1668.9400000000001,
            0.0066642651179016771,
            44100,
            17.472381693578892,
            4,
            2,
            {{
                DispersionAllpass{-1.1451721768868854, 0.34853236479446453},
                DispersionAllpass{-0.7409877039041175, 0.28890622171517283},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        },
        // String 188, MIDI 92, f1 = 1669.760000000000, B = 0.00666781930865, maxPartial = 6, loopDelaySamples = 17.461959509445, order = 4, sectionCount = 2
        DispersionPreset{
            92,
            1669.76,
            0.0066678193086544425,
            44100,
            17.461959509444657,
            4,
            2,
            {{
                DispersionAllpass{-1.145044116187907, 0.34847198837995941},
                DispersionAllpass{-0.74061794831246153, 0.28882388726201086},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        },
        // String 189, MIDI 93, f1 = 1767.810000000000, B = 0.00707852298843, maxPartial = 6, loopDelaySamples = 16.293496059503, order = 4, sectionCount = 2
        DispersionPreset{
            93,
            1767.8099999999999,
            0.0070785229884286109,
            44100,
            16.2934960595029,
            4,
            2,
            {{
                DispersionAllpass{-1.1294919906364032, 0.34118336183671738},
                DispersionAllpass{-0.69622930719757281, 0.27907359020156225},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        },
        // String 190, MIDI 93, f1 = 1768.680000000000, B = 0.00708203992601, maxPartial = 6, loopDelaySamples = 16.283775999634, order = 4, sectionCount = 2
        DispersionPreset{
            93,
            1768.6800000000001,
            0.0070820399260056689,
            44100,
            16.283775999633633,
            4,
            2,
            {{
                DispersionAllpass{-1.1293522191798679, 0.34111829861708315},
                DispersionAllpass{-0.6958341241403444, 0.27898805541474969},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        },
        // String 191, MIDI 93, f1 = 1769.550000000000, B = 0.0070855546207, maxPartial = 6, loopDelaySamples = 16.274066911990, order = 4, sectionCount = 2
        DispersionPreset{
            93,
            1769.55,
            0.0070855546206974353,
            44100,
            16.274066911989514,
            4,
            2,
            {{
                DispersionAllpass{-1.1292121419396663, 0.34105304798811298},
                DispersionAllpass{-0.69543900978526396, 0.27890245813015163},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        },
        // String 192, MIDI 94, f1 = 1874.600000000000, B = 0.00749347009861, maxPartial = 6, loopDelaySamples = 15.175438461666, order = 4, sectionCount = 2
        DispersionPreset{
            94,
            1874.5999999999999,
            0.0074934700986100814,
            44100,
            15.175438461665683,
            4,
            2,
            {{
                DispersionAllpass{-1.1121184450935044, 0.33315449304643202},
                DispersionAllpass{-0.64763250414005324, 0.26871715598445639},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        },
        // String 193, MIDI 94, f1 = 1875.520000000000, B = 0.00749689847767, maxPartial = 6, loopDelaySamples = 15.166422786888, order = 4, sectionCount = 2
        DispersionPreset{
            94,
            1875.52,
            0.0074968984776709983,
            44100,
            15.166422786888061,
            4,
            2,
            {{
                DispersionAllpass{-1.1119672214494896, 0.33308517074406235},
                DispersionAllpass{-0.64721334524839647, 0.26862932749612317},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        },
        // String 194, MIDI 94, f1 = 1876.450000000000, B = 0.00750036158852, maxPartial = 6, loopDelaySamples = 15.157319107060, order = 4, sectionCount = 2
        DispersionPreset{
            94,
            1876.45,
            0.007500361588523464,
            44100,
            15.157319107059795,
            4,
            2,
            {{
                DispersionAllpass{-0.64678961484116892, 0.26854058214070375},
                DispersionAllpass{-1.1118143686146906, 0.33301511905690856},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        },
        // String 195, MIDI 95, f1 = 1987.840000000000, B = 0.00789684398869, maxPartial = 6, loopDelaySamples = 14.135268631935, order = 4, sectionCount = 2
        DispersionPreset{
            95,
            1987.8399999999999,
            0.0078968439886875907,
            44100,
            14.135268631935297,
            4,
            2,
            {{
                DispersionAllpass{-1.0933733169010214, 0.32464284560180767},
                DispersionAllpass{-0.59605390539957226, 0.25811466123781013},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        },
        // String 196, MIDI 95, f1 = 1988.820000000000, B = 0.00790017222779, maxPartial = 6, loopDelaySamples = 14.126840526585, order = 4, sectionCount = 2
        DispersionPreset{
            95,
            1988.8199999999999,
            0.0079001722277890086,
            44100,
            14.12684052658523,
            4,
            2,
            {{
                DispersionAllpass{-1.0932101493823059, 0.32456949037173555},
                DispersionAllpass{-0.59560803145621977, 0.25802484059090408},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        },
        // String 197, MIDI 95, f1 = 1989.800000000000, B = 0.00790349769987, maxPartial = 6, loopDelaySamples = 14.118421565374, order = 4, sectionCount = 2
        DispersionPreset{
            95,
            1989.8,
            0.0079034976998747491,
            44100,
            14.118421565373868,
            4,
            2,
            {{
                DispersionAllpass{-1.0930470741412925, 0.32449621099244524},
                DispersionAllpass{-0.59516212452877792, 0.25793508233614332},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        },
        // String 198, MIDI 96, f1 = 2107.920000000000, B = 0.00828427510646, maxPartial = 6, loopDelaySamples = 13.166943420169, order = 4, sectionCount = 2
        DispersionPreset{
            96,
            2107.9200000000001,
            0.0082842751064562091,
            44100,
            13.166943420169478,
            4,
            2,
            {{
                DispersionAllpass{-1.0733395357715501, 0.31574024257227407},
                DispersionAllpass{-0.5415653018857145, 0.24738922962498008},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        },
        // String 199, MIDI 96, f1 = 2108.960000000000, B = 0.00828745325119, maxPartial = 6, loopDelaySamples = 13.159088209514, order = 4, sectionCount = 2
        DispersionPreset{
            96,
            2108.96,
            0.0082874532511910421,
            44100,
            13.159088209513939,
            4,
            2,
            {{
                DispersionAllpass{-1.0731658949820304, 0.31566401384616116},
                DispersionAllpass{-0.54109505434800442, 0.24729891385528222},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        },
        // String 200, MIDI 96, f1 = 2109.990000000000, B = 0.00829059787445, maxPartial = 6, loopDelaySamples = 13.151316857629, order = 4, sectionCount = 2
        DispersionPreset{
            96,
            2109.9899999999998,
            0.0082905978744504689,
            44100,
            13.151316857629102,
            4,
            2,
            {{
                DispersionAllpass{-1.0729940397151434, 0.31558860760991625},
                DispersionAllpass{-0.54062932327126578, 0.24720955398919514},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        },
        // String 201, MIDI 97, f1 = 2235.250000000000, B = 0.00865138110577, maxPartial = 4, loopDelaySamples = 15.218269273828, order = 2, sectionCount = 1
        DispersionPreset{
            97,
            2235.25,
            0.0086513811057734348,
            44100,
            15.218269273828362,
            2,
            1,
            {{
                DispersionAllpass{-0.89849267747932693, 0.2347147380029245},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        },
        // String 202, MIDI 97, f1 = 2236.350000000000, B = 0.00865436238514, maxPartial = 4, loopDelaySamples = 15.210022787641, order = 2, sectionCount = 1
        DispersionPreset{
            97,
            2236.3499999999999,
            0.0086543623851430551,
            44100,
            15.210022787641279,
            2,
            1,
            {{
                DispersionAllpass{-0.89822841052001856, 0.23460735681863279},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        },
        // String 203, MIDI 97, f1 = 2237.450000000000, B = 0.00865734046462, maxPartial = 4, loopDelaySamples = 15.201784970916, order = 2, sectionCount = 1
        DispersionPreset{
            97,
            2237.4499999999998,
            0.0086573404646191551,
            44100,
            15.20178497091603,
            2,
            1,
            {{
                DispersionAllpass{-0.89796414664719382, 0.23450000374488292},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        },
        // String 204, MIDI 98, f1 = 2370.280000000000, B = 0.00899389774103, maxPartial = 4, loopDelaySamples = 14.267005395944, order = 2, sectionCount = 1
        DispersionPreset{
            98,
            2370.2800000000002,
            0.0089938977410334853,
            44100,
            14.267005395943663,
            2,
            1,
            {{
                DispersionAllpass{-0.8660503250023327, 0.2217221883833527},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        },
        // String 205, MIDI 98, f1 = 2371.440000000000, B = 0.00899663956121, maxPartial = 4, loopDelaySamples = 14.259334338192, order = 2, sectionCount = 1
        DispersionPreset{
            98,
            2371.4400000000001,
            0.0089966395612102908,
            44100,
            14.259334338192204,
            2,
            1,
            {{
                DispersionAllpass{-0.86577176506881581, 0.22161228337809655},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        },
        // String 206, MIDI 98, f1 = 2372.610000000000, B = 0.00899940163094, maxPartial = 4, loopDelaySamples = 14.251605254178, order = 2, sectionCount = 1
        DispersionPreset{
            98,
            2372.6100000000001,
            0.0089994016309385607,
            44100,
            14.251605254178124,
            2,
            1,
            {{
                DispersionAllpass{-0.86549081551140628, 0.22150146551362598},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        },
        // String 207, MIDI 99, f1 = 2513.460000000000, B = 0.00930765183075, maxPartial = 4, loopDelaySamples = 13.377074501772, order = 2, sectionCount = 1
        DispersionPreset{
            99,
            2513.46,
            0.0093076518307531313,
            44100,
            13.377074501771681,
            2,
            1,
            {{
                DispersionAllpass{-0.83172921476835204, 0.20839190636963612},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        },
        // String 208, MIDI 99, f1 = 2514.700000000000, B = 0.00931015714097, maxPartial = 4, loopDelaySamples = 13.369838203390, order = 2, sectionCount = 1
        DispersionPreset{
            99,
            2514.6999999999998,
            0.0093101571409708352,
            44100,
            13.369838203389651,
            2,
            1,
            {{
                DispersionAllpass{-0.83143271186289736, 0.20827859444371227},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        },
        // String 209, MIDI 99, f1 = 2515.930000000000, B = 0.00931263874439, maxPartial = 4, loopDelaySamples = 13.362667758157, order = 2, sectionCount = 1
        DispersionPreset{
            99,
            2515.9299999999998,
            0.0093126387443926931,
            44100,
            13.362667758156912,
            2,
            1,
            {{
                DispersionAllpass{-0.83113861571398329, 0.20816623353713437},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        },
        // String 210, MIDI 100, f1 = 2665.290000000000, B = 0.00958877061951, maxPartial = 4, loopDelaySamples = 12.544083525655, order = 2, sectionCount = 1
        DispersionPreset{
            100,
            2665.29,
            0.009588770619505492,
            44100,
            12.544083525655282,
            2,
            1,
            {{
                DispersionAllpass{-0.79556623277743721, 0.1948047237153962},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        },
        // String 211, MIDI 100, f1 = 2666.600000000000, B = 0.00959097758226, maxPartial = 4, loopDelaySamples = 12.537333589690, order = 2, sectionCount = 1
        DispersionPreset{
            100,
            2666.5999999999999,
            0.0095909775822555152,
            44100,
            12.537333589690387,
            2,
            1,
            {{
                DispersionAllpass{-0.79525567245317597, 0.19469007108715528},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        },
        // String 212, MIDI 100, f1 = 2667.910000000000, B = 0.00959318091741, maxPartial = 4, loopDelaySamples = 12.530590669189, order = 2, sectionCount = 1
        DispersionPreset{
            100,
            2667.9099999999999,
            0.0095931809174093739,
            44100,
            12.530590669189163,
            2,
            1,
            {{
                DispersionAllpass{-0.79494514106945457, 0.19457546373141668},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        },
        // String 213, MIDI 101, f1 = 2826.290000000000, B = 0.00983368175536, maxPartial = 4, loopDelaySamples = 11.763926664566, order = 2, sectionCount = 1
        DispersionPreset{
            101,
            2826.29,
            0.009833681755363196,
            44100,
            11.763926664565513,
            2,
            1,
            {{
                DispersionAllpass{-0.75763918138237096, 0.18105829440841686},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        },
        // String 214, MIDI 101, f1 = 2827.680000000000, B = 0.0098355725817, maxPartial = 4, loopDelaySamples = 11.757598551854, order = 2, sectionCount = 1
        DispersionPreset{
            101,
            2827.6799999999998,
            0.0098355725817048413,
            44100,
            11.757598551853892,
            2,
            1,
            {{
                DispersionAllpass{-0.75731409265046845, 0.18094269497075063},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        },
        // String 215, MIDI 101, f1 = 2829.070000000000, B = 0.00983745970925, maxPartial = 4, loopDelaySamples = 11.751276998650, order = 2, sectionCount = 1
        DispersionPreset{
            101,
            2829.0700000000002,
            0.0098374597092516624,
            44100,
            11.751276998650265,
            2,
            1,
            {{
                DispersionAllpass{-0.75698903602959822, 0.18082714168069591},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        },
        // String 216, MIDI 102, f1 = 2997.020000000000, B = 0.0100392236293, maxPartial = 4, loopDelaySamples = 11.032669798351, order = 2, sectionCount = 1
        DispersionPreset{
            102,
            2997.02,
            0.010039223629270608,
            44100,
            11.032669798351105,
            2,
            1,
            {{
                DispersionAllpass{-0.71807041211274236, 0.16726593472998047},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        },
        // String 217, MIDI 102, f1 = 2998.490000000000, B = 0.0100407680851, maxPartial = 4, loopDelaySamples = 11.026751818976, order = 2, sectionCount = 1
        DispersionPreset{
            102,
            2998.4899999999998,
            0.010040768085133125,
            44100,
            11.026751818975672,
            2,
            1,
            {{
                DispersionAllpass{-0.71773314501465468, 0.1671507872745728},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        },
        // String 218, MIDI 102, f1 = 2999.970000000000, B = 0.0100423193092, maxPartial = 4, loopDelaySamples = 11.020799701877, order = 2, sectionCount = 1
        DispersionPreset{
            102,
            2999.9699999999998,
            0.010042319309200817,
            44100,
            11.020799701876925,
            2,
            1,
            {{
                DispersionAllpass{-0.71739364476203271, 0.16703491804677228},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        },
        // String 219, MIDI 103, f1 = 3178.060000000000, B = 0.0102026841908, maxPartial = 4, loopDelaySamples = 10.346640328607, order = 2, sectionCount = 1
        DispersionPreset{
            103,
            3178.0599999999999,
            0.010202684190796315,
            44100,
            10.346640328607414,
            2,
            1,
            {{
                DispersionAllpass{-0.677039923981128, 0.15355809905974699},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        },
        // String 220, MIDI 103, f1 = 3179.620000000000, B = 0.0102038678255, maxPartial = 4, loopDelaySamples = 10.341081018679, order = 2, sectionCount = 1
        DispersionPreset{
            103,
            3179.6199999999999,
            0.010203867825481026,
            44100,
            10.341081018679152,
            2,
            1,
            {{
                DispersionAllpass{-0.67669107427431507, 0.15344415346073489},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        },
        // String 221, MIDI 103, f1 = 3181.180000000000, B = 0.0102050477794, maxPartial = 4, loopDelaySamples = 10.335527330481, order = 2, sectionCount = 1
        DispersionPreset{
            103,
            3181.1799999999998,
            0.010205047779361105,
            44100,
            10.335527330481252,
            2,
            1,
            {{
                DispersionAllpass{-0.67634232576215414, 0.15333028927270548},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        },
        // String 222, MIDI 104, f1 = 3370.040000000000, B = 0.0103218931821, maxPartial = 4, loopDelaySamples = 9.702299862567, order = 2, sectionCount = 1
        DispersionPreset{
            104,
            3370.04,
            0.010321893182107819,
            44100,
            9.7022998625666936,
            2,
            1,
            {{
                DispersionAllpass{-0.63478640848671053, 0.14007884021721234},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        },
        // String 223, MIDI 104, f1 = 3371.690000000000, B = 0.0103226968934, maxPartial = 4, loopDelaySamples = 9.697088552418, order = 2, sectionCount = 1
        DispersionPreset{
            104,
            3371.6900000000001,
            0.010322696893368198,
            44100,
            9.6970885524179398,
            2,
            1,
            {{
                DispersionAllpass{-0.6344294649138148, 0.13996774950260937},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        },
        // String 224, MIDI 104, f1 = 3373.350000000000, B = 0.010323501854, maxPartial = 4, loopDelaySamples = 9.691850928877, order = 2, sectionCount = 1
        DispersionPreset{
            104,
            3373.3499999999999,
            0.010323501853965826,
            44100,
            9.6918509288766348,
            2,
            1,
            {{
                DispersionAllpass{-0.63407046449365867, 0.13985606413178733},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        },
        // String 225, MIDI 105, f1 = 3573.610000000000, B = 0.0103952430042, maxPartial = 4, loopDelaySamples = 9.096358331159, order = 2, sectionCount = 1
        DispersionPreset{
            105,
            3573.6100000000001,
            0.010395243004170114,
            44100,
            9.0963583311590366,
            2,
            1,
            {{
                DispersionAllpass{-0.59162043645767037, 0.12698568243357958},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        },
        // String 226, MIDI 105, f1 = 3575.370000000000, B = 0.0103956614332, maxPartial = 4, loopDelaySamples = 9.091424903928, order = 2, sectionCount = 1
        DispersionPreset{
            105,
            3575.3699999999999,
            0.010395661433177543,
            44100,
            9.0914249039276207,
            2,
            1,
            {{
                DispersionAllpass{-0.5912552071610524, 0.12687785659497428},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        },
        // String 227, MIDI 105, f1 = 3577.120000000000, B = 0.0103960740055, maxPartial = 4, loopDelaySamples = 9.086524360295, order = 2, sectionCount = 1
        DispersionPreset{
            105,
            3577.1199999999999,
            0.010396074005476357,
            44100,
            9.0865243602952557,
            2,
            1,
            {{
                DispersionAllpass{-0.59089220557574362, 0.12677074166979024},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        },
        // String 228, MIDI 106, f1 = 3789.480000000000, B = 0.010421746552, maxPartial = 4, loopDelaySamples = 8.525646759380, order = 2, sectionCount = 1
        DispersionPreset{
            106,
            3789.48,
            0.01042174655198037,
            44100,
            8.5256467593796579,
            2,
            1,
            {{
                DispersionAllpass{-0.54791901927043363, 0.11444295540469333},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        },
        // String 229, MIDI 106, f1 = 3791.340000000000, B = 0.010421769215, maxPartial = 4, loopDelaySamples = 8.521012119379, order = 2, sectionCount = 1
        DispersionPreset{
            106,
            3791.3400000000001,
            0.010421769215013135,
            44100,
            8.521012119378657,
            2,
            1,
            {{
                DispersionAllpass{-0.54755235772935029, 0.11434081443623016},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        },
        // String 230, MIDI 106, f1 = 3793.210000000000, B = 0.0104217886563, maxPartial = 4, loopDelaySamples = 8.516357131283, order = 2, sectionCount = 1
        DispersionPreset{
            106,
            3793.21,
            0.010421788656279837,
            44100,
            8.51635713128295,
            2,
            1,
            {{
                DispersionAllpass{-0.54718389520953414, 0.11423822265916461},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        },
        // String 231, MIDI 107, f1 = 4018.390000000000, B = 0.0104010428546, maxPartial = 4, loopDelaySamples = 7.987222569782, order = 2, sectionCount = 1
        DispersionPreset{
            107,
            4018.3899999999999,
            0.010401042854577084,
            44100,
            7.9872225697820873,
            2,
            1,
            {{
                DispersionAllpass{-0.50413089435553027, 0.10261858464807327},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        },
        // String 232, MIDI 107, f1 = 4020.370000000000, B = 0.0104006694063, maxPartial = 4, loopDelaySamples = 7.982829198253, order = 2, sectionCount = 1
        DispersionPreset{
            107,
            4020.3699999999999,
            0.010400669406307886,
            44100,
            7.9828291982533113,
            2,
            1,
            {{
                DispersionAllpass{-0.50376416278896152, 0.10252278568108801},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        },
        // String 233, MIDI 107, f1 = 4022.340000000000, B = 0.0104002947405, maxPartial = 4, loopDelaySamples = 7.978462227781, order = 2, sectionCount = 1
        DispersionPreset{
            107,
            4022.3400000000001,
            0.010400294740517305,
            44100,
            7.9784622277807653,
            2,
            1,
            {{
                DispersionAllpass{-0.50339948847202676, 0.10242757686502209},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        },
        // String 234, MIDI 108, f1 = 4261.130000000000, B = 0.010333412023, maxPartial = 4, loopDelaySamples = 7.478323701284, order = 2, sectionCount = 1
        DispersionPreset{
            108,
            4261.1300000000001,
            0.010333412023005079,
            44100,
            7.4783237012839869,
            2,
            1,
            {{
                DispersionAllpass{-0.4607642964250368, 0.091676221200883409},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        },
        // String 235, MIDI 108, f1 = 4263.230000000000, B = 0.0103326472992, maxPartial = 4, loopDelaySamples = 7.474166463998, order = 2, sectionCount = 1
        DispersionPreset{
            108,
            4263.2299999999996,
            0.010332647299167762,
            44100,
            7.4741664639979373,
            2,
            1,
            {{
                DispersionAllpass{-0.46040333819359647, 0.091588483249431668},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        },
        // String 236, MIDI 108, f1 = 4265.320000000000, B = 0.0103318833666, maxPartial = 4, loopDelaySamples = 7.470032942760, order = 2, sectionCount = 1
        DispersionPreset{
            108,
            4265.3199999999997,
            0.010331883366577739,
            44100,
            7.4700329427597456,
            2,
            1,
            {{
                DispersionAllpass{-0.46004434404267763, 0.091501279073683925},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f},
                DispersionAllpass{0.0f, 0.0f}
            }}
        }
    }};
};

#endif /* dispersion_filter_hpp */
