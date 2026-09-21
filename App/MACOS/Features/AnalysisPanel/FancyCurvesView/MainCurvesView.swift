//
//  MainEditorView.swift
//  bBpiano
//
//  Created by mac coiler on 2026/4/14.
//
//  AI-assisted.
//
import SwiftUI

struct MainCurvesView: View {
    @State private var currentTab: CurveTab = .velocity // 控制当前显示哪个页面
    
    
    var body: some View {
        ZStack {
            
            // 用 Switch 彻底接管渲染逻辑，点击箭头改变 currentTab 时，这里的视图会自动切换
            Group {
                switch currentTab {
                case .velocity:
                    VelocityView(currentTab: $currentTab)
                case .noteOff:
                    NoteOffView(currentTab: $currentTab)
                case .pedal:
                    PedalCurveView(currentTab: $currentTab)
                case .aftertouch:
                    AftertouchView(currentTab: $currentTab)
                }
            }
            // 注意：曲线上的控制点点击范围不在 MainEditorView 中定义；
            // 它应当在 VelocityView / NoteOffView / PedalCurveView / AftertouchView
            // 或它们共用的控制点组件中，通过 contentShape / frame / padding 扩大命中区域。
            // 给切换动作加一个淡入淡出效果
            .transition(.opacity)
        }
    }
}

#Preview("Full Editor System") {
    MainCurvesView()
}
