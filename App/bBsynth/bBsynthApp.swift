import SwiftUI

@main
struct bBsynthApp: App {
    @StateObject private var controller = SynthController()

    var body: some Scene {
        WindowGroup {
            ContentView()
                .environmentObject(controller)
                .frame(minWidth: 1_200, minHeight: 760)
        }
        .defaultSize(width: 1_470, height: 950)
        .windowResizability(.contentMinSize)
    }
}
