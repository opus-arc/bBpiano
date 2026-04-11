//
//  AnalysisPanelView.swift
//  blueBird
//
//  Created by opus arc on 2026/3/29.
//
//  This document is not AI-assisted.
//

import SwiftUI

struct AnalysisPanelView: View {
    
    
    var body: some View {
        ZStack {
            Color("AccentColor")
            
            SoundWaveView()
                .position(x: 440, y: 100)
                .frame(width: 880, height: 300)
            
//            PedalFrameView()
//                .position(x: 455, y: 510)
            PedalView()
                .position(x: 130, y: 340)
                .frame(width: 260, height: 400)

        }
        .frame(width: 880, height: 560)
    }

}



#Preview {
//    AnalysisPanelView()
    ContentView()
}
