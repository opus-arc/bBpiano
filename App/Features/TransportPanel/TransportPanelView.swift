//
//  TransportPanelView.swift
//  bBpiano
//
//  Created by opus arc on 2026/5/16.
//

import SwiftUI

struct TransportPanelView: View {
    @State private var hammerFEnabled = false
    
    var body: some View {
        HStack(spacing: 10) {
            PerformanceMeterPanel()
                .frame(width: 72, height: 24)
            HammerModeSwitch(isHammerF: $hammerFEnabled)
            Spacer(minLength: 0)
//            TransportControls()
        }
        .frame(height: 35)
        .padding(.horizontal, 6)
        .background(Color.clear)
    }
}

#Preview {
    TransportPanelView()
        .frame(width: 760)
}

private struct HammerModeSwitch: View {
    @Binding var isHammerF: Bool
    
    var body: some View {
        Button {
            isHammerF.toggle()
            set_hammer_mode(isHammerF ? 1 : 0)
        } label: {
            HStack(spacing: 0) {
                label("Normal", active: !isHammerF)
                label("Hammer-F", active: isHammerF)
            }
            .padding(2)
            .background(
                Capsule()
                    .fill(Color.black.opacity(0.06))
            )
            .overlay(
                Capsule()
                    .stroke(Color.black.opacity(0.10), lineWidth: 1)
            )
        }
        .buttonStyle(.plain)
    }
    
    private func label(_ text: String, active: Bool) -> some View {
        Text(text)
            .font(.system(size: 11, weight: active ? .semibold : .regular))
            .foregroundStyle(active ? Color.white : Color.black.opacity(0.58))
            .frame(width: 66, height: 22)
            .background(
                Capsule()
                    .fill(active ? Color.black.opacity(0.82) : Color.clear)
            )
    }
}
