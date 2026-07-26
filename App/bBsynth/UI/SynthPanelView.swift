import SwiftUI

struct SynthPanelView: View {
    @EnvironmentObject private var controller: SynthController

    var body: some View {
        ScrollView([.horizontal, .vertical]) {
            VStack(spacing: 14) {
                instrumentHeader

                HStack(alignment: .top, spacing: 14) {
                    OscillatorModule(parameters: controller.parameters)
                        .frame(width: 940)
                    MixerModule(parameters: controller.parameters)
                        .frame(width: 350)
                }

                HStack(alignment: .top, spacing: 14) {
                    FilterModule(parameters: controller.parameters)
                        .frame(width: 650)
                    LFOModule(parameters: controller.parameters)
                        .frame(width: 350)
                    VoiceModule(parameters: controller.parameters)
                        .frame(width: 405)
                }

                HStack(alignment: .top, spacing: 14) {
                    EnvelopeModule(
                        title: "AMPLIFIER ENVELOPE",
                        subtitle: "VCA  •  ADSR",
                        accent: SynthTheme.red,
                        parameters: controller.parameters,
                        ids: [.ampAttack, .ampDecay, .ampSustain, .ampRelease]
                    )
                    EnvelopeModule(
                        title: "FILTER ENVELOPE",
                        subtitle: "VCF  •  ADSR",
                        accent: SynthTheme.teal,
                        parameters: controller.parameters,
                        ids: [.filterAttack, .filterDecay, .filterSustain, .filterRelease]
                    )
                }
                .frame(width: 1_420)

                footer
            }
            .padding(.horizontal, 22)
            .padding(.vertical, 16)
            .frame(minWidth: 1_465, alignment: .top)
        }
        .background {
            ZStack {
                SynthTheme.cabinetGradient
                Canvas { context, size in
                    for index in 0..<18 {
                        let y = CGFloat(index) * size.height / 18
                        var path = Path()
                        path.move(to: CGPoint(x: 0, y: y))
                        path.addCurve(
                            to: CGPoint(x: size.width, y: y + 3),
                            control1: CGPoint(x: size.width * 0.32, y: y - 5),
                            control2: CGPoint(x: size.width * 0.68, y: y + 7)
                        )
                        context.stroke(path, with: .color(Color.black.opacity(0.08)), lineWidth: 1)
                    }
                }
            }
            .ignoresSafeArea()
        }
        .preferredColorScheme(.dark)
    }

    private var instrumentHeader: some View {
        HStack(spacing: 20) {
            PanelScrew()

            VStack(alignment: .leading, spacing: 1) {
                HStack(alignment: .firstTextBaseline, spacing: 9) {
                    Text("bB")
                        .font(.system(size: 28, weight: .black, design: .rounded))
                        .foregroundStyle(SynthTheme.amber)
                    Text("SYNTH")
                        .font(.system(size: 27, weight: .light, design: .rounded))
                        .tracking(6)
                        .foregroundStyle(SynthTheme.cream)
                }
                Text("POLYPHONIC ANALOG CONTROL INSTRUMENT")
                    .engraved(size: 8, color: SynthTheme.mutedCream)
            }

            Spacer()

            MIDIStatusDisplay(manager: controller.midiManager)

            Button("RESET PATCH") {
                controller.resetPatch()
            }
            .buttonStyle(HardwareButtonStyle())

            PanelScrew()
        }
        .padding(.horizontal, 18)
        .padding(.vertical, 12)
        .frame(width: 1_420)
        .background {
            RoundedRectangle(cornerRadius: 3)
                .fill(SynthTheme.panelGradient)
                .overlay {
                    RoundedRectangle(cornerRadius: 3)
                        .stroke(SynthTheme.panelEdge, lineWidth: 1)
                }
        }
    }

    private var footer: some View {
        HStack {
            Text("MIDI CONTROL SURFACE  •  NO AUDIO ENGINE")
            Spacer()
            Text("MIDI CHANNEL 1")
        }
        .engraved(size: 8, color: SynthTheme.mutedCream.opacity(0.78))
        .padding(.horizontal, 5)
        .frame(width: 1_420)
    }
}

private struct OscillatorModule: View {
    let parameters: ParameterStore

    var body: some View {
        ModulePanel(
            title: "OSCILLATORS",
            subtitle: "DUAL VOLTAGE CONTROLLED OSCILLATOR",
            accent: SynthTheme.amber
        ) {
            HStack(alignment: .top, spacing: 18) {
                oscillator(
                    title: "OSCILLATOR 1",
                    waveform: .osc1Waveform,
                    tune: .osc1Tune,
                    fine: .osc1FineTune,
                    level: .osc1Level
                )

                Rectangle()
                    .fill(SynthTheme.panelEdge)
                    .frame(width: 1, height: 225)

                oscillator(
                    title: "OSCILLATOR 2",
                    waveform: .osc2Waveform,
                    tune: .osc2Tune,
                    fine: .osc2FineTune,
                    level: .osc2Level
                )

                Rectangle()
                    .fill(SynthTheme.panelEdge)
                    .frame(width: 1, height: 225)

                VStack(spacing: 17) {
                    ParameterToggle(parameter: parameters.parameter(.oscillatorSync))
                    ParameterKnob(
                        parameter: parameters.parameter(.pulseWidth),
                        size: 67,
                        accent: SynthTheme.amber
                    )
                }
                .frame(maxHeight: .infinity)
            }
        }
    }

    private func oscillator(
        title: String,
        waveform: ParameterID,
        tune: ParameterID,
        fine: ParameterID,
        level: ParameterID
    ) -> some View {
        VStack(alignment: .leading, spacing: 13) {
            Text(title)
                .engraved(size: 11, color: SynthTheme.amber)
            ParameterSelector(
                parameter: parameters.parameter(waveform),
                accent: SynthTheme.amber,
                compact: true
            )
            HStack(spacing: 5) {
                ParameterKnob(parameter: parameters.parameter(tune), size: 62)
                ParameterKnob(parameter: parameters.parameter(fine), size: 62)
                ParameterKnob(parameter: parameters.parameter(level), size: 62)
            }
        }
        .frame(width: 342)
    }
}

private struct MixerModule: View {
    let parameters: ParameterStore

    var body: some View {
        ModulePanel(
            title: "MIXER",
            subtitle: "SOURCE LEVELS",
            accent: SynthTheme.red
        ) {
            HStack(spacing: 3) {
                ParameterKnob(
                    parameter: parameters.parameter(.mixerOsc1Level),
                    size: 66,
                    accent: SynthTheme.red
                )
                ParameterKnob(
                    parameter: parameters.parameter(.mixerOsc2Level),
                    size: 66,
                    accent: SynthTheme.red
                )
                ParameterKnob(
                    parameter: parameters.parameter(.mixerNoiseLevel),
                    size: 66,
                    accent: SynthTheme.red
                )
            }
            .frame(maxWidth: .infinity, maxHeight: .infinity)
        }
    }
}

private struct FilterModule: View {
    let parameters: ParameterStore

    var body: some View {
        ModulePanel(
            title: "FILTER",
            subtitle: "STATE VARIABLE  •  20 Hz — 20 kHz",
            accent: SynthTheme.teal
        ) {
            ParameterSelector(
                parameter: parameters.parameter(.filterType),
                accent: SynthTheme.teal
            )

            HStack(spacing: 5) {
                ParameterKnob(
                    parameter: parameters.parameter(.filterCutoff),
                    size: 90,
                    accent: SynthTheme.teal
                )
                ParameterKnob(
                    parameter: parameters.parameter(.filterResonance),
                    size: 72,
                    accent: SynthTheme.teal
                )
                ParameterKnob(
                    parameter: parameters.parameter(.filterDrive),
                    size: 72,
                    accent: SynthTheme.teal
                )
                ParameterKnob(
                    parameter: parameters.parameter(.filterKeyboardTracking),
                    size: 72,
                    accent: SynthTheme.teal
                )
                ParameterKnob(
                    parameter: parameters.parameter(.filterEnvelopeAmount),
                    size: 72,
                    accent: SynthTheme.teal
                )
            }
            .frame(maxWidth: .infinity)
        }
    }
}

private struct LFOModule: View {
    let parameters: ParameterStore

    var body: some View {
        ModulePanel(
            title: "LFO",
            subtitle: "MODULATION",
            accent: SynthTheme.amber
        ) {
            HStack(spacing: 4) {
                ParameterKnob(parameter: parameters.parameter(.lfoRate), size: 69)
                ParameterKnob(parameter: parameters.parameter(.lfoAmount), size: 69)
            }
            .frame(maxWidth: .infinity)

            ParameterSelector(
                parameter: parameters.parameter(.lfoWaveform),
                compact: true
            )
            ParameterSelector(
                parameter: parameters.parameter(.lfoDestination),
                compact: true
            )
        }
    }
}

private struct VoiceModule: View {
    let parameters: ParameterStore

    var body: some View {
        ModulePanel(
            title: "VOICE",
            subtitle: "ASSIGNMENT",
            accent: SynthTheme.red
        ) {
            ParameterSelector(
                parameter: parameters.parameter(.voicePolyphony),
                accent: SynthTheme.red,
                compact: true
            )
            ParameterSelector(
                parameter: parameters.parameter(.voiceUnison),
                accent: SynthTheme.red,
                compact: true
            )

            HStack(spacing: 14) {
                ParameterKnob(
                    parameter: parameters.parameter(.voiceDetune),
                    size: 70,
                    accent: SynthTheme.red
                )
                ParameterKnob(
                    parameter: parameters.parameter(.voiceStereoSpread),
                    size: 70,
                    accent: SynthTheme.red
                )
            }
            .frame(maxWidth: .infinity)
        }
    }
}

private struct EnvelopeModule: View {
    let title: String
    let subtitle: String
    let accent: Color
    let parameters: ParameterStore
    let ids: [ParameterID]

    var body: some View {
        ModulePanel(title: title, subtitle: subtitle, accent: accent) {
            HStack(spacing: 15) {
                ForEach(ids) { id in
                    ParameterKnob(
                        parameter: parameters.parameter(id),
                        size: 84,
                        accent: accent
                    )
                }
            }
            .frame(maxWidth: .infinity)
        }
    }
}

private struct MIDIStatusDisplay: View {
    @ObservedObject var manager: MIDIManager

    private var indicatorColor: Color {
        manager.state == .online ? Color.green : SynthTheme.red
    }

    var body: some View {
        HStack(spacing: 10) {
            Circle()
                .fill(indicatorColor)
                .frame(width: 8, height: 8)
                .shadow(color: indicatorColor.opacity(0.8), radius: 4)

            VStack(alignment: .leading, spacing: 2) {
                Text(manager.state.label)
                    .font(.system(size: 9, weight: .bold, design: .monospaced))
                    .foregroundStyle(SynthTheme.cream)
                Text(manager.lastMessage)
                    .font(.system(size: 8, weight: .medium, design: .monospaced))
                    .foregroundStyle(SynthTheme.amber)
                    .frame(width: 160, alignment: .leading)
                    .lineLimit(1)
            }

            Text(MIDIManager.virtualOutputName)
                .font(.system(size: 9, weight: .bold, design: .monospaced))
                .foregroundStyle(SynthTheme.mutedCream)
        }
        .padding(.horizontal, 12)
        .padding(.vertical, 8)
        .background(Color.black.opacity(0.45))
        .overlay {
            RoundedRectangle(cornerRadius: 2)
                .stroke(SynthTheme.panelEdge, lineWidth: 1)
        }
    }
}

private struct HardwareButtonStyle: ButtonStyle {
    func makeBody(configuration: Configuration) -> some View {
        configuration.label
            .engraved(size: 9, color: configuration.isPressed ? SynthTheme.amber : SynthTheme.cream)
            .padding(.horizontal, 14)
            .padding(.vertical, 10)
            .background {
                RoundedRectangle(cornerRadius: 2)
                    .fill(configuration.isPressed ? Color.black : SynthTheme.panelRaised)
            }
            .overlay {
                RoundedRectangle(cornerRadius: 2)
                    .stroke(SynthTheme.panelEdge, lineWidth: 1)
            }
    }
}
