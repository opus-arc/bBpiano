//
//  TransportControls.swift
//  bBpiano
//
//  Created by opus arc on 2026/5/16.
//

import SwiftUI

enum TransportAction {
    case rewind
    case fastForward
    case stop
    case record
    case loop
    case play

    var symbolName: String {
        switch self {
        case .rewind:
            return "backward.fill"
        case .fastForward:
            return "forward.fill"
        case .stop:
            return "stop.fill"
        case .record:
            return "circle.fill"
        case .loop:
            return "repeat"
        case .play:
            return "play.fill"
        }
    }
}

struct TransportControls: View {
    var onAction: (TransportAction) -> Void = { _ in }

    private let actions: [TransportAction] = [
        .rewind,
        .fastForward,
        .stop,
        .record,
        .loop,
        .play
    ]

    var body: some View {
        HStack(spacing: 4) {
            ForEach(actions, id: \.self) { action in
                TransportButton(action: action) {
                    onAction(action)
                }
            }
        }
        .controlSize(.mini)
    }
}

private struct TransportButton: View {
    let action: TransportAction
    let perform: () -> Void

    var body: some View {
        Button(action: perform) {
            Image(systemName: action.symbolName)
                .font(.system(size: 11.5, weight: .semibold))
                .frame(width: 18, height: 18)
        }
//        .glassEffect(.regular)
//        .glassEffect(.clear)
//        .glassEffect(.thick)
        .tint(action == .record ? .red : .primary)
        .focusEffectDisabled()
    }
}

#Preview {
    TransportControls()
        .padding()
}
