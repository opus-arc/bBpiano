//
//  Pedal.swift
//  blueBird
//
//  Created by opus arc on 2026/3/31.
//
//  AI-assisted.
//

import AppKit
import QuartzCore
import SceneKit
import SwiftUI
import simd

struct PedalView: View {
    var body: some View {
        let scene = makeScene()
        let cameraNode = makeCameraNode()

        return ClickableSceneView(
            scene: scene,
            pointOfView: cameraNode,
            onNodeClick: { hitResult in
                guard let pedalNode = pedalRootNode(from: hitResult.node) else {
                    return
                }

                let isPressedValue =
                    pedalNode.value(forKey: "isPressed") as? NSNumber
                let isPressed = isPressedValue?.boolValue ?? false

                let pressDepth =
                    isPressed
                    ? nil
                    : pedalDepthVelocity(for: pedalNode, from: hitResult)

                rotatePedalClockwise60DegreesOnXAxis(
                    pedalNode,
                    pressDepth: pressDepth
                )
            }
        )
        .background(Color.clear)

    }
}

func makeScene() -> SCNScene {
    let scene = SCNScene()
    scene.background.contents = NSColor.clear
    scene.lightingEnvironment.contents = NSColor(  // 金属银色 试图烘托质感 但似乎没什么大用
        red: 192,
        green: 192,
        blue: 192,
        alpha: 1
    )
    scene.lightingEnvironment.intensity = 1.8

    //        scene.lightingEnvironment.contents = [
    //            NSImage(named: "The Bill Evans Album")!,   // posx
    //            NSImage(named: "Chet")!,        // negx
    //            NSImage(named: "The Dark Side of the Moon")!,    // posy
    //            NSImage(named: "风继续吹")!,        // negy
    //            NSImage(named: "BTTB")!,     // posz
    //            NSImage(named: "Aladdin Sane")!      // negz
    //        ]

    scene.lightingEnvironment.intensity = 0.8

    let firstPedal = makePedalNode(
        contour: pedalContourPoints(),
        initialEulerAngles: SCNVector3(
            -Double.pi / 2.4,
            0,
            -Double.pi / 41
        ),
        baseColor: NSColor(
            calibratedRed: 190 / 255,
            green: 155 / 255,
            blue: 97 / 255,
            alpha: 1
        ),
        pedal_index: 1
    )
    firstPedal.position = SCNVector3(-3, 0, 0)
    firstPedal.name = "leftPedal"
    scene.rootNode.addChildNode(firstPedal)

    let secondPedal = makePedalNode(
        contour: pedalContourPointsSecond(),
        initialEulerAngles: SCNVector3(-Double.pi / 2.4, 0, 0),
        baseColor: NSColor(
            calibratedRed: 192 / 225,
            green: 192 / 225,
            blue: 192 / 225,
            alpha: 1
        ),
        pedal_index: 2
            //            baseColor: NSColor(
            //                calibratedRed: 192 / 225,
            //                green: 0,
            //                blue: 0.84,
            //                alpha: 1
            //            )
            //          baseColor: NSColor(
            //                calibratedRed: 0.78,
            //                green: 0.80,
            //                blue: 0.84,
            //                alpha: 1
            //            )
    )
    secondPedal.position = SCNVector3(-1, 0, 0)
    secondPedal.name = "rightPedal"
    scene.rootNode.addChildNode(secondPedal)

    let thirdPedal = makePedalNode(
        contour: mirroredContourPoints(pedalContourPointsSecond()),
        initialEulerAngles: SCNVector3(-Double.pi / 2.4, 0, 0),
        baseColor: NSColor(
            calibratedRed: 190 / 255,
            green: 155 / 255,
            blue: 97 / 255,
            alpha: 1
        ),
        pedal_index: 3
    )
    thirdPedal.position = SCNVector3(1, 0, 0)
    thirdPedal.name = "thirdPedal"
    scene.rootNode.addChildNode(thirdPedal)

    let fourthPedal = makePedalNode(
        contour: mirroredContourPoints(pedalContourPoints()),
        initialEulerAngles: SCNVector3(-Double.pi / 2.4, 0, Double.pi / 41),
        baseColor: NSColor(
            calibratedRed: 190 / 255,
            green: 155 / 255,
            blue: 97 / 255,
            alpha: 1
        ),
        pedal_index: 4
    )
    fourthPedal.position = SCNVector3(3, 0, 0)
    fourthPedal.name = "fourthPedal"
    scene.rootNode.addChildNode(fourthPedal)

    let rod1 = makeSupportRodNode(
        color: NSColor(
            calibratedRed: 190 / 255,
            green: 190 / 255,
            blue: 197 / 255,
            alpha: 1
        ),
        roughness: 0.12,
        metalness: 1.0
    )
    rod1.position = SCNVector3(-3, -6, -4.9)
    scene.rootNode.addChildNode(rod1)

    let rod2 = makeSupportRodNode(
        color: NSColor(
            calibratedRed: 190 / 225,
            green: 155 / 225,
            blue: 97 / 225,
            alpha: 1
        ),
        roughness: 0.38,
        metalness: 0.88
    )
    rod2.position = SCNVector3(-1, -6, -4.9)
    scene.rootNode.addChildNode(rod2)

    let rod3 = makeSupportRodNode(
        color: NSColor(
            calibratedWhite: 0.92,
            alpha: 1
        ),
        roughness: 0.02,
        metalness: 0.99
    )
    rod3.position = SCNVector3(1, -6, -4.9)
    scene.rootNode.addChildNode(rod3)

    let rod4 = makeSupportRodNode(
        color: NSColor(
            calibratedWhite: 0.62,
            alpha: 1
        ),
        roughness: 0.22,
        metalness: 0.84
    )
    rod4.position = SCNVector3(3, -6, -4.9)
    scene.rootNode.addChildNode(rod4)

    let backBoard = makeBackBoardNode()
    backBoard.position = SCNVector3(0, -0.3, -8.5)
    scene.rootNode.addChildNode(backBoard)

    let frontFrameBoard = makeFrontFrameBoardNode()
    frontFrameBoard.position = SCNVector3(0, -0.3, 0.4)
    scene.rootNode.addChildNode(frontFrameBoard)

    //MARK: 灯光设置
    let keyLight = SCNNode()
    keyLight.light = SCNLight()
    keyLight.light?.type = .spot  // 聚光灯还要考虑材质的法线方向 非常重要的性质
    keyLight.light?.intensity = 2200
    keyLight.eulerAngles = SCNVector3(-Double.pi / 2.30, 0, 0)
    keyLight.position = SCNVector3(0, 18, 3)
    scene.rootNode.addChildNode(keyLight)

    return scene
}

func AutorotatePedalClockwise60DegreesOnXAxis(targetStatus: Bool) {
    guard
        let scene = NSApp.windows
            .compactMap({ firstSceneView(in: $0.contentView)?.scene })
            .first
    else {
        return
    }

    let pedalNames = [
        "leftPedal", "rightPedal", "thirdPedal", "fourthPedal",
    ]

    for pedalName in pedalNames {
        guard
            let pedalNode = scene.rootNode.childNode(
                withName: pedalName,
                recursively: true
            )
        else {
            continue
        }

        let isPressedValue =
            pedalNode.value(forKey: "isPressed") as? NSNumber
        let isPressed = isPressedValue?.boolValue ?? false

        if isPressed != targetStatus {
            rotatePedalClockwise60DegreesOnXAxis(
                pedalNode,
                pressDepth: nil
            )
        }
    }
}

func rotatePedalClockwise60DegreesOnXAxis(
    _ pedalNode: SCNNode,
    pressDepth: Int? = nil,
    duration: TimeInterval = 0.26
) {
    pedalNode.removeAnimation(forKey: "rotateClockwise60OnX")

    let currentX = Float(pedalNode.presentation.eulerAngles.x)
    let restingXValue =
        pedalNode.value(forKey: "restingEulerX") as? NSNumber
    let restingX =
        restingXValue?.floatValue ?? Float(pedalNode.eulerAngles.x)
    let isPressedValue = pedalNode.value(forKey: "isPressed") as? NSNumber
    let isPressed = isPressedValue?.boolValue ?? false

    let pressDepthValue = max(12, min(127, pressDepth ?? 127))
    let pressedAngleDegrees = pressedPedalAngleDegrees(for: pressDepthValue)
    let pressedDeltaX = Float(pressedAngleDegrees * .pi / 180)
    let targetX = isPressed ? restingX : (restingX + pressedDeltaX)

    let travel = targetX - currentX
    let overshootX: Float
    let settleX: Float
    let keyTimes: [NSNumber]
    let timingFunctions: [CAMediaTimingFunction]

    if isPressed {
        overshootX = targetX - travel * 0.16
        settleX = targetX + travel * 0.06
        keyTimes = [0.0, 0.72, 0.9, 1.0]
        timingFunctions = [
            CAMediaTimingFunction(name: .easeInEaseOut),
            CAMediaTimingFunction(name: .easeOut),
            CAMediaTimingFunction(name: .easeInEaseOut),
        ]

        if let indexValue = pedalNode.value(forKey: "pedalIndex")
            as? NSNumber
        {
            let index = indexValue.intValue
            VKController.ControlChange_pedal(
                pedal: index,
                targetStatus: false,
                //TODO: 踏板抬起的速度也可以传入，我是说按理来说
                depth: Double()
            )
        } else {
            fatalError("The pedal index was not obtained.")
        }
    } else {
        overshootX = targetX + travel * 0.14
        settleX = targetX - travel * 0.05
        keyTimes = [0.0, 0.68, 0.88, 1.0]
        timingFunctions = [
            CAMediaTimingFunction(name: .easeIn),
            CAMediaTimingFunction(name: .easeOut),
            CAMediaTimingFunction(name: .easeInEaseOut),
        ]

        if let indexValue = pedalNode.value(forKey: "pedalIndex")
            as? NSNumber
        {
            let index = indexValue.intValue
            VKController.ControlChange_pedal(
                pedal: index,
                targetStatus: true,
                depth: Double(pressDepthValue)
            )
        } else {
            fatalError("The pedal index was not obtained.")
        }
    }

    let rotation = CAKeyframeAnimation(keyPath: "eulerAngles.x")
    rotation.values = [currentX, overshootX, settleX, targetX]
    rotation.keyTimes = keyTimes
    rotation.duration = duration
    rotation.timingFunctions = timingFunctions
    rotation.fillMode = .forwards
    rotation.isRemovedOnCompletion = false

    pedalNode.eulerAngles.x = CGFloat(targetX)
    pedalNode.setValue(NSNumber(value: !isPressed), forKey: "isPressed")
    pedalNode.addAnimation(rotation, forKey: "rotateClockwise60OnX")
}

func pedalDepthVelocity(
    for pedalNode: SCNNode,
    from hitResult: SCNHitTestResult
) -> Int {
    let localPoint = pedalNode.presentation.convertPosition(
        hitResult.worldCoordinates,
        from: nil
    )

    let (minVec, maxVec) = pedalNode.boundingBox

    let topY: CGFloat = 0
    let bottomY: CGFloat = minVec.y - maxVec.y
    let height: CGFloat = max(topY - bottomY, 0.0001)
    let clampedY: CGFloat = Swift.min(
        Swift.max(localPoint.y, bottomY),
        topY
    )

    let rawNormalizedFromTop: CGFloat = (topY - clampedY) / height

    // 命中的有效可踩区域通常比理论 boundingBox 短，
    let calibratedNormalizedFromTop: CGFloat = Swift.min(
        Swift.max((rawNormalizedFromTop - 0.02) / 0.42, 0),
        1
    )

    let velocity: CGFloat = 12 + calibratedNormalizedFromTop * 115

    return Int(velocity.rounded())
}

func pressedPedalAngleDegrees(for depth: Int) -> Float {
    let normalized = Float(depth - 12) / Float(127 - 12)
    let curved = pow(max(0, min(1, normalized)), 0.45)
    
    // MARK: 调整踏板最大踩下角
    return 20 + curved * 10
}

func pedalRootNode(from node: SCNNode) -> SCNNode? {
    var current: SCNNode? = node

    while let candidate = current {
        if let name = candidate.name, name.contains("Pedal") {
            return candidate
        }
        current = candidate.parent
    }

    return nil
}

func firstSceneView(in rootView: NSView?) -> SCNView? {
    guard let rootView else { return nil }

    if let scnView = rootView as? SCNView {
        return scnView
    }

    for subview in rootView.subviews {
        if let scnView = firstSceneView(in: subview) {
            return scnView
        }
    }

    return nil
}

func makeBackBoardNode() -> SCNNode {
    let board = SCNBox(
        width: 12.2,
        height: 5.6,
        length: 0.45,
        chamferRadius: 0.08
    )

    let material = SCNMaterial()
    material.lightingModel = .physicallyBased
    material.diffuse.contents = NSColor(
        calibratedRed: 0.62,
        green: 0.12,
        blue: 0.12,
        alpha: 1
    )
    material.roughness.contents = 0.72
    material.metalness.contents = 0.0
    board.materials = [material]

    let node = SCNNode(geometry: board)
    node.position = SCNVector3(0, 0.1, 0)
    return node
}

func makeFrontFrameBoardNode() -> SCNNode {
    let outerWidth: CGFloat = 12.2
    let outerHeight: CGFloat = 5.6
    let thickness: CGFloat = 2.45
    let outerCornerRadius: CGFloat = 0.36

    let outerRect = NSBezierPath(
        roundedRect: NSRect(
            x: -outerWidth / 2,
            y: -outerHeight / 2,
            width: outerWidth,
            height: outerHeight
        ),
        xRadius: outerCornerRadius,
        yRadius: outerCornerRadius
    )

    // 每个小框都可以单独设置
    let opening1 = NSRect(
        x: -3.2,
        y: 0,
        width: 1.1,
        height: 0.5
    )
    let opening2 = NSRect(x: -1.39, y: 0.1, width: 1.1, height: 0.5)
    let opening3 = NSRect(x: 0.32, y: 0.1, width: 1.1, height: 0.5)
    let opening4 = NSRect(x: 2, y: 0, width: 1.1, height: 0.5)

    outerRect.appendRect(opening1)
    outerRect.appendRect(opening2)
    outerRect.appendRect(opening3)
    outerRect.appendRect(opening4)

    outerRect.windingRule = .evenOdd

    let frame = SCNShape(path: outerRect, extrusionDepth: thickness)
    frame.chamferRadius = 0.08

    let material = SCNMaterial()
    material.lightingModel = .physicallyBased
    material.diffuse.contents = NSColor(
        calibratedWhite: 0.07,
        alpha: 1
    )
    material.roughness.contents = 0.58
    material.metalness.contents = 0.08
    frame.materials = [material]

    let node = SCNNode(geometry: frame)
    node.position = SCNVector3(0, 0.1, 0)
    node.pivot = SCNMatrix4MakeTranslation(0, 0, thickness / 2)
    return node
}

func makeSupportRodNode(
    color: NSColor = NSColor(
        calibratedWhite: 0.82,
        alpha: 1
    ),
    roughness: CGFloat = 0.12,
    metalness: CGFloat = 1.0
) -> SCNNode {
    let rodRadius: CGFloat = 0.08
    let rodHeight: CGFloat = 7.4

    let cylinder = SCNCylinder(
        radius: rodRadius,
        height: rodHeight
    )

    let material = SCNMaterial()
    material.lightingModel = .physicallyBased
    material.diffuse.contents = color
    material.roughness.contents = roughness
    material.metalness.contents = metalness
    material.specular.contents = NSColor.white
    cylinder.materials = [material]

    let node = SCNNode(geometry: cylinder)
    node.eulerAngles = SCNVector3(0, 0, 0)
    return node
}

func makePedalNode(
    contour: [CGPoint],
    initialEulerAngles: SCNVector3 = SCNVector3(0, 0, -Double.pi / 41),
    baseColor: NSColor = NSColor(
        calibratedRed: 190 / 255,
        green: 155 / 255,
        blue: 97 / 255,
        alpha: 1
    ),
    pedal_index: Int
) -> SCNNode {
    let geometry = bulgedExtrusionGeometry(
        contour: contour,
        depth: 15,
        bulge: 0.13,
        slices: 48
    )

    let mat = SCNMaterial()
    mat.lightingModel = .physicallyBased

    mat.diffuse.contents = baseColor

    // 金属度 踏板一般不是纯金属吧，含黄铜 有时候有绿色也有可能
    mat.metalness.contents = 0.957

    // 抛光程度（越低越亮）
    mat.roughness.contents = 0.08

    // 提升高光表现
    mat.specular.contents = NSColor.white

    mat.isDoubleSided = true
    geometry.materials = [mat]

    let node = SCNNode(geometry: geometry)
    let (minVec, maxVec) = node.boundingBox
    let centerX = (minVec.x + maxVec.x) / 2
    let centerZ = (minVec.z + maxVec.z) / 2
    let topY = maxVec.y

    node.pivot = SCNMatrix4MakeTranslation(centerX, topY, centerZ)
    node.scale = SCNVector3(0.03, 0.03, 0.03)

    node.eulerAngles = initialEulerAngles
    node.setValue(
        NSNumber(value: Float(initialEulerAngles.x)),
        forKey: "restingEulerX"
    )
    node.setValue(false as NSNumber, forKey: "isPressed")
    node.setValue(
        NSNumber(value: pedal_index),
        forKey: "pedalIndex"
    )

    return node
}

func makeCameraNode() -> SCNNode {

    // 相机本体 给 SCNNode 挂上相机组件
    let cameraNode = SCNNode()
    let camera = SCNCamera()

    // 定义可见深度区间
    camera.zNear = 0.1
    camera.zFar = 100

    // 视场角（FOV）
    // 小 FOV（35°）长焦镜头
    // 大 FOV（70°）广角
    camera.fieldOfView = 35

    // 挂载
    cameraNode.camera = camera

    // 相机位置
    cameraNode.position = SCNVector3(0, 0, 30)

    // 视线方向 相机看向什么位置
    cameraNode.look(at: SCNVector3(0, 1.5, 0))
    return cameraNode
}

func pedalTopPath() -> NSBezierPath {
    let path = NSBezierPath()
    let points = pedalContourPoints()
    guard let first = points.first else { return path }

    path.move(to: first)
    for p in points.dropFirst() {
        path.line(to: p)
    }
    path.close()
    return path
}

func pedalContourPoints() -> [CGPoint] {
    var path = ContourBuilder()

    path.move(to: CGPoint(x: 0, y: 60))
    path.line(to: CGPoint(x: 26, y: 62))  // 轻微的倾斜
    path.line(to: CGPoint(x: 26, y: 0))

    path.addQuadCurve(
        to: CGPoint(x: 20, y: -45.0),
        control: CGPoint(x: 24.5, y: -30.0),
        steps: 38
    )

    path.addQuadCurve(
        to: CGPoint(x: 15, y: -55.0),
        control: CGPoint(x: 19, y: -50.0),
        steps: 38
    )

    path.addQuadCurve(
        to: CGPoint(x: 0, y: -60.0),
        control: CGPoint(x: 7.5, y: -62.0),
        steps: 38
    )

    // 用更顺手的贝塞尔方式继续拟合，而不是手工塞一长串点
    path.addQuadCurve(
        to: CGPoint(x: -23.0, y: -35.0),
        control: CGPoint(x: -23.5, y: -55.0),
        steps: 38
    )

    path.addQuadCurve(
        to: CGPoint(x: -21.0, y: -25),
        control: CGPoint(x: -23.5, y: -33.0),
        steps: 38
    )

    path.addCurve(
        to: CGPoint(x: -5, y: 5),
        control1: CGPoint(x: -15, y: -15),
        control2: CGPoint(x: -8, y: -2),
        steps: 38
    )

    path.addQuadCurve(
        to: CGPoint(x: 0, y: 55),
        control: CGPoint(x: 2, y: 35),
        steps: 38
    )

    return path.points
}

func pedalContourPointsSecond() -> [CGPoint] {
    var path = ContourBuilder()

    path.move(to: CGPoint(x: 0, y: 60))
    path.line(to: CGPoint(x: 26, y: 60))  // 轻微的倾斜
    path.line(to: CGPoint(x: 26, y: -40))

    path.addQuadCurve(
        to: CGPoint(x: 18, y: -55.0),
        control: CGPoint(x: 24.5, y: -50.0),
        steps: 38
    )

    path.addQuadCurve(
        to: CGPoint(x: -5, y: -55.0),
        control: CGPoint(x: 7.5, y: -62.0),
        steps: 38
    )

    path.addQuadCurve(
        to: CGPoint(x: -15, y: -35.0),
        control: CGPoint(x: -14, y: -50.0),
        steps: 38
    )

    path.addQuadCurve(
        to: CGPoint(x: -5, y: -15.0),
        control: CGPoint(x: -14, y: -30.0),
        steps: 38
    )

    path.addQuadCurve(
        to: CGPoint(x: -2, y: 0),
        control: CGPoint(x: -3, y: -10),
        steps: 38
    )

    return path.points
}

func mirroredContourPoints(_ points: [CGPoint]) -> [CGPoint] {
    Array(points.map { CGPoint(x: -$0.x, y: $0.y) }.reversed())
}

struct ContourBuilder {
    private(set) var points: [CGPoint] = []
    private var currentPoint: CGPoint?

    mutating func move(to point: CGPoint) {
        points = [point]
        currentPoint = point
    }

    mutating func line(to point: CGPoint) {
        guard currentPoint != nil else {
            move(to: point)
            return
        }
        points.append(point)
        currentPoint = point
    }

    mutating func addQuadCurve(
        to end: CGPoint,
        control: CGPoint,
        steps: Int = 16
    ) {
        guard let start = currentPoint else {
            move(to: end)
            return
        }

        let segmentSteps = max(2, steps)
        for step in 1...segmentSteps {
            let t = CGFloat(step) / CGFloat(segmentSteps)
            let mt = 1 - t
            let point = CGPoint(
                x: mt * mt * start.x
                    + 2 * mt * t * control.x
                    + t * t * end.x,
                y: mt * mt * start.y
                    + 2 * mt * t * control.y
                    + t * t * end.y
            )
            points.append(point)
        }

        currentPoint = end
    }

    mutating func addCurve(
        to end: CGPoint,
        control1: CGPoint,
        control2: CGPoint,
        steps: Int = 24
    ) {
        guard let start = currentPoint else {
            move(to: end)
            return
        }

        let segmentSteps = max(2, steps)
        for step in 1...segmentSteps {
            let t = CGFloat(step) / CGFloat(segmentSteps)
            let mt = 1 - t
            let point = CGPoint(
                x: mt * mt * mt * start.x
                    + 3 * mt * mt * t * control1.x
                    + 3 * mt * t * t * control2.x
                    + t * t * t * end.x,
                y: mt * mt * mt * start.y
                    + 3 * mt * mt * t * control1.y
                    + 3 * mt * t * t * control2.y
                    + t * t * t * end.y
            )
            points.append(point)
        }

        currentPoint = end
    }
}

func bulgedExtrusionGeometry(
    contour: [CGPoint],
    depth: CGFloat,
    bulge: CGFloat,
    slices: Int
) -> SCNGeometry {
    var contour = contour
    if polygonSignedArea(contour) < 0 {
        contour.reverse()
    }

    let ringCount = max(2, slices + 1)
    let vertexCountPerRing = contour.count
    guard vertexCountPerRing >= 3 else { return SCNGeometry() }
    let center = polygonCentroid(contour)

    var vertices: [SIMD3<Float>] = []
    vertices.reserveCapacity(ringCount * vertexCountPerRing)

    for slice in 0..<ringCount {
        let t = Float(slice) / Float(ringCount - 1)
        let z = (t - 0.5) * Float(depth)

        // 最关键的边缘算法， 一个方式改变侧边形态问题
        let s = sin(t * .pi)
        let profile = sqrt(s)

        let scale = 1.0 + Float(bulge) * profile

        for p in contour {
            let dx = Float(p.x - center.x)
            let dy = Float(p.y - center.y)
            let x = Float(center.x) + dx * scale
            let y = Float(center.y) + dy * scale
            vertices.append(SIMD3<Float>(x, y, z))
        }
    }

    var indices: [UInt32] = []

    for slice in 0..<(ringCount - 1) {
        let base0 = slice * vertexCountPerRing
        let base1 = (slice + 1) * vertexCountPerRing

        for i in 0..<vertexCountPerRing {
            let next = (i + 1) % vertexCountPerRing
            let a = UInt32(base0 + i)
            let b = UInt32(base0 + next)
            let c = UInt32(base1 + i)
            let d = UInt32(base1 + next)

            indices.append(a)
            indices.append(b)
            indices.append(c)

            indices.append(b)
            indices.append(d)
            indices.append(c)
        }
    }

    let capTriangles = triangulatePolygon(contour)

    // 盖面使用独立顶点，避免和侧面共用法线后产生“顶面微微鼓起”的视觉假象
    let frontCapBase = vertices.count
    for p in contour {
        vertices.append(
            SIMD3<Float>(Float(p.x), Float(p.y), -Float(depth) * 0.5)
        )
    }

    let backCapBase = vertices.count
    for p in contour {
        vertices.append(
            SIMD3<Float>(Float(p.x), Float(p.y), Float(depth) * 0.5)
        )
    }

    for tri in capTriangles {
        indices.append(UInt32(frontCapBase + tri.0))
        indices.append(UInt32(frontCapBase + tri.2))
        indices.append(UInt32(frontCapBase + tri.1))
    }

    for tri in capTriangles {
        indices.append(UInt32(backCapBase + tri.0))
        indices.append(UInt32(backCapBase + tri.1))
        indices.append(UInt32(backCapBase + tri.2))
    }

    let vertexData = Data(
        bytes: vertices,
        count: vertices.count * MemoryLayout<SIMD3<Float>>.stride
    )
    let normals = makeNormals(
        vertices: vertices,
        indices: indices,
        frontCapRange: frontCapBase..<(frontCapBase + vertexCountPerRing),
        backCapRange: backCapBase..<(backCapBase + vertexCountPerRing)
    )
    let normalData = Data(
        bytes: normals,
        count: normals.count * MemoryLayout<SIMD3<Float>>.stride
    )

    let vertexSource = SCNGeometrySource(
        data: vertexData,
        semantic: .vertex,
        vectorCount: vertices.count,
        usesFloatComponents: true,
        componentsPerVector: 3,
        bytesPerComponent: MemoryLayout<Float>.stride,
        dataOffset: 0,
        dataStride: MemoryLayout<SIMD3<Float>>.stride
    )

    let normalSource = SCNGeometrySource(
        data: normalData,
        semantic: .normal,
        vectorCount: vertices.count,
        usesFloatComponents: true,
        componentsPerVector: 3,
        bytesPerComponent: MemoryLayout<Float>.stride,
        dataOffset: 0,
        dataStride: MemoryLayout<SIMD3<Float>>.stride
    )

    let indexData = Data(
        bytes: indices,
        count: indices.count * MemoryLayout<UInt32>.stride
    )
    let element = SCNGeometryElement(
        data: indexData,
        primitiveType: .triangles,
        primitiveCount: indices.count / 3,
        bytesPerIndex: MemoryLayout<UInt32>.stride
    )

    return SCNGeometry(
        sources: [vertexSource, normalSource],
        elements: [element]
    )
}

func makeNormals(
    vertices: [SIMD3<Float>],
    indices: [UInt32],
    frontCapRange: Range<Int>,
    backCapRange: Range<Int>
) -> [SIMD3<Float>] {
    var normals = Array(
        repeating: SIMD3<Float>(0, 0, 0),
        count: vertices.count
    )

    var i = 0
    while i < indices.count {
        let ia = Int(indices[i])
        let ib = Int(indices[i + 1])
        let ic = Int(indices[i + 2])

        let a = vertices[ia]
        let b = vertices[ib]
        let c = vertices[ic]

        let ab = b - a
        let ac = c - a
        let n = simd_cross(ab, ac)

        normals[ia] += n
        normals[ib] += n
        normals[ic] += n

        i += 3
    }

    for idx in frontCapRange {
        normals[idx] = SIMD3<Float>(0, 0, -1)
    }

    for idx in backCapRange {
        normals[idx] = SIMD3<Float>(0, 0, 1)
    }

    return normals.map { normal in
        let len = simd_length(normal)
        if len < 0.0001 { return SIMD3<Float>(0, 1, 0) }
        return normal / len
    }
}

func polygonSignedArea(_ polygon: [CGPoint]) -> CGFloat {
    guard polygon.count > 2 else { return 0 }
    var area: CGFloat = 0
    for i in 0..<polygon.count {
        let p0 = polygon[i]
        let p1 = polygon[(i + 1) % polygon.count]
        area += (p0.x * p1.y) - (p1.x * p0.y)
    }
    return area * 0.5
}

func polygonCentroid(_ polygon: [CGPoint]) -> CGPoint {
    let area = polygonSignedArea(polygon)
    if abs(area) < 0.0001 {
        let sx = polygon.reduce(CGFloat(0)) { $0 + $1.x }
        let sy = polygon.reduce(CGFloat(0)) { $0 + $1.y }
        return CGPoint(
            x: sx / CGFloat(max(polygon.count, 1)),
            y: sy / CGFloat(max(polygon.count, 1))
        )
    }

    var cx: CGFloat = 0
    var cy: CGFloat = 0
    for i in 0..<polygon.count {
        let p0 = polygon[i]
        let p1 = polygon[(i + 1) % polygon.count]
        let factor = (p0.x * p1.y) - (p1.x * p0.y)
        cx += (p0.x + p1.x) * factor
        cy += (p0.y + p1.y) * factor
    }
    let scale = 1 / (6 * area)
    return CGPoint(x: cx * scale, y: cy * scale)
}

func triangulatePolygon(_ polygon: [CGPoint]) -> [(Int, Int, Int)] {
    guard polygon.count >= 3 else { return [] }

    var remaining = Array(0..<polygon.count)
    var triangles: [(Int, Int, Int)] = []
    let isCCW = polygonSignedArea(polygon) > 0

    func cross(_ a: CGPoint, _ b: CGPoint, _ c: CGPoint) -> CGFloat {
        (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x)
    }

    func pointInTriangle(
        _ p: CGPoint,
        _ a: CGPoint,
        _ b: CGPoint,
        _ c: CGPoint
    ) -> Bool {
        let c1 = cross(a, b, p)
        let c2 = cross(b, c, p)
        let c3 = cross(c, a, p)
        if isCCW {
            return c1 >= 0 && c2 >= 0 && c3 >= 0
        } else {
            return c1 <= 0 && c2 <= 0 && c3 <= 0
        }
    }

    var guardCount = 0
    while remaining.count > 3 && guardCount < 10_000 {
        var earFound = false

        for i in 0..<remaining.count {
            let prev = remaining[
                (i - 1 + remaining.count) % remaining.count
            ]
            let curr = remaining[i]
            let next = remaining[(i + 1) % remaining.count]

            let a = polygon[prev]
            let b = polygon[curr]
            let c = polygon[next]
            let turn = cross(a, b, c)

            if isCCW ? (turn <= 0) : (turn >= 0) {
                continue
            }

            var containsOtherPoint = false
            for candidate in remaining
            where candidate != prev && candidate != curr
                && candidate != next
            {
                if pointInTriangle(polygon[candidate], a, b, c) {
                    containsOtherPoint = true
                    break
                }
            }

            if containsOtherPoint {
                continue
            }

            triangles.append((prev, curr, next))
            remaining.remove(at: i)
            earFound = true
            break
        }

        if !earFound {
            break
        }

        guardCount += 1
    }

    if remaining.count == 3 {
        triangles.append((remaining[0], remaining[1], remaining[2]))
    }

    return triangles
}

struct ClickableSceneView: NSViewRepresentable {
    let scene: SCNScene
    let pointOfView: SCNNode
    let onNodeClick: (SCNHitTestResult) -> Void

    func makeCoordinator() -> Coordinator {
        Coordinator(onNodeClick: onNodeClick)
    }

    func makeNSView(context: Context) -> SCNView {
        let scnView = SCNView()
        scnView.scene = scene
        scnView.pointOfView = pointOfView
        scnView.allowsCameraControl = false
        scnView.backgroundColor = .clear
        scnView.autoenablesDefaultLighting = false
        scnView.rendersContinuously = true

        let clickGesture = NSClickGestureRecognizer(
            target: context.coordinator,
            action: #selector(Coordinator.handleClick(_:))
        )
        scnView.addGestureRecognizer(clickGesture)
        context.coordinator.sceneView = scnView

        return scnView
    }

    func updateNSView(_ nsView: SCNView, context: Context) {
        nsView.scene = scene
        nsView.pointOfView = pointOfView
        context.coordinator.onNodeClick = onNodeClick
        context.coordinator.sceneView = nsView
    }

    final class Coordinator: NSObject {
        var onNodeClick: (SCNHitTestResult) -> Void
        weak var sceneView: SCNView?

        init(onNodeClick: @escaping (SCNHitTestResult) -> Void) {
            self.onNodeClick = onNodeClick
        }

        @objc func handleClick(_ gesture: NSClickGestureRecognizer) {
            guard let sceneView = sceneView else { return }

            let location = gesture.location(in: sceneView)
            let hitResults = sceneView.hitTest(location, options: nil)

            guard let firstHit = hitResults.first else { return }
            onNodeClick(firstHit)
        }
    }
}

#Preview {
    PedalView()
}
