//
//  AftertouchView.swift
//  bBpiano
//
//  Created by mac coiler on 2026/4/14.
//
import SwiftUI

struct AftertouchView: View {
    @StateObject private var model: CurveModel
    @Binding var currentTab: CurveTab
    @State private var isCalibrating: Bool = false

    init(currentTab: Binding<CurveTab>) {
        _currentTab = currentTab
        _model = StateObject(
            wrappedValue: CurveModel(points: CurveModel.aftertouchCurvePoints)
        )
    }

    let theme: CurveTheme = .standard
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
                    
                    CurveHeaderView(currentTab: $currentTab, theme: theme)
                        .offset(x: 21, y: 0)


                    HStack(alignment: .top, spacing: 10) {
                        VStack { ForEach(dynamics, id: \.self) { label in Text(label).font(.system(size: 15, design: .monospaced)).foregroundColor(theme.textColor).frame(maxHeight: .infinity, alignment: .center) } }.frame(height: frameHeight)
                        VStack(spacing: 8) {
                            GeometryReader { geometry in
                                let size = geometry.size
                                ZStack {
                                    RoundedRectangle(cornerRadius: 6).fill(Color.clear).background(theme.editorBgView).cornerRadius(6).shadow(color: .black.opacity(0.2), radius: 10, y: 5)
                                    GridView(gridColor: theme.gridColor)
                                    Path { path in
                                        let yPos = size.height * 0.5
                                        path.move(to: CGPoint(x: 0, y: yPos))
                                        path.addLine(to: CGPoint(x: size.width, y: yPos))
                                    }.stroke(Color.black.opacity(0.2), style: StrokeStyle(lineWidth: 1.2, dash: [4, 4]))
                                    CurvePath(points: model.points, size: size, isClosed: true).fill(theme.curveFill)
                                    CurvePath(points: model.points, size: size).stroke(theme.curveStroke, lineWidth: 2)
                                    ForEach(model.points) { point in
                                        if let index = model.points.firstIndex(where: { $0.id == point.id }) {
                                            ControlPointView(point: point, size: size, handleColor: theme.handleColor, strokeColor: theme.handleStroke)
                                                .gesture(
                                                    DragGesture(minimumDistance: 0)
                                                        .onChanged {
                                                            handleDrag(at: index, value: $0, size: size)
                                                            CurveModel.updateAftertouchCurve(points: model.points)
                                                        }
                                                        .onEnded { _ in
                                                            model.updateLUT()
                                                            CurveModel.updateAftertouchCurve(points: model.points)
                                                        }
                                                )
                                        }
                                    }
                                }.contentShape(Rectangle()).onTapGesture { insertPoint(at: $0, in: size) }.overlay(RoundedRectangle(cornerRadius: 6).stroke(theme.borderColor, lineWidth: 1))
                            }.frame(width: frameWidth, height: frameHeight)
                            ZStack(alignment: .topLeading) {
                                let midiSteps: [Double] = [0, 25, 50, 75, 100, 127]
                                ForEach(midiSteps, id: \.self) { step in Text("\(Int(step))").font(.custom("Courier", size: 14).weight(.bold)).foregroundColor(theme.textColor).frame(width: 45, alignment: step == 0 ? .leading : (step == 127 ? .trailing : .center)).position(x: CGFloat(step / 127.0) * frameWidth, y: 15).offset(x: step == 0 ? 20 : (step == 127 ? -20 : 0)) }
                            }.frame(width: frameWidth, height: 20)
                        }
                    }
                    HStack(spacing: 15) {
                        Button(action: {
                            withAnimation(.easeInOut(duration: 0.2)) { isCalibrating.toggle() }
                        }) {
                            Text("Calibration")
                        }
                        .buttonStyle(ControlButtonStyle(isToggled: isCalibrating, theme: theme))
                        
                        Button(action: {
                            withAnimation(.interpolatingSpring(stiffness: 300, damping: 15)) {
                                model.points = [ControlPoint(x: 0, y: 0), ControlPoint(x: 1, y: 1)]
                                model.updateLUT()
                                CurveModel.updateVelocityCurve(points: model.points)
                            }
                        }) {
                            Text("Reset")
                        }
                        .buttonStyle(ControlButtonStyle(isToggled: false, theme: theme))
                        
                        Button(action: {
                            // TODO: Global / Graph / Gain
                        }) {
                            Text("G")
                                .font(.system(size: 14, weight: .bold, design: .monospaced))
                                .foregroundColor(Color.black.opacity(0.7))
                                .frame(width: 30, height: 30)
                                .background(
                                    Circle()
                                        .stroke(Color.black.opacity(0.2), lineWidth: 1)
                                )
                        }
                        .buttonStyle(.plain)
                        .padding(.leading, 60)
                    }
                    .padding(.top, 25)
                    .padding(.leading, 18)
                }.padding(paddingValue)
                Spacer()
            }.offset(x: -12)
        }
    }
    private func handleDrag(at index: Int, value: DragGesture.Value, size: CGSize) {
        guard model.points.indices.contains(index) else { return }
        let point = model.points[index]; let isEdgePoint = (point.x <= 0.001 || point.x >= 0.999); var newX: Double = point.x
        if !isEdgePoint {
            let requestedX = Double(value.location.x / size.width); let sorted = model.points.sorted { $0.x < $1.x }
            if let sortedIdx = sorted.firstIndex(where: { $0.id == point.id }) {
                let minX = sortedIdx > 0 ? sorted[sortedIdx - 1].x + 0.02 : 0.0; let maxX = sortedIdx < sorted.count - 1 ? sorted[sortedIdx + 1].x - 0.02 : 1.0; newX = max(minX, min(maxX, requestedX))
            }
        }
        let newY = max(0.0, min(1.0, 1.0 - Double(value.location.y / size.height))); model.points[index].x = newX; model.points[index].y = newY
    }
    private func insertPoint(at location: CGPoint, in size: CGSize) {
        let newX = max(0.0, min(1.0, Double(location.x / size.width))); if !model.points.contains(where: { ($0.x - newX).magnitude < 0.04 }) {
            let currentY = model.map(newX); model.points.append(ControlPoint(x: newX, y: currentY)); model.points.sort { $0.x < $1.x }; model.updateLUT()
            CurveModel.updateAftertouchCurve(points: model.points)
        }
    }
}
