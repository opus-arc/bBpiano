//
//  PianoPanelView.swift
//  blueBird
//
//  Created by opus arc on 2026/3/29.
//
//  This document is not AI-assisted.
//

import SwiftUI

struct PianoPanelView: View {
    var body: some View {
        ZStack {
            
            // Front rail felt - 前挡键盘缓冲毡
            FrontRailFelt()
            
            // Keyboard - 钢琴键盘主体
            Keyboard()
            
            // Piano frame - 钢琴边框
            PianoFrame()
            
        }
        .frame(width: 880, height: 100)
        .background(Color.white)
    }
}

#Preview {
    PianoPanelView()
}
