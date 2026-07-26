import SwiftUI

struct ParameterSelector: View {
    @ObservedObject var parameter: SynthParameter
    var accent: Color = SynthTheme.amber
    var compact = false

    private var options: [String] {
        if case .discrete(let options) = parameter.type {
            return options
        }
        return []
    }

    var body: some View {
        VStack(alignment: .leading, spacing: 7) {
            HStack {
                Text(parameter.name)
                    .engraved(size: 9)
                Spacer()
                ccLabel
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
        .accessibilityElement(children: .contain)
    }

    private var selectedIndex: Int {
        Int(parameter.currentValue.rounded())
    }

    @ViewBuilder
    private var ccLabel: some View {
        if let cc = parameter.midiCC {
            Text("CC \(cc)")
                .font(.system(size: 8, weight: .medium, design: .monospaced))
                .foregroundStyle(SynthTheme.mutedCream.opacity(0.7))
        }
    }
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
                    Text(parameter.name)
                        .engraved(size: 9)
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
    var subtitle: String = ""
    var accent: Color = SynthTheme.amber
    @ViewBuilder let content: Content

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
                Rectangle()
                    .fill(accent)
                    .frame(height: 2)
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

struct PanelScrew: View {
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
