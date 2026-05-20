////  blueBirdApp.swift
////  blueBird
////
////  Created by opus arc on 2026/3/29.
////
////  This document is not AI-assisted.
////
//
//
//import SwiftUI
//import AppKit
//
//@main
//struct blueBirdApp: App {
//    @NSApplicationDelegateAdaptor(AppDelegate.self) private var appDelegate
//    var body: some Scene {
//        WindowGroup {
//            ZStack {
////                Image("油画背景1")
////                    .resizable()
////                    .scaledToFill()
////                    .frame(width: 880, height: 695)
////                    .clipped()
////                    .overlay(
////                        LinearGradient(
////                            gradient: Gradient(colors: [
////                                Color.clear,
////                                Color.clear,
////
////                                Color.white.opacity(0.15),
////                                Color.white.opacity(0.45),
////                                Color.white
////                            ]),
////                            startPoint: .center,
////                            endPoint: .bottom
////                        )
////                    )
//
//                RootView()
//            }
//            .frame(width: 880, height: 695)
//            .preferredColorScheme(.light)
//        }
//        .windowResizability(.contentSize)
//    }
//
//}
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
////        bBpiano_shutdown()
//        print("bBpiano shut down")
//
//        return .terminateNow
//    }
//}
//
//struct RootView: View {
//    @State private var didStartAudio = false
//
//    var body: some View {
//        ContentView()
//            .task {
//                guard !didStartAudio else { return }
//                didStartAudio = true
//
//                do {
//                    try SoundCard.shared.start()
//                    print("Sound card started")
////                    bBpiano_init()
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
