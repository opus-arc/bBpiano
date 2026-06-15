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


