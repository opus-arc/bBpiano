//
//  TransportPanelView.swift
//  bBpiano
//
//  Created by opus arc on 2026/5/16.
//

import SwiftUI

struct TransportPanelView: View {
    var body: some View {
        HStack(spacing: 8) {
            PerformanceMeterPanel()
                .position(x: 30, y: 17.5)
            Spacer(minLength: 0)
            TransportControls()
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

