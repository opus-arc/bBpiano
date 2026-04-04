//
//  ContentView.swift
//  blueBird
//
//  Created by opus arc on 2026/3/29.
//
//  This document is not AI-assisted.
//

/**
 
 默认的 主界面视图文件，通常是用户进入 App 后首先看到的界面。
     •    定义首页 UI
     •    组织按钮、文本、列表、导航等界面元素
     •    调用 ViewModel 或 Model 展示数据
     •    响应用户交互
 
 */

import SwiftUI

struct ContentView: View {
    var body: some View {
        VStack(spacing: 0) {
            AnalysisPanelView()
            PianoPanelView()
        }
        .frame(width: 880, height: 660)
        .background(Color.white)
    }
}

#Preview {
    ContentView()
}

