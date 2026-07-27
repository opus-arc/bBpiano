import SwiftUI

struct ContentView: View {
    var body: some View {
        SynthPanelView()
    }
}

// MARK: - Hardware Theme

enum SynthTheme {
    static let panelBlack = Color(red: 0.055, green: 0.058, blue: 0.055)
    static let panelRaised = Color(red: 0.105, green: 0.108, blue: 0.098)
    static let panelEdge = Color(red: 0.28, green: 0.27, blue: 0.23)
    static let cream = Color(red: 0.90, green: 0.85, blue: 0.69)
    static let mutedCream = Color(red: 0.62, green: 0.60, blue: 0.50)
    static let amber = Color(red: 0.93, green: 0.49, blue: 0.15)
    static let red = Color(red: 0.70, green: 0.12, blue: 0.08)
    static let teal = Color(red: 0.18, green: 0.52, blue: 0.48)
    static let walnut = Color(red: 0.23, green: 0.095, blue: 0.035)

    static let panelGradient = LinearGradient(
        colors: [
            Color(red: 0.12, green: 0.12, blue: 0.108),
            panelBlack,
            Color(red: 0.085, green: 0.088, blue: 0.082),
        ],
        startPoint: .topLeading,
        endPoint: .bottomTrailing
    )

    static let cabinetGradient = LinearGradient(
        colors: [
            Color(red: 0.34, green: 0.15, blue: 0.055),
            walnut,
            Color(red: 0.12, green: 0.04, blue: 0.015),
        ],
        startPoint: .top,
        endPoint: .bottom
    )
}

private struct EngravedLabel: ViewModifier {
    let size: CGFloat
    let color: Color

    func body(content: Content) -> some View {
        content
            .font(.system(size: size, weight: .bold, design: .rounded))
            .tracking(size * 0.12)
            .foregroundStyle(color)
    }
}

private extension View {
    func engraved(size: CGFloat = 10, color: Color = SynthTheme.cream) -> some View {
        modifier(EngravedLabel(size: size, color: color))
    }
}

// MARK: - Reusable Parameter Controls

struct ParameterKnob: View {
    @ObservedObject var parameter: SynthParameter
    var size: CGFloat = 82
    var accent: Color = SynthTheme.amber

    @State private var dragOrigin: Double?

    var body: some View {
        VStack(spacing: 5) {
            Text(parameter.name)
                .engraved(size: 9)
                .lineLimit(1)

            ZStack {
                tickRing
                knobBody
            }
            .frame(width: size + 20, height: size + 20)
            .contentShape(Rectangle())
            .gesture(dragGesture)
            .onTapGesture(count: 2) { parameter.resetToDefault() }
            .accessibilityElement(children: .ignore)
            .accessibilityLabel(parameter.name)
            .accessibilityValue(parameter.displayValue)
            .accessibilityAdjustableAction { direction in
                let delta = direction == .increment ? 0.01 : -0.01
                parameter.setNormalizedValue(parameter.normalizedValue + delta)
            }

            Text(parameter.displayValue)
                .font(.system(size: 11, weight: .semibold, design: .monospaced))
                .foregroundStyle(accent)
                .frame(minWidth: size + 14)

            if let cc = parameter.midiCC {
                Text("CC \(cc)")
                    .font(.system(size: 8, weight: .medium, design: .monospaced))
                    .foregroundStyle(SynthTheme.mutedCream.opacity(0.7))
            }
        }
        .frame(minWidth: size + 28)
    }

    private var tickRing: some View {
        ZStack {
            ForEach(0..<11, id: \.self) { index in
                Capsule()
                    .fill(index == 5 ? accent : SynthTheme.mutedCream.opacity(0.75))
                    .frame(width: index % 5 == 0 ? 2.2 : 1.2, height: index % 5 == 0 ? 8 : 5)
                    .offset(y: -(size * 0.53))
                    .rotationEffect(.degrees(-135 + Double(index) * 27))
            }
        }
        .frame(width: size + 18, height: size + 18)
    }

    private var knobBody: some View {
        let angle = -135 + parameter.normalizedValue * 270
        return ZStack {
            Circle()
                .fill(Color.black.opacity(0.8))
                .frame(width: size + 5, height: size + 5)
                .shadow(color: .black.opacity(0.8), radius: 5, x: 2, y: 4)
            Circle()
                .fill(
                    AngularGradient(
                        colors: [
                            Color(white: 0.10), Color(white: 0.28), Color(white: 0.08),
                            Color(white: 0.20), Color(white: 0.10),
                        ],
                        center: .center
                    )
                )
                .overlay { Circle().strokeBorder(Color.white.opacity(0.13), lineWidth: 1) }
                .frame(width: size, height: size)
            Circle()
                .fill(
                    RadialGradient(
                        colors: [Color.white.opacity(0.08), .clear],
                        center: .topLeading,
                        startRadius: 2,
                        endRadius: size * 0.65
                    )
                )
                .frame(width: size - 8, height: size - 8)
            Capsule()
                .fill(accent)
                .frame(width: 3, height: size * 0.31)
                .offset(y: -size * 0.27)
                .rotationEffect(.degrees(angle))
                .shadow(color: accent.opacity(0.35), radius: 2)
        }
    }

    private var dragGesture: some Gesture {
        DragGesture(minimumDistance: 0)
            .onChanged { value in
                if dragOrigin == nil { dragOrigin = parameter.normalizedValue }
                let travel = Double(value.translation.width - value.translation.height)
                parameter.setNormalizedValue((dragOrigin ?? 0) + travel / 240)
            }
            .onEnded { _ in dragOrigin = nil }
    }
}

struct ParameterSelector: View {
    @ObservedObject var parameter: SynthParameter
    var accent: Color = SynthTheme.amber
    var compact = false

    private var options: [String] {
        if case .discrete(let options) = parameter.type { return options }
        return []
    }

    var body: some View {
        VStack(alignment: .leading, spacing: 7) {
            HStack {
                Text(parameter.name).engraved(size: 9)
                Spacer()
                if let cc = parameter.midiCC {
                    Text("CC \(cc)")
                        .font(.system(size: 8, weight: .medium, design: .monospaced))
                        .foregroundStyle(SynthTheme.mutedCream.opacity(0.7))
                }
            }

            HStack(spacing: 3) {
                ForEach(Array(options.enumerated()), id: \.offset) { index, option in
                    Button {
                        parameter.setValue(Double(index))
                    } label: {
                        Text(option.uppercased())
                            .font(.system(size: compact ? 8 : 9, weight: .bold, design: .rounded))
                            .lineLimit(1)
                            .minimumScaleFactor(0.7)
                            .foregroundStyle(index == selectedIndex ? Color.black : SynthTheme.mutedCream)
                            .frame(maxWidth: .infinity)
                            .padding(.vertical, compact ? 5 : 7)
                            .background {
                                RoundedRectangle(cornerRadius: 2)
                                    .fill(index == selectedIndex ? accent : Color.black.opacity(0.42))
                            }
                            .overlay {
                                RoundedRectangle(cornerRadius: 2)
                                    .stroke(
                                        index == selectedIndex ? accent.opacity(0.9) : SynthTheme.panelEdge,
                                        lineWidth: 1
                                    )
                            }
                    }
                    .buttonStyle(.plain)
                }
            }

            Text(parameter.displayValue.uppercased())
                .font(.system(size: 9, weight: .semibold, design: .monospaced))
                .foregroundStyle(accent)
        }
    }

    private var selectedIndex: Int { Int(parameter.currentValue.rounded()) }
}

struct ParameterToggle: View {
    @ObservedObject var parameter: SynthParameter
    var accent: Color = SynthTheme.red

    private var isOn: Bool { parameter.currentValue >= 0.5 }

    var body: some View {
        Button {
            parameter.setValue(isOn ? 0 : 1)
        } label: {
            VStack(spacing: 7) {
                HStack(spacing: 5) {
                    Circle()
                        .fill(isOn ? accent : Color.black)
                        .frame(width: 8, height: 8)
                        .shadow(color: isOn ? accent.opacity(0.8) : .clear, radius: 4)
                    Text(parameter.name).engraved(size: 9)
                }
                ZStack {
                    RoundedRectangle(cornerRadius: 3)
                        .fill(Color.black.opacity(0.75))
                        .frame(width: 34, height: 52)
                        .overlay {
                            RoundedRectangle(cornerRadius: 3)
                                .stroke(SynthTheme.panelEdge, lineWidth: 1)
                        }
                    RoundedRectangle(cornerRadius: 2)
                        .fill(
                            LinearGradient(
                                colors: [Color(white: 0.36), Color(white: 0.10)],
                                startPoint: .top,
                                endPoint: .bottom
                            )
                        )
                        .frame(width: 25, height: 24)
                        .offset(y: isOn ? -10 : 10)
                        .shadow(color: .black, radius: 2, y: 2)
                }
                Text(parameter.displayValue)
                    .font(.system(size: 9, weight: .bold, design: .monospaced))
                    .foregroundStyle(isOn ? accent : SynthTheme.mutedCream)
                if let cc = parameter.midiCC {
                    Text("CC \(cc)")
                        .font(.system(size: 8, design: .monospaced))
                        .foregroundStyle(SynthTheme.mutedCream.opacity(0.65))
                }
            }
        }
        .buttonStyle(.plain)
        .accessibilityLabel(parameter.name)
        .accessibilityValue(parameter.displayValue)
    }
}

struct ModulePanel<Content: View>: View {
    let title: String
    let subtitle: String
    let accent: Color
    let content: Content

    init(
        title: String,
        subtitle: String = "",
        accent: Color = SynthTheme.amber,
        @ViewBuilder content: () -> Content
    ) {
        self.title = title
        self.subtitle = subtitle
        self.accent = accent
        self.content = content()
    }

    var body: some View {
        VStack(alignment: .leading, spacing: 13) {
            HStack(alignment: .firstTextBaseline, spacing: 12) {
                Text(title)
                    .font(.system(size: 17, weight: .black, design: .rounded))
                    .tracking(2.4)
                    .foregroundStyle(SynthTheme.cream)
                if !subtitle.isEmpty {
                    Text(subtitle)
                        .font(.system(size: 9, weight: .medium, design: .monospaced))
                        .foregroundStyle(SynthTheme.mutedCream)
                }
                Spacer()
            }
            .padding(.bottom, 7)
            .overlay(alignment: .bottom) {
                Rectangle().fill(accent).frame(height: 2)
            }
            content
        }
        .padding(15)
        .background {
            RoundedRectangle(cornerRadius: 4)
                .fill(SynthTheme.panelGradient)
                .overlay {
                    RoundedRectangle(cornerRadius: 4)
                        .stroke(SynthTheme.panelEdge, lineWidth: 1)
                }
                .shadow(color: .black.opacity(0.75), radius: 6, y: 4)
        }
    }
}

private struct PanelScrew: View {
    var body: some View {
        Circle()
            .fill(
                LinearGradient(
                    colors: [Color(white: 0.55), Color(white: 0.14)],
                    startPoint: .topLeading,
                    endPoint: .bottomTrailing
                )
            )
            .frame(width: 9, height: 9)
            .overlay {
                Rectangle()
                    .fill(Color.black.opacity(0.65))
                    .frame(width: 6, height: 1)
                    .rotationEffect(.degrees(-22))
            }
    }
}

// MARK: - Instrument Panel

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

                HStack {
                    Text("MIDI CONTROL SURFACE  •  NO AUDIO ENGINE")
                    Spacer()
                    Text("MIDI CHANNEL 1")
                }
                .engraved(size: 8, color: SynthTheme.mutedCream.opacity(0.78))
                .padding(.horizontal, 5)
                .frame(width: 1_420)
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
            Button("RESET PATCH") { controller.resetPatch() }
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
                Rectangle().fill(SynthTheme.panelEdge).frame(width: 1, height: 225)
                oscillator(
                    title: "OSCILLATOR 2",
                    waveform: .osc2Waveform,
                    tune: .osc2Tune,
                    fine: .osc2FineTune,
                    level: .osc2Level
                )
                Rectangle().fill(SynthTheme.panelEdge).frame(width: 1, height: 225)
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
            Text(title).engraved(size: 11, color: SynthTheme.amber)
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
        ModulePanel(title: "MIXER", subtitle: "SOURCE LEVELS", accent: SynthTheme.red) {
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
        ModulePanel(title: "LFO", subtitle: "MODULATION", accent: SynthTheme.amber) {
            HStack(spacing: 4) {
                ParameterKnob(parameter: parameters.parameter(.lfoRate), size: 69)
                ParameterKnob(parameter: parameters.parameter(.lfoAmount), size: 69)
            }
            .frame(maxWidth: .infinity)
            ParameterSelector(parameter: parameters.parameter(.lfoWaveform), compact: true)
            ParameterSelector(parameter: parameters.parameter(.lfoDestination), compact: true)
        }
    }
}

private struct VoiceModule: View {
    let parameters: ParameterStore

    var body: some View {
        ModulePanel(title: "VOICE", subtitle: "ASSIGNMENT", accent: SynthTheme.red) {
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
            .engraved(
                size: 9,
                color: configuration.isPressed ? SynthTheme.amber : SynthTheme.cream
            )
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
