//
//  MainEditorView.swift
//  bBpiano
//
//  Created by mac coiler on 2026/4/14.
//
//  AI-assisted.
//
import SwiftUI

struct MainEditorView: View {
    @State private var isDarkMode: Bool = true
    @State private var currentTab: CurveTab = .velocity // 控制当前显示哪个页面
    
    var currentTheme: CurveTheme { isDarkMode ? .dark : .light }
    
    var body: some View {
        ZStack {
            currentTheme.mainBg.edgesIgnoringSafeArea(.all)
            
            // 用 Switch 彻底接管渲染逻辑，点击箭头改变 currentTab 时，这里的视图会自动切换
            Group {
                switch currentTab {
                case .velocity:
                    VelocityView(isDarkMode: $isDarkMode, currentTab: $currentTab)
                case .noteOff:
                    NoteOffView(isDarkMode: $isDarkMode, currentTab: $currentTab)
                case .pedal:
                    PedalCurveView(isDarkMode: $isDarkMode, currentTab: $currentTab)
                case .aftertouch:
                    AftertouchView(isDarkMode: $isDarkMode, currentTab: $currentTab)
                }
            }
            // 给切换动作加一个淡入淡出效果
            .transition(.opacity)
        }
        .animation(.spring(response: 0.35, dampingFraction: 0.8), value: isDarkMode)
    }
}

#Preview("Full Editor System") {
    MainEditorView()
}
