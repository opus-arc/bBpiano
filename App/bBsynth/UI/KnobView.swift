import SwiftUI

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
            .onTapGesture(count: 2) {
                parameter.resetToDefault()
            }
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
                            Color(white: 0.10),
                            Color(white: 0.28),
                            Color(white: 0.08),
                            Color(white: 0.20),
                            Color(white: 0.10),
                        ],
                        center: .center
                    )
                )
                .overlay {
                    Circle()
                        .strokeBorder(Color.white.opacity(0.13), lineWidth: 1)
                }
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
                if dragOrigin == nil {
                    dragOrigin = parameter.normalizedValue
                }
                let travel = Double(value.translation.width - value.translation.height)
                parameter.setNormalizedValue((dragOrigin ?? 0) + travel / 240)
            }
            .onEnded { _ in
                dragOrigin = nil
            }
    }
}
