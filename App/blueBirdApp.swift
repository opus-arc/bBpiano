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
    @NSApplicationDelegateAdaptor(AppDelegate.self) private var appDelegate
    

    init() {
        #if DEBUG
        if let path = Bundle.main.path(forResource: "macOSInjection", ofType: "bundle") {
            if Bundle(path: path)?.load() == true {
                print("InjectionIII macOS bundle loaded from app bundle")
            } else {
                print("Failed to load bundled macOSInjection.bundle")
            }
        } else {
            print("macOSInjection.bundle not found in app bundle")
        }
        #endif
    }
    
    
    var body: some Scene {
        WindowGroup {
            ZStack {
                RootView()
            }
            .frame(width: 880, height: 695)
            .preferredColorScheme(.light)
        }
        .windowResizability(.contentSize)
    }

}

final class AppDelegate: NSObject, NSApplicationDelegate {
    private var didShutdownBPiano = false

    func applicationShouldTerminate(_ sender: NSApplication) -> NSApplication.TerminateReply {
        guard !didShutdownBPiano else {
            return .terminateNow
        }

        didShutdownBPiano = true
        bBpiano_shutdown()
        print("bBpiano shut down")

        return .terminateNow
    }
}

struct RootView: View {
    @State private var didStartAudio = false

    private var isRunningTests: Bool {
        ProcessInfo.processInfo.environment["XCTestConfigurationFilePath"] != nil
    }
    
    var body: some View {
        ContentView()
            .task {
                
                
                // XCTest 环境下不启动声卡
                if isRunningTests {
                    print("Running under XCTest, audio engine disabled")
                    return
                }
                
                guard !didStartAudio else { return }
                didStartAudio = true

                do {
                    
                    try SoundCard.shared.start()
                    print("Sound card started")
                    
                    // 
//                    try MidiService.play(
//                        playbackRate: 1,
//                        startTime: 325,
//                        midiFileURL: URL(
//                            filePath: "/Users/opusarc/XCodeProjects/bBpiano/Doc/midi/Chopin - Scherzo No. 3 in C-sharp Minor, Op. 39.mid"
//                        )!
//                    )
//                    
//                    try MidiService.play(
//                        playbackRate: 1,
//                        startTime: 0,
//                        midiFileURL: URL(
//                            filePath: "/Users/opusarc/Projects/XCodeProjects/bBpiano/Doc/midi/Mozart - Sonata in F Major, K332.mid"
//                        )!
//                    )
//                    
//                    try MidiService.play(
//                        playbackRate: 1,
//                        startTime: 0,
//                        midiFileURL: URL(
//                            filePath: "/Users/opusarc/XCodeProjects/bBpiano/Doc/midi/Piano E-Competition MIDI-2017-Wenhao Zhang-Schubert - Impromptu Op. 90 No. 3 in G-flat Major, D. 899.mid"
//                        )!
//                    )
                    

//                    MidiService.stop()
                    
                    
                    bBpiano_init()
                    Thread.sleep(forTimeInterval: 2.0)
                    print("bBpiano inited")
                } catch {
                    print("Failed to start sound card or bBpiano")
                    print("Error:", error)
                    let nsError = error as NSError
                    print("Domain:", nsError.domain)
                    print("Code:", nsError.code)
                    print("UserInfo:", nsError.userInfo)
                }
            }
    }
}
