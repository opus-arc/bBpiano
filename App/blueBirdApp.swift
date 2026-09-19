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

