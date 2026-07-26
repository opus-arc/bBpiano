import Foundation

enum ParameterCatalog {
    static let definitions: [ParameterDefinition] = [
        discrete(.osc1Waveform, "WAVEFORM", ["Sine", "Triangle", "Saw", "Square", "Pulse"], defaultIndex: 2),
        continuous(.osc1Tune, "TUNE", -24, 24, 0, .linear, .semitones),
        continuous(.osc1FineTune, "FINE", -100, 100, 0, .linear, .cents),
        continuous(.osc1Level, "LEVEL", 0, 100, 80, .linear, .percent),

        discrete(.osc2Waveform, "WAVEFORM", ["Sine", "Triangle", "Saw", "Square", "Pulse"], defaultIndex: 1),
        continuous(.osc2Tune, "TUNE", -24, 24, 0, .linear, .semitones),
        continuous(.osc2FineTune, "FINE", -100, 100, 0, .linear, .cents),
        continuous(.osc2Level, "LEVEL", 0, 100, 65, .linear, .percent),
        toggle(.oscillatorSync, "HARD SYNC", off: "OFF", on: "ON", defaultOn: false),
        continuous(.pulseWidth, "PULSE WIDTH", 0, 100, 50, .linear, .percent),

        continuous(.mixerOsc1Level, "OSC 1", 0, 100, 80, .linear, .percent),
        continuous(.mixerOsc2Level, "OSC 2", 0, 100, 65, .linear, .percent),
        continuous(.mixerNoiseLevel, "NOISE", 0, 100, 0, .linear, .percent),

        discrete(.filterType, "FILTER MODE", ["LP", "HP", "BP", "Notch"], defaultIndex: 0),
        continuous(.filterCutoff, "CUTOFF", 20, 20_000, 3_000, .logarithmic, .hertz),
        continuous(.filterResonance, "RESONANCE", 0, 100, 25, .linear, .percent),
        continuous(.filterDrive, "DRIVE", 0, 100, 5, .linear, .percent),
        continuous(.filterKeyboardTracking, "KEY TRACK", 0, 100, 50, .linear, .percent),
        continuous(.filterEnvelopeAmount, "ENV AMOUNT", -100, 100, 45, .linear, .percent),

        continuous(.ampAttack, "ATTACK", 0.001, 10, 0.015, .exponential(exponent: 3), .envelopeTime),
        continuous(.ampDecay, "DECAY", 0.001, 10, 0.35, .exponential(exponent: 3), .envelopeTime),
        continuous(.ampSustain, "SUSTAIN", 0, 100, 75, .linear, .percent),
        continuous(.ampRelease, "RELEASE", 0.001, 20, 0.8, .exponential(exponent: 3), .envelopeTime),

        continuous(.filterAttack, "ATTACK", 0.001, 10, 0.01, .exponential(exponent: 3), .envelopeTime),
        continuous(.filterDecay, "DECAY", 0.001, 10, 0.6, .exponential(exponent: 3), .envelopeTime),
        continuous(.filterSustain, "SUSTAIN", 0, 100, 45, .linear, .percent),
        continuous(.filterRelease, "RELEASE", 0.001, 20, 1.2, .exponential(exponent: 3), .envelopeTime),

        continuous(.lfoRate, "RATE", 0.05, 20, 1.2, .logarithmic, .hertz),
        continuous(.lfoAmount, "AMOUNT", 0, 100, 0, .linear, .percent),
        discrete(.lfoWaveform, "WAVEFORM", ["Sine", "Triangle", "Square", "Random"], defaultIndex: 0),
        discrete(.lfoDestination, "DESTINATION", ["Pitch", "Filter", "Amplitude"], defaultIndex: 1),

        discrete(.voicePolyphony, "POLYPHONY", ["1", "2", "4", "8", "16"], defaultIndex: 3),
        discrete(.voiceUnison, "UNISON", ["1", "2", "4", "8"], defaultIndex: 0),
        continuous(.voiceDetune, "DETUNE", 0, 100, 8, .linear, .cents),
        continuous(.voiceStereoSpread, "STEREO", 0, 100, 35, .linear, .percent),
    ]

    private static func continuous(
        _ id: ParameterID,
        _ name: String,
        _ minimum: Double,
        _ maximum: Double,
        _ defaultValue: Double,
        _ curve: MappingCurve,
        _ unit: ParameterDisplayUnit
    ) -> ParameterDefinition {
        ParameterDefinition(
            id: id,
            name: name,
            type: .continuous,
            minimumValue: minimum,
            maximumValue: maximum,
            defaultValue: defaultValue,
            mappingCurve: curve,
            displayUnit: unit
        )
    }

    private static func discrete(
        _ id: ParameterID,
        _ name: String,
        _ options: [String],
        defaultIndex: Int
    ) -> ParameterDefinition {
        ParameterDefinition(
            id: id,
            name: name,
            type: .discrete(options: options),
            minimumValue: 0,
            maximumValue: Double(max(options.count - 1, 0)),
            defaultValue: Double(defaultIndex),
            mappingCurve: .linear,
            displayUnit: .plain
        )
    }

    private static func toggle(
        _ id: ParameterID,
        _ name: String,
        off: String,
        on: String,
        defaultOn: Bool
    ) -> ParameterDefinition {
        ParameterDefinition(
            id: id,
            name: name,
            type: .toggle(off: off, on: on),
            minimumValue: 0,
            maximumValue: 1,
            defaultValue: defaultOn ? 1 : 0,
            mappingCurve: .linear,
            displayUnit: .plain
        )
    }
}

@MainActor
final class ParameterStore {
    private var parameters: [ParameterID: SynthParameter] = [:]

    init(onChange: @escaping (SynthParameter) -> Void) {
        for definition in ParameterCatalog.definitions {
            parameters[definition.id] = SynthParameter(
                definition: definition,
                onChange: onChange
            )
        }
    }

    func parameter(_ id: ParameterID) -> SynthParameter {
        guard let parameter = parameters[id] else {
            preconditionFailure("Missing parameter definition for \(id.rawValue)")
        }
        return parameter
    }

    var allParameters: [SynthParameter] {
        ParameterID.allCases.compactMap { parameters[$0] }
    }

    func resetAll() {
        allParameters.forEach { $0.resetToDefault() }
    }
}
