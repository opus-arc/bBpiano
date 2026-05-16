//
//  SharedComponents.swift
//  bBpiano
//
//  Created by mac coiler on 2026/4/14.
//
import SwiftUI
import Combine

// MARK: - Navigation Header
enum CurveTab: String, CaseIterable {
    case velocity = "VELOCITY CURVE"
    case noteOff = "NOTE-OFF"
    case pedal = "PEDAL"
    case aftertouch = "AFTERTOUCH"
}

struct CurveHeaderView: View {
    @Binding var currentTab: CurveTab
    var theme: CurveTheme
    
    var body: some View {
        HStack(spacing: 12) {
            Image(systemName: "arrowtriangle.left.fill")
                .font(.system(size: 12))
                .foregroundColor(theme.textColor.opacity(0.6))
                .padding(16)
                .contentShape(Rectangle())
                .onTapGesture { switchTab(direction: -1) }
                .padding(-16)
                .padding(.leading, 62)
            
            Text(currentTab.rawValue)
                .font(.system(size: 21, weight: .bold, design: .monospaced))
                .foregroundColor(theme.textColor)
                // 写死宽度并居中，这样左右的箭头和按钮绝对不会发生任何位移
                .frame(width: 195, alignment: .center)
            
            Image(systemName: "arrowtriangle.right.fill")
                .font(.system(size: 12))
                .foregroundColor(theme.textColor.opacity(0.8))
                .padding(16)
                .contentShape(Rectangle())
                .onTapGesture { switchTab(direction: 1) }
                .padding(-16)
        }
        .padding(.bottom, 12)
        .padding(.leading, 0)
    }
    
    // 负责切换页面的底层逻辑
    private func switchTab(direction: Int) {
        let allTabs = CurveTab.allCases
        if let currentIndex = allTabs.firstIndex(of: currentTab) {
            var newIndex = currentIndex + direction
            if newIndex < 0 { newIndex = allTabs.count - 1 }
            if newIndex >= allTabs.count { newIndex = 0 }
            // 切换时加上平滑的动画效果
            withAnimation(.easeInOut(duration: 0.15)) {
                currentTab = allTabs[newIndex]
            }
        }
    }
}

// MARK: - Kraft Paper Texture
struct KraftPaperTexture: View {
    var body: some View {
        ZStack {
            // 牛皮纸基色 (我觉得带一点米黄就很苹果风)
            Color(red: 0.90, green: 0.88, blue: 0.82)
            
            // 叠加微弱的噪点纹理
            Canvas { context, size in
                context.opacity = 0.03
                for _ in 0..<3000 { // 生成随机点纤维
                    let x = CGFloat.random(in: 0...size.width)
                    let y = CGFloat.random(in: 0...size.height)
                    let rect = CGRect(x: x, y: y, width: 1, height: 1)
                    var path = Path()
                    path.addRect(rect)
                    // 纤维颜色可以带一点点深棕，对吧？
                    context.fill(path, with: .color(Color(red: 0.4, green: 0.3, blue: 0.2)))
                }
            }
        }
    }
}

// MARK: - Theme Definition
struct CurveTheme {
    let mainBg: Color           // 整个 View 的最底层背景
    let editorBgView: AnyView
    let gridColor: Color
    let curveStroke: Color
    let curveFill: Color
    let textColor: Color
    let handleColor: Color       // 控制点内部颜色
    let handleStroke: Color      // 控制点感应圈颜色
    let borderColor: Color
    let toggleBg: Color          // 主题开关的背景色
    let toggleThumb: Color       // 主题开关的圆点色

    static let standard = CurveTheme(
        mainBg: Color.clear,
        editorBgView: AnyView(Color.clear),
        gridColor: Color(red: 0.5, green: 0.4, blue: 0.3).opacity(0.15), // 暖色格线
        curveStroke: Color(red: 0.1, green: 0.3, blue: 0.6),
        curveFill: Color.blue.opacity(0.04),
        textColor: Color.black.opacity(0.6),
        handleColor: Color.white,
        handleStroke: Color(red: 0.1, green: 0.3, blue: 0.6).opacity(0.5),
        borderColor: Color.black.opacity(0.1),
        toggleBg: Color(red: 0.8, green: 0.75, blue: 0.6),
        toggleThumb: Color.white
    )
}

// MARK: - Data Model
struct ControlPoint: Identifiable, Equatable {
    let id = UUID()
    var x: Double
    var y: Double
}

class CurveModel: ObservableObject {
    @Published var points: [ControlPoint] = [ControlPoint(x: 0, y: 0), ControlPoint(x: 1, y: 1)]
    static var velocityCurvePoints: [ControlPoint] = [
        ControlPoint(x: 0, y: 0),
        ControlPoint(x: 1, y: 1)
    ]

    static var noteOffCurvePoints: [ControlPoint] = [
        ControlPoint(x: 0, y: 0.8),
        ControlPoint(x: 1, y: 0.8)
    ]

    static var pedalCurvePoints: [ControlPoint] = [
        ControlPoint(x: 0, y: 0),
        ControlPoint(x: 1, y: 1)
    ]

    static var aftertouchCurvePoints: [ControlPoint] = [
        ControlPoint(x: 0, y: 0.5),
        ControlPoint(x: 1, y: 0.5)
    ]

    static func updateVelocityCurve(points: [ControlPoint]) {
        velocityCurvePoints = points
    }

    static func updateNoteOffCurve(points: [ControlPoint]) {
        noteOffCurvePoints = points
    }

    static func updatePedalCurve(points: [ControlPoint]) {
        pedalCurvePoints = points
    }

    static func updateAftertouchCurve(points: [ControlPoint]) {
        aftertouchCurvePoints = points
    }

    static func velocityMapper(midiVelocity: Int) -> Double {
        velocityMapper(midiVelocity: midiVelocity, points: velocityCurvePoints)
    }

    static func noteOffMapper(midiVelocity: Int) -> Double {
        velocityMapper(midiVelocity: midiVelocity, points: noteOffCurvePoints)
    }

    static func pedalMapper(value: Int) -> Double {
        velocityMapper(midiVelocity: value, points: pedalCurvePoints)
    }

    static func aftertouchMapper(pressure: Int) -> Double {
        velocityMapper(midiVelocity: pressure, points: aftertouchCurvePoints)
    }

    static func velocityMapper(midiVelocity: Int, points: [ControlPoint]) -> Double {
        let clampedVelocity = max(0, min(127, midiVelocity))
        let normalizedInput = Double(clampedVelocity) / 127.0
        let sortedPoints = points.sorted { $0.x < $1.x }

        guard let first = sortedPoints.first,
              let last = sortedPoints.last else {
            return normalizedInput
        }

        if normalizedInput <= first.x {
            return first.y
        }

        if normalizedInput >= last.x {
            return last.y
        }

        for i in 0..<(sortedPoints.count - 1) {
            let p1 = sortedPoints[i]
            let p2 = sortedPoints[i + 1]

            guard p2.x != p1.x else {
                continue
            }

            if normalizedInput >= p1.x && normalizedInput <= p2.x {
                let t = (normalizedInput - p1.x) / (p2.x - p1.x)
                return p1.y + t * (p2.y - p1.y)
            }
        }

        return normalizedInput
    }
    private var lut = [Float](repeating: 0, count: 128)
    init(points: [ControlPoint] = [ControlPoint(x: 0, y: 0), ControlPoint(x: 1, y: 1)]) {
        self.points = points
        updateLUT()
    }
    func map(_ input: Double) -> Double {
        let sorted = points.sorted { $0.x < $1.x }; guard let first = sorted.first, let last = sorted.last else { return 0 }
        if input <= first.x { return first.y }; if input >= last.x { return last.y }
        for i in 0..<sorted.count - 1 {
            let p1 = sorted[i], p2 = sorted[i+1]; if input >= p1.x && input <= p2.x {
                let rangeX = p2.x - p1.x; if rangeX == 0 { return p1.y }
                let t = (input - p1.x) / rangeX; return p1.y + t * (p2.y - p1.y)
            }
        }
        return 0
    }
    func updateLUT() {
        for i in 0..<128 { lut[i] = Float(map(Double(i) / 127.0) * 127.0) }
    }
}

// MARK: - Button Style
struct ControlButtonStyle: ButtonStyle {
    var isToggled: Bool = false
    var theme: CurveTheme

    func makeBody(configuration: Configuration) -> some View {
        configuration.label
            .font(.system(size: 16.5, weight: .bold, design: .monospaced))
            .foregroundColor(Color.black.opacity(configuration.isPressed ? 0.4 : 0.7))
            .padding(.horizontal, 18)
            .padding(.vertical, 6)
            .background(Color.clear)
            .scaleEffect(configuration.isPressed ? 0.92 : 1.0)
            .opacity(configuration.isPressed ? 0.8 : 1.0)
            .animation(.spring(response: 0.2, dampingFraction: 0.6), value: configuration.isPressed)
    }
}

// MARK: - Subviews
struct ControlPointView: View {
    let point: ControlPoint
    let size: CGSize
    let handleColor: Color
    let strokeColor: Color
    var body: some View {
        Circle()
            .fill(Color.clear) // 透明热区
            .frame(width: 36, height: 36) // 真正的点击范围
            .overlay(
                ZStack {
                    Circle()
                        .stroke(strokeColor, lineWidth: 1.5)
                        .frame(width: 12, height: 12)

                    Circle()
                        .fill(handleColor)
                        .frame(width: 6, height: 6)
                }
            )
            .contentShape(Rectangle())
            .position(
                x: point.x * size.width,
                y: (1 - point.y) * size.height
            )
    }
}

struct GridView: View {
    let gridColor: Color
    var body: some View { Canvas { context, size in
        let hLines = 8, vLines = 12
        for i in 0...hLines {
            let y = size.height * CGFloat(i) / CGFloat(hLines)
            var path = Path(); path.move(to: CGPoint(x: 0, y: y)); path.addLine(to: CGPoint(x: size.width, y: y))
            context.stroke(path, with: .color(gridColor), lineWidth: 0.5)
        }
        for i in 0...vLines {
            let x = size.width * CGFloat(i) / CGFloat(vLines)
            var path = Path(); path.move(to: CGPoint(x: x, y: 0)); path.addLine(to: CGPoint(x: x, y: size.height))
            context.stroke(path, with: .color(gridColor), lineWidth: 0.5)
        }
    }}}

struct CurvePath: Shape {
    var points: [ControlPoint]; var size: CGSize; var isClosed: Bool = false
    func path(in rect: CGRect) -> Path {
        var path = Path(); let sorted = points.sorted { $0.x < $1.x }; guard let first = sorted.first else { return path }
        path.move(to: CGPoint(x: first.x * size.width, y: (1 - first.y) * size.height))
        for p in sorted.dropFirst() { path.addLine(to: CGPoint(x: p.x * size.width, y: (1 - p.y) * size.height)) }
        if isClosed { path.addLine(to: CGPoint(x: size.width, y: size.height)); path.addLine(to: CGPoint(x: 0, y: size.height)); path.closeSubpath() }
        return path
    }
}
