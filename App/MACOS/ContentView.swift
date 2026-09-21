//
//  ContentView.swift
//  blueBird
//
//  Created by opus arc on 2026/3/29.
//
//  This document is not AI-assisted.
//

import SwiftUI

struct ContentView: View {
    
    var body: some View {
        
        VStack(spacing: 0) {
            TransportPanelView()
            AnalysisPanelView()
            PianoPanelView()
        }
        .frame(width: 880, height: 695)
        .background(Color.clear)
        
    }
}

#Preview {
    ContentView()
}

