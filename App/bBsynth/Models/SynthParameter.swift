import Combine
import Foundation

@MainActor
final class SynthParameter: ObservableObject, Identifiable {
    let definition: ParameterDefinition
    private let onChange: (SynthParameter) -> Void

    @Published private(set) var currentValue: Double

    init(
        definition: ParameterDefinition,
        onChange: @escaping (SynthParameter) -> Void
    ) {
        self.definition = definition
        currentValue = definition.defaultValue
        self.onChange = onChange
    }

    var id: ParameterID { definition.id }
    var name: String { definition.name }
    var type: ParameterType { definition.type }
    var minimumValue: Double { definition.minimumValue }
    var maximumValue: Double { definition.maximumValue }
    var defaultValue: Double { definition.defaultValue }
    var mappingCurve: MappingCurve { definition.mappingCurve }
    var midiCC: UInt8? { definition.midiCC }

    var normalizedValue: Double {
        mappingCurve.normalized(
            value: currentValue,
            minimum: minimumValue,
            maximum: maximumValue
        )
    }

    var midiValue: UInt8 {
        UInt8((normalizedValue * 127).rounded().clamped(to: 0...127))
    }

    var displayValue: String {
        if case .discrete(let options) = type {
            let index = Int(currentValue.rounded()).clamped(to: 0...(max(options.count - 1, 0)))
            return options.isEmpty ? "—" : options[index]
        }

        if case .toggle(let off, let on) = type {
            return currentValue >= 0.5 ? on : off
        }

        switch definition.displayUnit {
        case .hertz:
            if currentValue >= 1_000 {
                let precision = currentValue >= 10_000 ? 1 : 2
                return String(format: "%.\(precision)f kHz", currentValue / 1_000)
            }
            return currentValue < 1 ? String(format: "%.2f Hz", currentValue) : String(format: "%.1f Hz", currentValue)
        case .percent:
            return String(format: "%.0f%%", currentValue)
        case .semitones:
            return String(format: "%+.1f st", currentValue)
        case .cents:
            return String(format: "%+.0f cent", currentValue)
        case .envelopeTime:
            return currentValue < 1
                ? String(format: "%.0f ms", currentValue * 1_000)
                : String(format: "%.2f s", currentValue)
        case .plain:
            return String(format: "%.0f", currentValue)
        }
    }

    func setValue(_ value: Double, sendMIDI: Bool = true) {
        let sanitized = value.clamped(to: minimumValue...maximumValue)
        guard abs(sanitized - currentValue) > 0.000_000_1 else { return }
        currentValue = sanitized
        if sendMIDI {
            onChange(self)
        }
    }

    func setNormalizedValue(_ value: Double, sendMIDI: Bool = true) {
        let mapped = mappingCurve.value(
            normalized: value,
            minimum: minimumValue,
            maximum: maximumValue
        )
        setValue(mapped, sendMIDI: sendMIDI)
    }

    func resetToDefault() {
        setValue(defaultValue)
    }
}

extension Comparable {
    fileprivate func clamped(to range: ClosedRange<Self>) -> Self {
        min(max(self, range.lowerBound), range.upperBound)
    }
}
