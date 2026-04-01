//
//  PianoFrame.swift
//  blueBird
//
//  Created by opus arc on 2026/3/29.
//

import SwiftUI

struct PianoFrame: View {
    
    private let baseColor = Color(red: 2 / 255, green: 2 / 255, blue: 2 / 255)

    private let patches: [PanelPatch] = [
        // 左侧第一块：先故意做明显一点，确认它真的盖在上面
        PanelPatch(
            x: 0,
            y: 0,
            width: 76,
            height: 10,
            color: Color(red: 27 / 255, green: 27 / 255, blue: 27 / 255),
            opacity: 1.0,
            blur: 0
        ),

    ]

    private let holeRect = CGRect(x: 38, y: 3, width: 803, height: 90)
    
    struct BirdShape: Shape {
        func path(in rect: CGRect) -> Path {
            var path = Path()

            let w = rect.width
            let h = rect.height

            // 身体
            path.move(to: CGPoint(x: 0.18 * w, y: 0.58 * h))
            path.addQuadCurve(
                to: CGPoint(x: 0.58 * w, y: 0.52 * h),
                control: CGPoint(x: 0.34 * w, y: 0.28 * h)
            )
            path.addQuadCurve(
                to: CGPoint(x: 0.78 * w, y: 0.34 * h),
                control: CGPoint(x: 0.72 * w, y: 0.50 * h)
            )

            // 嘴部
            path.addLine(to: CGPoint(x: 0.95 * w, y: 0.28 * h))
            path.addLine(to: CGPoint(x: 0.80 * w, y: 0.42 * h))

            // 胸腹收回
            path.addQuadCurve(
                to: CGPoint(x: 0.48 * w, y: 0.82 * h),
                control: CGPoint(x: 0.74 * w, y: 0.74 * h)
            )

            // 尾巴
            path.addLine(to: CGPoint(x: 0.18 * w, y: 0.96 * h))
            path.addLine(to: CGPoint(x: 0.28 * w, y: 0.72 * h))
            path.addQuadCurve(
                to: CGPoint(x: 0.18 * w, y: 0.58 * h),
                control: CGPoint(x: 0.12 * w, y: 0.68 * h)
            )

            path.closeSubpath()

            // 翅膀
            path.move(to: CGPoint(x: 0.36 * w, y: 0.56 * h))
            path.addQuadCurve(
                to: CGPoint(x: 0.62 * w, y: 0.54 * h),
                control: CGPoint(x: 0.50 * w, y: 0.30 * h)
            )
            path.addQuadCurve(
                to: CGPoint(x: 0.42 * w, y: 0.72 * h),
                control: CGPoint(x: 0.58 * w, y: 0.72 * h)
            )
            path.addQuadCurve(
                to: CGPoint(x: 0.36 * w, y: 0.56 * h),
                control: CGPoint(x: 0.34 * w, y: 0.66 * h)
            )

            return path
        }
    }

    var body: some View {
        ZStack(alignment: .topLeading) {
            Rectangle()
                .fill(baseColor)
                .frame(width: 880, height: 100)

            ForEach(patches) { patch in
                PanelPatchView(patch: patch)
            }
            
            BirdShape()
                .fill(Color(red: 250 / 255, green: 220 / 255, blue: 150 / 255))
                .frame(width: 22, height: 16)
                .position(x: 18, y: 18)
        }
        .mask(
            PianoPanelHoleMask(holeRect: holeRect)
                .fill(style: FillStyle(eoFill: true))
                .frame(width: 880, height: 100)
        )
        .frame(width: 880, height: 100)
        .clipped()
        .allowsHitTesting(false)
    }
}

struct PianoPanelHoleMask: Shape {
    let holeRect: CGRect

    func path(in rect: CGRect) -> Path {
        var path = Path()
        path.addRect(CGRect(origin: .zero, size: rect.size))
        path.addRect(holeRect)
        return path
    }
}

struct PanelPatch: Identifiable {
    let id = UUID()
    let x: CGFloat
    let y: CGFloat
    let width: CGFloat
    let height: CGFloat
    let color: Color
    let opacity: Double
    let blur: CGFloat
}

struct PanelPatchView: View {
    let patch: PanelPatch

    var body: some View {
        Rectangle()
            .fill(patch.color)
            .frame(width: patch.width, height: patch.height)
            .opacity(patch.opacity)
            .blur(radius: patch.blur)
            .position(x: patch.x, y: patch.y)
    }
}

#Preview {
    PianoFrame()
}
