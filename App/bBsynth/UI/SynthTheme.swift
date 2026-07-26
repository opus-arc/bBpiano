import SwiftUI

enum SynthTheme {
    static let panelBlack = Color(red: 0.055, green: 0.058, blue: 0.055)
    static let panelRaised = Color(red: 0.105, green: 0.108, blue: 0.098)
    static let panelEdge = Color(red: 0.28, green: 0.27, blue: 0.23)
    static let cream = Color(red: 0.90, green: 0.85, blue: 0.69)
    static let mutedCream = Color(red: 0.62, green: 0.60, blue: 0.50)
    static let amber = Color(red: 0.93, green: 0.49, blue: 0.15)
    static let red = Color(red: 0.70, green: 0.12, blue: 0.08)
    static let teal = Color(red: 0.18, green: 0.52, blue: 0.48)
    static let walnut = Color(red: 0.23, green: 0.095, blue: 0.035)

    static let panelGradient = LinearGradient(
        colors: [
            Color(red: 0.12, green: 0.12, blue: 0.108),
            panelBlack,
            Color(red: 0.085, green: 0.088, blue: 0.082),
        ],
        startPoint: .topLeading,
        endPoint: .bottomTrailing
    )

    static let cabinetGradient = LinearGradient(
        colors: [
            Color(red: 0.34, green: 0.15, blue: 0.055),
            walnut,
            Color(red: 0.12, green: 0.04, blue: 0.015),
        ],
        startPoint: .top,
        endPoint: .bottom
    )
}

struct EngravedLabel: ViewModifier {
    let size: CGFloat
    let color: Color

    func body(content: Content) -> some View {
        content
            .font(.system(size: size, weight: .bold, design: .rounded))
            .tracking(size * 0.12)
            .foregroundStyle(color)
    }
}

extension View {
    func engraved(size: CGFloat = 10, color: Color = SynthTheme.cream) -> some View {
        modifier(EngravedLabel(size: size, color: color))
    }
}
