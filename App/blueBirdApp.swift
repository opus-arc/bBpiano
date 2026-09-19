//  blueBirdApp.swift
//  blueBird
//
//  Created by opus arc on 2026/3/29.
//
//  This document is not AI-assisted.
//


import SwiftUI
import AppKit
internal import System


@main
struct blueBirdApp: App {

    var body: some Scene {
        WindowGroup {
            ZStack {
                ContentView()
            }
            .frame(width: 880, height: 695)
            .preferredColorScheme(.light)
        }
        .windowResizability(.contentSize)
    }

}
//
//final class AppDelegate: NSObject, NSApplicationDelegate {
//    private var didShutdownBPiano = false
//
//    func applicationShouldTerminate(_ sender: NSApplication) -> NSApplication.TerminateReply {
//        guard !didShutdownBPiano else {
//            return .terminateNow
//        }
//
//        didShutdownBPiano = true
//        bBpiano_shutdown()
//        print("bBpiano shut down")
//
//        return .terminateNow
//    }
//}
//
//struct RootView: View {
//    @State private var didStartAudio = false
//
//    private var isRunningTests: Bool {
//        ProcessInfo.processInfo.environment["XCTestConfigurationFilePath"] != nil
//    }
//    
//    var body: some View {
//        ContentView()
//            .task {
//                
//                
////                // XCTest 环境下不启动声卡
////                if isRunningTests {
////                    print("Running under XCTest, audio engine disabled")
////                    return
////                }
////                
////                guard !didStartAudio else { return }
////                didStartAudio = true
//
//                do {
//                    
//                    
//                    
//                    Thread.sleep(forTimeInterval: 2.0)
//                    print("bBpiano inited")
//                } catch {
//                    print("Failed to start sound card or bBpiano")
//                    print("Error:", error)
//                    let nsError = error as NSError
//                    print("Domain:", nsError.domain)
//                    print("Code:", nsError.code)
//                    print("UserInfo:", nsError.userInfo)
//                }
//            }
//    }
//}
