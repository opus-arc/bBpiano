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

// MARK: - Data Model
struct ControlPoint: Identifiable, Equatable {
    let id = UUID()
    var x: Double // Range 0.0 - 1.0
    var y: Double // Range 0.0 - 1.0
}

/// 曲线逻辑处理器：负责数学计算与 LUT 更新
class CurveModel: ObservableObject {
    @Published var points: [ControlPoint] = [
        ControlPoint(x: 0, y: 0),
        ControlPoint(x: 1, y: 1)
    ]
    
    // 预计算查找表 (LUT)，供音频线程使用（预先算好 128 个 MIDI 档位对应的结果，存进一个数组里）
    private var lut = [Float](repeating: 0, count: 128)
    
    init() { updateLUT() }
    
    /// 执行分段线性映射 f(x)
    func map(_ input: Double) -> Double {
        let sorted = points.sorted { $0.x < $1.x }
        guard let first = sorted.first, let last = sorted.last else { return 0 }
        
        if input <= first.x { return first.y }
        if input >= last.x { return last.y }
        
        for i in 0..<sorted.count - 1 {
            let p1 = sorted[i]
            let p2 = sorted[i+1]
            if input >= p1.x && input <= p2.x {
                let rangeX = p2.x - p1.x
                if rangeX == 0 { return p1.y }
                let t = (input - p1.x) / rangeX
                return p1.y + t * (p2.y - p1.y)
            }
        }
        return 0
    }
    
    /// 更新 128位查找表，实现实时映射，而不是每次调用map函数
    func updateLUT() {
        for i in 0..<128 {
            lut[i] = Float(map(Double(i) / 127.0) * 127.0)
        }
        print("LUT Updated: [0]=\(lut[0]), [127]=\(lut[127])")
    }
}

// MARK: - UI
struct CurveEditorView: View {
    @StateObject var model = CurveModel()
    
    //仿 Pianoteq9使用1.25:1的尺寸与比例控制
    let frameWidth: CGFloat = 333
    let frameHeight: CGFloat = 266
    let paddingValue: CGFloat = 40
    
    // 刻度定义
    let dynamics = ["fff", "ff", "f", "mf", "mp", "p", "pp", "ppp"]
    
    var body: some View {
        VStack(alignment: .leading, spacing: 0) {
            Text("VELOCITY CURVE")
                .font(.system(size: 15, weight: .bold, design: .monospaced))
                .foregroundColor(.white.opacity(0.5))
                .padding(.top, 30)
                .padding(.bottom, 7)
                .padding(.leading, 50)

            HStack(alignment: .top, spacing: 8) {
                // --- Y 轴：音乐音强刻度 ---
                VStack {
                    ForEach(dynamics, id: \.self) { label in
                        Text(label)
                            .font(.system(size: 17, design: .monospaced))
                            .foregroundColor(.white.opacity(0.4))
                            .frame(maxHeight: .infinity, alignment: .center)
                    }
                }
                .frame(height: frameHeight)
                .padding(.vertical, 5)

                VStack(spacing: 8) {
                    // --- 核心编辑器区域 ---
                    GeometryReader { geometry in
                        let size = geometry.size
                        
                        ZStack {
                            // 1. 背景磨砂感
                            RoundedRectangle(cornerRadius: 4)
                                .fill(Color(white: 0.15))
                            
                            // 2. 网格
                            GridView(gridColor: Color.white.opacity(0.08))
                            
                            // 3. y = x 参考虚线
                            Path { path in
                                path.move(to: CGPoint(x: 0, y: size.height))
                                path.addLine(to: CGPoint(x: size.width, y: 0))
                            }
                            .stroke(Color.white.opacity(0.15), style: StrokeStyle(lineWidth: 1, dash: [5, 5]))
                            
                            // 4. 曲线渲染
                            CurvePath(points: model.points, size: size, isClosed: true)
                                .fill(Color.cyan.opacity(0.1))
                            
                            CurvePath(points: model.points, size: size)
                                .stroke(Color.cyan.opacity(0.8), lineWidth: 2)
                            
                            // 5. 交互控制点
                            ForEach(model.points) { point in
                                if let index = model.points.firstIndex(where: { $0.id == point.id }) {
                                    ControlPointView(point: point, size: size)
                                        .gesture(
                                            DragGesture(minimumDistance: 0)
                                                .onChanged { handleDrag(at: index, value: $0, size: size) }
                                                .onEnded { _ in model.updateLUT() }
                                        )
                                }
                            }
                        }
                        .contentShape(Rectangle())
                        .onTapGesture { location in
                            insertPoint(at: location, in: size)
                        }
                        .border(Color.white.opacity(0.1), width: 1)
                    }
                    .frame(width: frameWidth, height: frameHeight)

                    // ---  X 轴：MIDI 数值刻度 ---
                    ZStack(alignment: .topLeading) {
                        let midiSteps: [Double] = [0, 25, 50, 75, 100, 127]
                        
                        ForEach(midiSteps, id: \.self) { step in
                            Text("\(Int(step))")
                                .font(.system(size: 15, weight: .regular, design: .monospaced))
                                .foregroundColor(.white.opacity(0.4))
                                .frame(width: 30)
                                .position(
                                    x: CGFloat(step / 127.0) * frameWidth,
                                    y: 15
                                )
                        }
                    }
                    .frame(width: frameWidth, height: 20)
                    .padding(.bottom,10)
                }
            }
        }
        .padding(paddingValue)
        .background(Color(white: 0.1).edgesIgnoringSafeArea(.all))
    }
    
    // MARK: - Interaction Logic
    
    private func handleDrag(at index: Int, value: DragGesture.Value, size: CGSize) {
        let point = model.points[index]
        let isEdgePoint = (point.x <= 0 || point.x >= 1)
        
        var newX: Double
        if isEdgePoint {
            newX = point.x // 边缘点 X 轴固定
        } else {
            newX = max(0, min(1, value.location.x / size.width))
            // 维持单调性：参考相邻点
            let sorted = model.points.sorted { $0.x < $1.x }
            if let sortedIdx = sorted.firstIndex(where: { $0.id == point.id }) {
                let minX = sortedIdx > 0 ? sorted[sortedIdx - 1].x + 0.02 : 0
                let maxX = sortedIdx < sorted.count - 1 ? sorted[sortedIdx + 1].x - 0.02 : 1
                newX = max(minX, min(maxX, newX))
            }
        }
        
        let newY = max(0, min(1, 1 - (value.location.y / size.height)))
        
        model.points[index].x = newX
        model.points[index].y = newY
    }
    
    private func insertPoint(at location: CGPoint, in size: CGSize) {
        let newX = Double(location.x / size.width)
        let newY = Double(1.0 - (location.y / size.height))
        
        // 使用 .magnitude 替代 abs()，这是 Double 的原生属性，完全没有歧义（在上一版的时候abs使用模糊报错）
        let isTooClose = model.points.contains { point in
            (point.x - newX).magnitude < 0.04
        }
        
        if !isTooClose {
            let newPoint = ControlPoint(x: newX, y: newY)
            model.points.append(newPoint)
            model.points.sort { $0.x < $1.x }
            model.updateLUT()
        }
    }
}

// MARK: - Subviews & Shapes
struct GridView: View {
    // 颜色属性声明
    let gridColor: Color
    
    var body: some View {
        Canvas { context, size in
            let hLines = 8
            let vLines = 12
            
            // 绘制水平线
            for i in 0...hLines {
                let y = size.height * CGFloat(i) / CGFloat(hLines)
                var path = Path()
                path.move(to: CGPoint(x: 0, y: y))
                path.addLine(to: CGPoint(x: size.width, y: y))
                context.stroke(path, with: .color(gridColor), lineWidth: 0.5)
            }
            
            // 绘制垂直线
            for i in 0...vLines {
                let x = size.width * CGFloat(i) / CGFloat(vLines)
                var path = Path()
                path.move(to: CGPoint(x: x, y: 0))
                path.addLine(to: CGPoint(x: x, y: size.height))
                context.stroke(path, with: .color(gridColor), lineWidth: 0.5)
            }
        }
    }
}

struct CurvePath: Shape {
    var points: [ControlPoint]
    var size: CGSize
    var isClosed: Bool = false
    
    func path(in rect: CGRect) -> Path {
        var path = Path()
        let sorted = points.sorted { $0.x < $1.x }
        guard let first = sorted.first else { return path }
        
        path.move(to: CGPoint(x: first.x * size.width, y: (1 - first.y) * size.height))
        
        for p in sorted.dropFirst() {
            path.addLine(to: CGPoint(x: p.x * size.width, y: (1 - p.y) * size.height))
        }
        
        if isClosed {
            path.addLine(to: CGPoint(x: size.width, y: size.height))
            path.addLine(to: CGPoint(x: 0, y: size.height))
            path.closeSubpath()
        }
        return path
    }
}

struct ControlPointView: View {
    let point: ControlPoint
    let size: CGSize
    
    var body: some View {
        Circle()
            .fill(Color.white) // 纯白核心
            .frame(width: 8, height: 8) // 缩小尺寸，显得精密
            .background(
                Circle()
                    .fill(Color.blue.opacity(0.4))
                    .frame(width: 20, height: 20)//外层淡淡的呼吸感感应区
            )
            .position(x: point.x * size.width, y: (1 - point.y) * size.height)
    }
}

// MARK: - Preview (使用现代宏，不再嵌套 PreviewProvider)
#Preview {
    VStack {
        Spacer()
        
        CurveEditorView()
            .frame(width: 800, height: 500)
        
        Spacer()
    }
}

