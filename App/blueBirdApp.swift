//
//  blueBirdApp.swift
//  blueBird
//
//  Created by opus arc on 2026/3/29.
//
//  This document is not AI-assisted.
//

/**
 
 •    声明这是应用入口
 •    创建主窗口（WindowGroup）
 •    指定应用启动后先显示哪个界面
 •    有时也会在这里注入全局依赖，例如：
 •    数据容器
 •    环境对象 environmentObject
 •    全局配置

 */

import SwiftUI


@main
// 测试阶段 暂时把这个标签留给 Core
struct blueBirdApp: App {
    
    var body: some Scene {
        WindowGroup {
            ContentView()
                .frame(width: 880, height: 660) // 经过测量之后的理想尺寸
        }
        .windowResizability(.contentSize) // 让窗口自适应内容的大小
        
    }
}


//public final class Core {
//    
//    public static let shared = Core()
//
//    public let pianoShared: PianoModel
//
//    private init() {
//        
//        self.pianoShared = PianoModel()
//        // 初始化 cpp 的 piano
//    }
//}



