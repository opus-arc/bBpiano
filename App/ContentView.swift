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

#if DEBUG
import Inject
#endif

import SwiftUI

struct ContentView: View {
    
    #if DEBUG
    @ObserveInjection var inject
    #endif

    var body: some View {
        
    
        
        VStack(spacing: 0) {
            TransportPanelView()
            AnalysisPanelView()
            PianoPanelView()
        }
        .frame(width: 880, height: 695)
        .background(Color.clear)
        
        #if DEBUG
        .enableInjection()
        #endif
    }
}

#Preview {
    ContentView()
}

