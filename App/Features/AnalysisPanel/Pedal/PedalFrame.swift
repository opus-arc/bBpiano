//
//  PedalFrame.swift
//  blueBird
//
//  Created by opus arc on 2026/4/1.
//
//  This document is not AI-assisted.
//

import SwiftUI

struct PedalFrameView: View {

    var body: some View {
        ZStack {
            pedalHole.position(x: 48, y: 25)
            pedalHole.position(x: 87, y: 25)  // 这里在中间隔开两个像素看着稍微自然一些

            pedalHole.position(x: 124, y: 25)
            pedalHole.position(x: 162, y: 25)

        }
        .frame(width: 280, height: 88, alignment: .topLeading)
    }

    var pedalHole: some View {
        ZStack {
            RoundedRectangle(cornerRadius: 1)
                .stroke(
                    Color(
                        red: 30 / 255,
                        green: 30 / 255,
                        blue: 30 / 255)
                    ,
                    lineWidth: 5
                )
                .fill(Color.black)
                .frame(width: 29.5, height: 22)

            pedalFelt
                .position(x: 10, y: -6.3)
                .frame(width: 20, height: 3.5)

        }
    }
    var pedalFelt: some View {
        Rectangle()
            .fill(
                LinearGradient(
                    colors: [
                        Color(red: 36 / 255, green: 3 / 255, blue: 5 / 255),
                        Color(red: 36 / 255, green: 3 / 255, blue: 3 / 255),
                        Color(red: 36 / 255, green: 3 / 255, blue: 5 / 255),
                    ],
                    startPoint: .top,
                    endPoint: .bottom
                )
            )
            .frame(width: 28, height: 3)
    }
}

#Preview {
    AnalysisPanelView()
}
