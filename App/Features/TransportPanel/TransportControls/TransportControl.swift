//
//  TransportControl.swift
//  bBpiano
//
//  Created by opus arc on 2026/5/25.
//

#if DEBUG
import Inject
#endif


import SwiftUI

struct TransportControl: View {
    
    #if DEBUG
    @ObserveInjection var inject
    #endif
    
    @State private var volume: Double = 0.62
    @State private var progress: Double = 0.11

    var body: some View {
        VStack {
            Spacer()

            LiquidGlassTransportBar(
                volume: $volume,
                progress: $progress,
                elapsedText: "1:59",
                remainingText: "-21:58"
            )
            .frame(width: 646, height: 84)
            .padding(.bottom, 22)
        }
        .frame(maxWidth: .infinity, maxHeight: .infinity)
        .background(PreviewBackdrop())
        
        #if DEBUG
        .enableInjection()
        #endif
    }
}

private struct LiquidGlassTransportBar: View {
    @Binding var volume: Double
    @Binding var progress: Double

    let elapsedText: String
    let remainingText: String

    var body: some View {
        ZStack {
            RoundedRectangle(cornerRadius: 18, style: .continuous)
                .fill(.regularMaterial)
                .overlay {
                    RoundedRectangle(cornerRadius: 18, style: .continuous)
                        .fill(
                            LinearGradient(
                                colors: [
                                    .white.opacity(0.46),
                                    .white.opacity(0.08),
                                    .black.opacity(0.12)
                                ],
                                startPoint: .topLeading,
                                endPoint: .bottomTrailing
                            )
                        )
                }
                .overlay {
                    RoundedRectangle(cornerRadius: 18, style: .continuous)
                        .strokeBorder(.white.opacity(0.30), lineWidth: 1)
                }
                .shadow(color: .black.opacity(0.24), radius: 18, x: 0, y: 10)
                .shadow(color: .white.opacity(0.24), radius: 1, x: 0, y: 1)

            VStack(spacing: 12) {
                HStack(alignment: .center, spacing: 30) {
                    GlassSlider(value: $volume, knobDiameter: 22, trackHeight: 6)
                        .frame(width: 86)

                    Spacer(minLength: 6)

                    HStack(spacing: 27) {
                        TransportIcon(systemName: "backward.end.fill", size: 16)
                        TransportIcon(systemName: "gobackward", size: 25)
                        TransportIcon(systemName: "play.fill", size: 32)
                        TransportIcon(systemName: "goforward", size: 25)
                        TransportIcon(systemName: "forward.end.fill", size: 16)
                    }

                    Spacer(minLength: 6)

                    HStack(spacing: 18) {
                        TransportIcon(systemName: "gearshape.fill", size: 18)
                        TransportIcon(systemName: "arrow.up.left.and.arrow.down.right", size: 16)
                        TransportIcon(systemName: "list.bullet.rectangle", size: 18)
                    }
                }
                .frame(height: 31)

                HStack(spacing: 16) {
                    Text(elapsedText)
                        .font(.system(size: 14, weight: .semibold, design: .rounded))
                        .monospacedDigit()
                        .foregroundStyle(.white.opacity(0.94))
                        .frame(width: 46, alignment: .leading)

                    GlassSlider(value: $progress, knobDiameter: 24, trackHeight: 7)

                    Text(remainingText)
                        .font(.system(size: 14, weight: .semibold, design: .rounded))
                        .monospacedDigit()
                        .foregroundStyle(.white.opacity(0.94))
                        .frame(width: 56, alignment: .trailing)
                }
                .frame(height: 25)
            }
            .padding(.horizontal, 18)
            .padding(.vertical, 12)
        }
        .compositingGroup()
    }
}

private struct GlassSlider: View {
    @Binding var value: Double

    let knobDiameter: CGFloat
    let trackHeight: CGFloat

    var body: some View {
        GeometryReader { proxy in
            let width = proxy.size.width
            let clampedValue = min(max(value, 0), 1)
            let knobX = clampedValue * width

            ZStack(alignment: .leading) {
                Capsule(style: .continuous)
                    .fill(.white.opacity(0.15))
                    .frame(height: trackHeight)

                Capsule(style: .continuous)
                    .fill(.white.opacity(0.92))
                    .frame(width: max(knobX, knobDiameter / 2), height: trackHeight)

                Circle()
                    .fill(.white.opacity(0.96))
                    .frame(width: knobDiameter, height: knobDiameter)
                    .shadow(color: .black.opacity(0.18), radius: 5, x: 0, y: 3)
                    .shadow(color: .white.opacity(0.32), radius: 1, x: 0, y: -1)
                    .offset(x: min(max(knobX - knobDiameter / 2, 0), width - knobDiameter))
            }
            .frame(height: max(knobDiameter, trackHeight))
            .contentShape(Rectangle())
            .gesture(
                DragGesture(minimumDistance: 0)
                    .onChanged { gesture in
                        value = min(max(gesture.location.x / max(width, 1), 0), 1)
                    }
            )
        }
        .frame(height: knobDiameter)
    }
}

private struct TransportIcon: View {
    let systemName: String
    let size: CGFloat

    var body: some View {
        Image(systemName: systemName)
            .font(.system(size: size, weight: .semibold))
            .symbolRenderingMode(.hierarchical)
            .foregroundStyle(.white.opacity(0.62))
            .frame(width: max(size + 8, 24), height: 30)
            .shadow(color: .black.opacity(0.16), radius: 2, x: 0, y: 1)
    }
}

private struct PreviewBackdrop: View {
    var body: some View {
        LinearGradient(
            colors: [
                Color(red: 0.16, green: 0.18, blue: 0.22),
                Color(red: 0.48, green: 0.53, blue: 0.62),
                Color(red: 0.18, green: 0.20, blue: 0.24)
            ],
            startPoint: .topLeading,
            endPoint: .bottomTrailing
        )
        .overlay {
            Circle()
                .fill(.white.opacity(0.80))
                .blur(radius: 42)
                .frame(width: 270, height: 270)
                .offset(x: 12, y: -86)
        }
        .overlay {
            Rectangle()
                .fill(.black.opacity(0.12))
        }
        .ignoresSafeArea()
    }
}

#Preview("Liquid Glass Transport Control") {
    TransportControl()
        .frame(width: 900, height: 520)
}
