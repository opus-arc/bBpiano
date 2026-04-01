//
//  FrontRailFelt.swift
//  blueBird
//
//  Created by opus arc on 2026/3/29.
//

import SwiftUI

struct FrontRailFelt: View {
    var body: some View {
        ZStack {
            
            // Front rail felt - 前挡键盘缓冲毡
            
            // 吸收冲击，降低机械噪音，防尘，保护结构，提升触键的柔和感与回弹品质的毛毡
            // 沿用了 Steinway & Sons、Yamaha 的深红混色的传统
            
            LinearGradient(
                colors: [
                    Color(red: 77/255, green: 10/255, blue: 5/255),
                    Color(red: 77/255, green: 10/255, blue: 8/255),
                    Color(red: 67/255, green: 12/255, blue: 6/255)
                ],
                startPoint: .top,
                endPoint: .bottom
            )
            
        }
        .frame(width: 880, height: 100)
    }

}
