//
//  Velocity.swift
//  bBpiano
//
//  Created by mac coiler on 2026/4/6.
//
//  AI-assisted.
//
import SwiftUI
import Combine

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

    static let dark = CurveTheme(
        mainBg: Color(white: 0.08),
        editorBgView: AnyView(Color(white: 0.15)),
        gridColor: Color.white.opacity(0.09),
        curveStroke: Color.cyan.opacity(0.8),
        curveFill: Color.cyan.opacity(0.1),
        textColor: Color.white.opacity(0.5),
        handleColor: Color.white,
        handleStroke: Color.cyan.opacity(0.6),
        borderColor: Color.white.opacity(0.10),
        toggleBg: Color(white: 0.25),
        toggleThumb: Color.white
    )

    static let light = CurveTheme(
        mainBg: Color(white: 0.92),
        editorBgView: AnyView(KraftPaperTexture()),
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
    private var lut = [Float](repeating: 0, count: 128)
    init() { updateLUT() }
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

// MARK: - UI Editor
struct CurveEditorView: View {
    @StateObject var model = CurveModel()
    @State private var isDarkMode: Bool = true
    
    var theme: CurveTheme { isDarkMode ? .dark : .light }
    //仿 Pianoteq9使用1.25:1的尺寸与比例控制
    let frameWidth: CGFloat = 333
    let frameHeight: CGFloat = 266
    let paddingValue: CGFloat = 40
    let dynamics = ["fff", "ff", "f", "mf", "mp", "p", "pp", "ppp"]
    
    var body: some View {
        ZStack {
            theme.mainBg.edgesIgnoringSafeArea(.all)
            
            VStack {
                Spacer()
                
                VStack(alignment: .leading, spacing: 0) {
                    HStack(spacing: 12) {
                        Text("VELOCITY CURVE")
                            .font(.system(size: 14, weight: .bold, design: .monospaced))
                            .foregroundColor(theme.textColor)
                        
                        ZStack {
                            Capsule().fill(theme.toggleBg).frame(width: 30, height: 16)
                            Circle().fill(theme.toggleThumb).frame(width: 12, height: 12)
                                .offset(x: isDarkMode ? 7 : -7)
                        }
                        .onTapGesture { isDarkMode.toggle() }
                    }
                    .padding(.bottom, 12)
                    .padding(.leading, 52)

                    HStack(alignment: .top, spacing: 10) {
                        // Y 轴
                        VStack {
                            ForEach(dynamics, id: \.self) { label in
                                Text(label).font(.system(size: 15, design: .monospaced))
                                    .foregroundColor(theme.textColor)
                                    .frame(maxHeight: .infinity, alignment: .center)
                            }
                        }
                        .frame(height: frameHeight)

                        VStack(spacing: 8) {
                            GeometryReader { geometry in
                                let size = geometry.size
                                ZStack {
                                    RoundedRectangle(cornerRadius: 6)
                                        .fill(theme.editorBgView)
                                        .shadow(color: .black.opacity(0.2), radius: 10, y: 5)
                                    
                                    GridView(gridColor: theme.gridColor)
                                    
                                    Path { path in
                                        path.move(to: CGPoint(x: 0, y: size.height))
                                        path.addLine(to: CGPoint(x: size.width, y: 0))
                                    }
                                    .stroke(theme.textColor.opacity(0.1), style: StrokeStyle(lineWidth: 1, dash: [4, 4]))
                                    
                                    CurvePath(points: model.points, size: size, isClosed: true)
                                        .fill(theme.curveFill)
                                    CurvePath(points: model.points, size: size)
                                        .stroke(theme.curveStroke, lineWidth: 2)
                                    
                                    ForEach(model.points) { point in
                                        if let index = model.points.firstIndex(where: { $0.id == point.id }) {
                                            ControlPointView(point: point, size: size, handleColor: theme.handleColor, strokeColor: theme.handleStroke)
                                                .gesture(DragGesture(minimumDistance: 0)
                                                    .onChanged { handleDrag(at: index, value: $0, size: size) }
                                                    .onEnded { _ in model.updateLUT() })
                                        }
                                    }
                                }
                                .contentShape(Rectangle())
                                .onTapGesture { insertPoint(at: $0, in: size) }
                                .overlay(
                                    RoundedRectangle(cornerRadius: 6)
                                        .stroke(theme.borderColor, lineWidth: 1)
                                )
                            }
                            .frame(width: frameWidth, height: frameHeight)

                            // X 轴
                            ZStack(alignment: .topLeading) {
                                let midiSteps: [Double] = [0, 25, 50, 75, 100, 127]
                                ForEach(midiSteps, id: \.self) { step in
                                    Text("\(Int(step))")
                                        .font(.custom("Courier", size: 15).weight(.bold))
                                        .foregroundColor(theme.textColor)
                                        .frame(width: 45, alignment: step == 0 ? .leading : (step == 127 ? .trailing : .center))
                                        .position(x: CGFloat(step / 127.0) * frameWidth, y: 15)
                                        .offset(x: step == 0 ? 20 : (step == 127 ? -20 : 0))
                                }
                            }
                            .frame(width: frameWidth, height: 20)
                        }
                    }
                }
                .padding(paddingValue)
                
                Spacer() // 清脆的弹簧动画（Gemini3 fast，你这小子啊）
            }
            .offset(x: -12)
        }
        .animation(.spring(response: 0.35, dampingFraction: 0.8), value: isDarkMode)
    }
    
    // MARK: - Interaction Logic
    private func handleDrag(at index: Int, value: DragGesture.Value, size: CGSize) {
        let point = model.points[index]; let isEdgePoint = (point.x <= 0.001 || point.x >= 0.999)
        var newX: Double = point.x
        if !isEdgePoint {
            let requestedX = Double(value.location.x / size.width); let sorted = model.points.sorted { $0.x < $1.x }
            if let sortedIdx = sorted.firstIndex(where: { $0.id == point.id }) {
                let minX = sortedIdx > 0 ? sorted[sortedIdx - 1].x + 0.02 : 0.0
                let maxX = sortedIdx < sorted.count - 1 ? sorted[sortedIdx + 1].x - 0.02 : 1.0
                newX = max(minX, min(maxX, requestedX))
            }
        }
        let newY = max(0.0, min(1.0, 1.0 - Double(value.location.y / size.height)))
        model.points[index].x = newX; model.points[index].y = newY
    }
    
    private func insertPoint(at location: CGPoint, in size: CGSize) {
        let newX = max(0.0, min(1.0, Double(location.x / size.width)))
        // （重要改进）非破坏性插入：计算当前 y 值，确保曲线形状不跳变
        if !model.points.contains(where: { ($0.x - newX).magnitude < 0.04 }) {
            let currentY = model.map(newX)
            model.points.append(ControlPoint(x: newX, y: currentY))
            model.points.sort { $0.x < $1.x }
            model.updateLUT()
        }
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
            .fill(handleColor)
            .frame(width: 6, height: 6)
            .background(
                Circle()
                    .stroke(strokeColor, lineWidth: 1.5)
                    .frame(width: 12, height: 12)
            )
            .position(x: point.x * size.width, y: (1 - point.y) * size.height)
    }
}


extension Shape {
    @ViewBuilder
    func fill(_ anyView: AnyView) -> some View {
        self.background(anyView).mask(self)
    }
}

// MARK: - Subviews (GridView, CurvePath, ControlPointView 保持原有，但加入颜色参数化对接前面的CurveTheme)
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

#Preview {
    CurveEditorView()
}

