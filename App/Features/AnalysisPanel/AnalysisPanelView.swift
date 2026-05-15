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
            
            MainCurvesView()
                .position(x: -50, y: 545)
                .scaleEffect(0.6)
            
            PedalView()
                .position(x: 130, y: 340)
                .frame(width: 260, height: 400)

        }
        .frame(width: 880, height: 560)
    }

}



#Preview {
    AnalysisPanelView()
//    ContentView()
}
