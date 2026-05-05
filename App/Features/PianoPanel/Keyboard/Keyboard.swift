//
//  Keyboard.swift
//  blueBird
//
//  Created by opus arc on 2026/3/29.
//
//  AI-assisted.
//

import SwiftUI
import CoreGraphics

struct Keyboard: View {
    
    @State private var lastPointerLocation: CGPoint?
    
    private let whiteKeyWidth: CGFloat = 15
    private let whiteKeyHeight: CGFloat = 88
    private let blackKeyWidth: CGFloat = 12
    private let blackKeyHeight: CGFloat = 58

    private let unavailableElements = Color(red: 142/255,green: 142/255,blue: 147/255)
    private let pressedKeyLightOverlay = Color(red: 244/255, green: 160/255, blue: 160/255)
    private let pressedKeyDarkOverlay = Color(red: 140/255, green: 18/255, blue: 12/255)
    private let extraKeysEnabled = false
    private let releaseDelayNanoseconds: UInt64 = 35_000_000
    
    private let whiteKeyColor = Color(red: 240 / 255, green: 240 / 255, blue: 240 / 255)
    private let blackKeyColor = Color(red: 25 / 255, green: 25 / 255, blue: 25 / 255)
    private let whiteBorderColor = Color(red: 182/255, green: 182/255, blue: 178/255)

    private let whiteNotes: [Int] = Array(21...108).filter { midi in
        !Self.blackPitchClasses.contains(midi % 12)
    }

    private let blackNotes: [BlackKeyLayout] = Keyboard.makeBlackKeyLayouts(
        whiteKeyWidth: 15,
        blackKeyWidth: 12
    )

    private let extraWhiteMIDINotes: [Int] = [109, 111]

    private var extraBlackNotes: [BlackKeyLayout] {
        [
            BlackKeyLayout(
                midi: 110,
                x: CGFloat(whiteNotes.count + 1) * whiteKeyWidth - blackKeyWidth / 2
            )
        ]
    }

    @State private var activePointerMIDINote: Int?
    @State private var activePointerVelocity: Int?
    @State private var highlightedMIDIVelocities: [Int: Int] = [:]
    @State private var pendingReleaseTokens: [Int: UUID] = [:]
    @State private var isPointerTracking = false
    
    var body: some View {
        ZStack(alignment: .topLeading) {
            HStack(spacing: 0) {
                ForEach(Array(whiteNotes.enumerated()), id: \.element) { _, midi in
                    WhiteKeyView(
                        midi: midi,
                        width: whiteKeyWidth,
                        height: whiteKeyHeight,
                        fill: whiteKeyColor,
                        border: whiteBorderColor,
                        pressedOverlay: pressedOverlay(for: midi),
                        isHighlighted: isHighlighted(midi)
                    )
                }
                ForEach(extraWhiteMIDINotes, id: \.self) { midi in
                    WhiteKeyView(
                        midi: midi,
                        width: whiteKeyWidth,
                        height: whiteKeyHeight,
                        fill: whiteKeyColor,
                        border: whiteBorderColor,
                        pressedOverlay: pressedOverlay(for: midi),
                        isHighlighted: isHighlighted(midi)
                    )
                    .modifier(ExtraKeyAvailabilityModifier(
                        isEnabled: extraKeysEnabled,
                        unavailableOverlay: unavailableElements
                    ))
                }
            }

            ForEach(blackNotes + extraBlackNotes) { item in
                BlackKeyView(
                    midi: item.midi,
                    width: blackKeyWidth,
                    height: blackKeyHeight,
                    fill: blackKeyColor,
                    pressedOverlay: pressedOverlay(for: item.midi),
                    isHighlighted: isHighlighted(item.midi)
                )
                .modifier(ExtraKeyAvailabilityModifier(
                    isEnabled: isExtraKey(item.midi) ? extraKeysEnabled : true,
                    unavailableOverlay: unavailableElements
                ))
                .offset(x: item.x, y: 0)
            }
        }
        .contentShape(Rectangle())
        .gesture(keyboardPointerGesture)
        .frame(width: 833, height: 90, alignment: .topLeading)
        .clipped()
        .offset(x: 15, y: 0)
    }
}

private extension Keyboard {
    static let blackPitchClasses: Set<Int> = [1, 3, 6, 8, 10]

    var allWhiteMIDINotes: [Int] {
        whiteNotes + extraWhiteMIDINotes
    }

    var allExtraMIDINotes: Set<Int> {
        Set(extraWhiteMIDINotes + extraBlackNotes.map(\.midi))
    }

    func isExtraKey(_ midi: Int) -> Bool {
        allExtraMIDINotes.contains(midi)
    }

    func isHighlighted(_ midi: Int) -> Bool {
        highlightedMIDIVelocities[midi] != nil
    }

    func velocity(at location: CGPoint) -> Int {
        let effectiveHeight: CGFloat

        if blackMIDINote(atX: location.x, y: location.y) != nil {
            effectiveHeight = blackKeyHeight
        } else {
            effectiveHeight = whiteKeyHeight
        }

        let clampedY = min(max(location.y, 0), effectiveHeight)
        let progress = clampedY / effectiveHeight
        let velocityRange = 127 - 12
        let rawVelocity = 12 + Int(round(progress * CGFloat(velocityRange)))
        return min(max(rawVelocity, 12), 127)
    }

    func currentVelocity(for midi: Int) -> Int? {
        highlightedMIDIVelocities[midi]
    }

    func pressedOverlay(for midi: Int) -> Color {
        guard let velocity = currentVelocity(for: midi) else {
            return pressedKeyLightOverlay
        }

        let normalized = Double(velocity - 12) / Double(127 - 12)
        return Color(
            red: pressedKeyLightOverlayRed + (pressedKeyDarkOverlayRed - pressedKeyLightOverlayRed) * normalized,
            green: pressedKeyLightOverlayGreen + (pressedKeyDarkOverlayGreen - pressedKeyLightOverlayGreen) * normalized,
            blue: pressedKeyLightOverlayBlue + (pressedKeyDarkOverlayBlue - pressedKeyLightOverlayBlue) * normalized
        )
    }

    var pressedKeyLightOverlayRed: Double { 244 / 255 }
    var pressedKeyLightOverlayGreen: Double { 160 / 255 }
    var pressedKeyLightOverlayBlue: Double { 160 / 255 }

    var pressedKeyDarkOverlayRed: Double { 140 / 255 }
    var pressedKeyDarkOverlayGreen: Double { 18 / 255 }
    var pressedKeyDarkOverlayBlue: Double { 12 / 255 }

    var keyboardPointerGesture: some Gesture {
        DragGesture(minimumDistance: 0, coordinateSpace: .local)
            .onChanged { value in
                let midi = midiNote(at: value.location)
                let pressure = velocity(at: value.location)

                if !isPointerTracking {
                    if let midi {
                        cancelPendingReleaseIfNeeded(for: midi)
                    }
                    isPointerTracking = true
                    activePointerMIDINote = midi
                    activePointerVelocity = midi == nil ? nil : pressure
                    lastPointerLocation = value.location

                    if let midi {
                        highlightedMIDIVelocities[midi] = pressure
                        VKController.NoteOn(note: midi, velocity: Double(pressure))
                    }
                    return
                }

                let previousLocation = lastPointerLocation
                lastPointerLocation = value.location

                if midi == activePointerMIDINote {
                    guard let midi else {
                        activePointerVelocity = nil
                        return
                    }

                    let horizontalMovement = previousLocation.map { abs(value.location.x - $0.x) } ?? 0
                    _ = previousLocation
                        .map { abs(value.location.y - $0.y) } ?? 0

                    // 只有横向几乎没动，才认为是同键 pressure / aftertouch
                    if horizontalMovement < 2, activePointerVelocity != pressure {
                        activePointerVelocity = pressure
                        highlightedMIDIVelocities[midi] = pressure
                        VKController.PolyAftertouch(
                            note: midi,
                            pressure: Double(pressure)
                        )
                    }

                    return
                }

                let previousMIDINote = activePointerMIDINote

                if let midi {
                    cancelPendingReleaseIfNeeded(for: midi)
                }

                activePointerMIDINote = midi
                activePointerVelocity = midi == nil ? nil : pressure

                if let midi {
                    highlightedMIDIVelocities[midi] = pressure
                    VKController.NoteOn(note: midi, velocity: Double(pressure))
                }

                scheduleDelayedRelease(for: previousMIDINote)
            }
            .onEnded { _ in
                let releasedMIDINote = activePointerMIDINote

                activePointerMIDINote = nil
                activePointerVelocity = nil
                lastPointerLocation = nil
                isPointerTracking = false
                scheduleDelayedRelease(for: releasedMIDINote)
            }
    }
    
    func cancelPendingReleaseIfNeeded(for midi: Int) {
        pendingReleaseTokens[midi] = nil
    }

    func scheduleDelayedRelease(for midi: Int?) {
        guard let midi else {
            return
        }

        let token = UUID()
        pendingReleaseTokens[midi] = token

        Task { @MainActor in
            try? await Task.sleep(nanoseconds: releaseDelayNanoseconds)

            guard pendingReleaseTokens[midi] == token else {
                return
            }

//            print("midi\(midi) 释放 velocity 0")
            
            VKController.NoteOff(note: midi, velocity: 0)

            highlightedMIDIVelocities[midi] = nil
            pendingReleaseTokens[midi] = nil

            if activePointerMIDINote == midi {
                activePointerMIDINote = nil
                activePointerVelocity = nil
            }
        }
    }

    func midiNote(at location: CGPoint) -> Int? {
        // 手势已经挂在 offset 之前的键盘内容本体上，location 是本地坐标，不能再减去外层的 15。
        let localX = location.x
        let localY = location.y

        guard localX >= 0, localY >= 0, localY <= whiteKeyHeight + 2 else {
            return nil
        }

        let midi: Int?
        if let blackMidi = blackMIDINote(atX: localX, y: localY) {
            midi = blackMidi
        } else {
            midi = whiteMIDINote(atX: localX)
        }

        guard let midi else {
            return nil
        }

        if !extraKeysEnabled && isExtraKey(midi) {
            return nil
        }

        return midi
    }

    func blackMIDINote(atX x: CGFloat, y: CGFloat) -> Int? {
        guard y <= blackKeyHeight else {
            return nil
        }

        for item in (blackNotes + extraBlackNotes).reversed() {
            let minX = item.x
            let maxX = item.x + blackKeyWidth

            if x >= minX && x <= maxX {
                return item.midi
            }
        }

        return nil
    }

    func whiteMIDINote(atX x: CGFloat) -> Int? {
        guard x >= 0 else {
            return nil
        }

        let index = Int(floor(x / whiteKeyWidth))
        guard allWhiteMIDINotes.indices.contains(index) else {
            return nil
        }

        return allWhiteMIDINotes[index]
    }

    static func blackKeyCenterShift(for pitchClass: Int) -> CGFloat {
        switch pitchClass {
        case 1:
            // C# 稍向左，和 D# 拉开一点
            return -0.5
        case 3:
            // D# 稍向右，和 C# 拉开一点
            return 0.5
        case 6:
            // F# 稍向左
            return -1.0
        case 8:
            // G# 保持居中
            return 0.0
        case 10:
            // A# 稍向右
            return 1.0
        default:
            return 0.0
        }
    }

    static func makeBlackKeyLayouts(
        whiteKeyWidth: CGFloat,
        blackKeyWidth: CGFloat
    ) -> [BlackKeyLayout] {
        let midiNotes = Array(21...108)
        let whiteNotes = midiNotes.filter { !blackPitchClasses.contains($0 % 12) }

        var whiteIndexByMIDINote: [Int: Int] = [:]
        for (index, midi) in whiteNotes.enumerated() {
            whiteIndexByMIDINote[midi] = index
        }

        return midiNotes.compactMap { midi in
            guard blackPitchClasses.contains(midi % 12) else {
                return nil
            }

            let previousWhiteMIDINote = midi - 1
            guard let previousWhiteIndex = whiteIndexByMIDINote[previousWhiteMIDINote] else {
                return nil
            }

            let centeredX = CGFloat(previousWhiteIndex + 1) * whiteKeyWidth - blackKeyWidth / 2
            let x = centeredX + blackKeyCenterShift(for: midi % 12)
            return BlackKeyLayout(midi: midi, x: x)
        }
    }
}

private struct WhiteKeyView: View {
    let midi: Int
    let width: CGFloat
    let height: CGFloat
    let fill: Color
    let border: Color
    let pressedOverlay: Color
    let isHighlighted: Bool
    private let pressedOverlayOpacity: Double = 0.92

    private let leftBorder = Color(red: 200 / 255, green: 200 / 255, blue: 200 / 255)
    private let rightBorder = Color(red: 180 / 255, green: 180 / 255, blue: 180 / 255)
    private let seamColor = Color(red: 105 / 255, green: 105 / 255, blue: 100 / 255)
    private let seamWidth: CGFloat = 0.6
    private let topHighlightColor = Color(red: 116 / 255, green: 116 / 255, blue: 116 / 255)
    private let pressedExtensionHeight: CGFloat = 2

    var body: some View {
        ZStack(alignment: .topLeading) {
            // 真正的键体比 frame 稍窄，右侧留出一条缝隙
            WhiteKeyShape(cut: 1.5)
                .fill(
                    LinearGradient(
                        stops: [
                            .init(color: Color(red: 112 / 255, green: 112 / 255, blue: 112 / 255), location: 0.0),
                            .init(color: Color(red: 132 / 255, green: 132 / 255, blue: 132 / 255), location: 0.04),
                            .init(color: Color(red: 154 / 255, green: 154 / 255, blue: 154 / 255), location: 0.10),
                            .init(color: Color(red: 184 / 255, green: 184 / 255, blue: 184 / 255), location: 0.18),
                            .init(color: Color(red: 218 / 255, green: 218 / 255, blue: 218 / 255), location: 0.28),
                            .init(color: fill, location: 0.38)
                        ],
                        startPoint: .top,
                        endPoint: .bottom
                    )
                )
                .frame(width: width - seamWidth, height: height)
                .overlay {
                    WhiteKeyTopBottomBorder(
                        shape: WhiteKeyShape(cut: 1.5),
                        topColor: topHighlightColor,
                        bottomColor: border,
                        height: height
                    )
                }
                .overlay {
                    if isHighlighted {
                        WhiteKeyShape(cut: 1.5)
                            .fill(pressedOverlay.opacity(pressedOverlayOpacity))
                    }
                }
                .overlay(alignment: .leading) {
                    Rectangle()
                        .fill(
                            LinearGradient(
                                stops: [
                                    .init(color: Color(red: 77 / 255, green: 77 / 255, blue: 77 / 255), location: 0),
                                    .init(color: Color(red: 88 / 255, green: 88 / 255, blue: 88 / 255), location: 0.08),
                                    .init(color: Color(red: 106 / 255, green: 106 / 255, blue: 106 / 255), location: 0.18),
                                    .init(color: leftBorder, location: 0.34)
                                ],
                                startPoint: .top,
                                endPoint: .bottom
                            )
                        )
                        .frame(width: 0.6)
                        .padding(.bottom, 1.5)
                }
                .overlay(alignment: .trailing) {
                    Rectangle()
                        .fill(
                            LinearGradient(
                                stops: [
                                    .init(color: Color(red: 98 / 255, green: 98 / 255, blue: 98 / 255), location: 0.0),
                                    .init(color: Color(red: 110 / 255, green: 110 / 255, blue: 110 / 255), location: 0.04),
                                    .init(color: Color(red: 124 / 255, green: 124 / 255, blue: 124 / 255), location: 0.10),
                                    .init(color: Color(red: 140 / 255, green: 140 / 255, blue: 140 / 255), location: 0.18),
                                    .init(color: Color(red: 156 / 255, green: 156 / 255, blue: 156 / 255), location: 0.28),
                                    .init(color: rightBorder, location: 0.40),
                                    .init(color: rightBorder, location: 1.0)
                                ],
                                startPoint: .top,
                                endPoint: .bottom
                            )
                        )
                        .frame(width: 0.6)
                        .padding(.bottom, 1.5)
                        
                }

            // 键与键之间真正可见的细缝
            Rectangle()
                .fill(seamColor)
                .frame(width: seamWidth)
                .frame(maxWidth: .infinity, alignment: .trailing)

            if isHighlighted {
                Rectangle()
                    .fill(pressedOverlay.opacity(pressedOverlayOpacity))
                    .frame(width: width - seamWidth, height: pressedExtensionHeight)
                    .offset(y: height)
            }
        }
        .frame(width: width, height: height + pressedExtensionHeight, alignment: .topLeading)
    }
}


private struct WhiteKeyTopBottomBorder<S: Shape>: View {
    let shape: S
    let topColor: Color
    let bottomColor: Color
    let height: CGFloat

    var body: some View {
        ZStack {
            shape
                .stroke(topColor, lineWidth: 0.6)
                .mask(
                    Rectangle()
                        .frame(height: 1)
                        .frame(maxHeight: .infinity, alignment: .top)
                )

            shape
                .stroke(bottomColor, lineWidth: 0.6)
                .mask(
                    Rectangle()
                        .frame(height: 2)
                        .frame(maxHeight: .infinity, alignment: .bottom)
                )
        }
    }
}

private struct WhiteKeyShape: Shape {
    let cut: CGFloat

    func path(in rect: CGRect) -> Path {
        var path = Path()

        let c = cut

        path.move(to: CGPoint(x: 0, y: 0))
        path.addLine(to: CGPoint(x: rect.width, y: 0))
        path.addLine(to: CGPoint(x: rect.width, y: rect.height - c))
        path.addLine(to: CGPoint(x: rect.width - c, y: rect.height))
        path.addLine(to: CGPoint(x: c, y: rect.height))
        path.addLine(to: CGPoint(x: 0, y: rect.height - c))
        path.closeSubpath()

        return path
    }
}

private struct BlackKeyView: View {
    let midi: Int
    let width: CGFloat
    let height: CGFloat
    let fill: Color
    let pressedOverlay: Color
    let isHighlighted: Bool
    private let pressedOverlayOpacity: Double = 0.88

    private let leftBorder = Color(red: 117 / 255, green: 117 / 255, blue: 117 / 255)
    private let rightBorder = Color(red: 124 / 255, green: 124 / 255, blue: 124 / 255)
    private let whiteKeyColor = Color(red: 240 / 255, green: 240 / 255, blue: 240 / 255)
    private let rightGlowWidth: CGFloat = 5

    private var bodyHeightExcludingBottomBorder: CGFloat {
        height - 1
    }

    private var ridgeStartX: CGFloat {
        floor(width * 0.46)
    }

    private var ridgeWidth: CGFloat {
        max(0, (width - 1) - ridgeStartX)
    }

    private var leftMainWidth: CGFloat {
        ridgeStartX
    }

    private var upperSectionHeight: CGFloat {
        bodyHeightExcludingBottomBorder * 0.55
    }

    private var middleSectionHeight: CGFloat {
        bodyHeightExcludingBottomBorder * 0.35
    }

    private var lowerSectionHeight: CGFloat {
        bodyHeightExcludingBottomBorder * 0.10
    }

    var body: some View {
        ZStack(alignment: .topLeading) {
            BlackKeyShape(cut: 1)
                .fill(Color(red: 16 / 255, green: 16 / 255, blue: 16 / 255))
                .overlay {
                    ZStack(alignment: .topLeading) {
                        // 第一个部分：中轴线偏右的高光斜坡
                        LinearGradient(
                            stops: [
                                .init(color: Color(red: 60 / 255, green: 60 / 255, blue: 60 / 255), location: 0.0),
                                .init(color: Color(red: 60 / 255, green: 60 / 255, blue: 60 / 255), location: 0.36),
                                .init(color: Color(red: 46 / 255, green: 46 / 255, blue: 46 / 255), location: 0.72),
                                .init(color: Color(red: 34 / 255, green: 34 / 255, blue: 34 / 255), location: 1.0)
                            ],
                            startPoint: .leading,
                            endPoint: .trailing
                        )
                        .frame(width: ridgeWidth, height: bodyHeightExcludingBottomBorder)
                        .offset(x: ridgeStartX, y: 0)

                        // 剩余部分的第一段：顶部到 55%
                        Rectangle()
                            .fill(
                                LinearGradient(
                                    stops: [
                                        .init(color: Color(red: 16 / 255, green: 16 / 255, blue: 16 / 255), location: 0.0),
                                        .init(color: Color(red: 18 / 255, green: 18 / 255, blue: 18 / 255), location: 0.18),
                                        .init(color: Color(red: 21 / 255, green: 21 / 255, blue: 21 / 255), location: 0.36),
                                        .init(color: Color(red: 25 / 255, green: 25 / 255, blue: 25 / 255), location: 0.58),
                                        .init(color: Color(red: 30 / 255, green: 30 / 255, blue: 30 / 255), location: 0.80),
                                        .init(color: Color(red: 34 / 255, green: 34 / 255, blue: 34 / 255), location: 1.0)
                                    ],
                                    startPoint: .top,
                                    endPoint: .bottom
                                )
                            )
                            .frame(width: leftMainWidth, height: upperSectionHeight)
                            .offset(x: 0, y: 0)

                        // 第二个部分：55% 到 90%
                        Rectangle()
                            .fill(
                                LinearGradient(
                                    stops: [
                                        .init(color: Color(red: 40 / 255, green: 40 / 255, blue: 40 / 255), location: 0.0),
                                        .init(color: Color(red: 41 / 255, green: 41 / 255, blue: 41 / 255), location: 0.18),
                                        .init(color: Color(red: 42 / 255, green: 42 / 255, blue: 42 / 255), location: 0.38),
                                        .init(color: Color(red: 44 / 255, green: 44 / 255, blue: 44 / 255), location: 0.62),
                                        .init(color: Color(red: 46 / 255, green: 46 / 255, blue: 46 / 255), location: 0.82),
                                        .init(color: Color(red: 48 / 255, green: 48 / 255, blue: 48 / 255), location: 1.0)
                                    ],
                                    startPoint: .top,
                                    endPoint: .bottom
                                )
                            )
                            .frame(width: leftMainWidth, height: middleSectionHeight)
                            .offset(x: 0, y: upperSectionHeight)

                        // 第三个部分：90% 到底部（不包含底边框）
                        Rectangle()
                            .fill(Color(red: 60 / 255, green: 60 / 255, blue: 60 / 255))
                            .frame(width: leftMainWidth, height: lowerSectionHeight)
                            .offset(x: 0, y: upperSectionHeight + middleSectionHeight)
                    }
                    .mask(BlackKeyShape(cut: 1))
                }
                .overlay {
                    if isHighlighted {
                        BlackKeyShape(cut: 1)
                            .fill(pressedOverlay.opacity(pressedOverlayOpacity))
                    }
                }
                .overlay(alignment: .leading) {
                    Rectangle()
                        .fill(
                            LinearGradient(
                                stops: [
                                    .init(color: Color(red: 92 / 255, green: 92 / 255, blue: 92 / 255), location: 0.0),
                                    .init(color: Color(red: 108 / 255, green: 108 / 255, blue: 108 / 255), location: 0.10),
                                    .init(color: leftBorder, location: 0.22),
                                    .init(color: leftBorder, location: 1.0)
                                ],
                                startPoint: .top,
                                endPoint: .bottom
                            )
                        )
                        .frame(width: 1, height: height - 1)
                }
                .overlay(alignment: .trailing) {
                    Rectangle()
                        .fill(
                            LinearGradient(
                                stops: [
                                    .init(color: Color(red: 82 / 255, green: 82 / 255, blue: 82 / 255), location: 0.0),
                                    .init(color: Color(red: 96 / 255, green: 96 / 255, blue: 96 / 255), location: 0.10),
                                    .init(color: Color(red: 108 / 255, green: 108 / 255, blue: 108 / 255), location: 0.20),
                                    .init(color: Color(red: 116 / 255, green: 116 / 255, blue: 116 / 255), location: 0.48),
                                    .init(color: rightBorder, location: 1.0)
                                ],
                                startPoint: .top,
                                endPoint: .bottom
                            )
                        )
                        .frame(width: 1, height: height - 1)
                }
                .frame(width: width, height: height)

            Rectangle()
                .fill(
                    LinearGradient(
                        stops: [
                            .init(color: Color.black.opacity(0.26), location: 0.0),
                            .init(color: Color.black.opacity(0.14), location: 0.42),
                            .init(color: Color.clear, location: 1.0)
                        ],
                        startPoint: .leading,
                        endPoint: .trailing
                    )
                )
                .frame(width: rightGlowWidth, height: height - 1)
                .mask(
                    LinearGradient(
                        stops: [
                            .init(color: .black, location: 0.0),
                            .init(color: Color.black.opacity(0.82), location: 0.18),
                            .init(color: Color.black.opacity(0.42), location: 0.45),
                            .init(color: Color.clear, location: 1.0)
                        ],
                        startPoint: .top,
                        endPoint: .bottom
                    )
                )
                .blendMode(.multiply)
                .offset(x: width, y: 0)
        }
        .frame(width: width + rightGlowWidth, height: height, alignment: .topLeading)
    }
    
}

private struct BlackKeyShape: Shape {
    let cut: CGFloat

    func path(in rect: CGRect) -> Path {
        var path = Path()

        let c = cut

        path.move(to: CGPoint(x: 0, y: 0))
        path.addLine(to: CGPoint(x: rect.width, y: 0))
        path.addLine(to: CGPoint(x: rect.width, y: rect.height - c))
        path.addLine(to: CGPoint(x: rect.width - c, y: rect.height))
        path.addLine(to: CGPoint(x: c, y: rect.height))
        path.addLine(to: CGPoint(x: 0, y: rect.height - c))
        path.closeSubpath()

        return path
    }
}

private struct BlackKeyLayout: Identifiable {
    let midi: Int
    let x: CGFloat

    var id: Int { midi }
}

#Preview {
    
        Keyboard()
    
}

private struct ExtraKeyAvailabilityModifier: ViewModifier {
    let isEnabled: Bool
    let unavailableOverlay: Color

    func body(content: Content) -> some View {
        content
            .overlay {
                if !isEnabled {
                    unavailableOverlay.opacity(0.55)
                }
            }
            .allowsHitTesting(isEnabled)
    }
}
