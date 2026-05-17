////  blueBirdApp.swift
////  blueBird
////
////  Created by opus arc on 2026/3/29.
////
////  This document is not AI-assisted.
////
//
//import SwiftUI
//
//@main
//struct blueBirdApp: App {
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
//        }
//        .windowResizability(.contentSize)
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
//                } catch {
//                    print("Failed to start sound card")
//                    print("Error:", error)
//                    let nsError = error as NSError
//                    print("Domain:", nsError.domain)
//                    print("Code:", nsError.code)
//                    print("UserInfo:", nsError.userInfo)
//                }
//            }
//    }
//}
