import Foundation

enum MIDIParameterMapper {
    /// The complete MIDI implementation map. UI code never owns CC numbers.
    private static let controlChanges: [ParameterID: UInt8] = [
        .osc1Waveform: 20,
        .osc1Tune: 21,
        .osc1FineTune: 22,
        .osc1Level: 23,
        .osc2Waveform: 24,
        .osc2Tune: 25,
        .osc2FineTune: 26,
        .osc2Level: 27,
        .oscillatorSync: 28,
        .pulseWidth: 29,

        .mixerOsc1Level: 30,
        .mixerOsc2Level: 31,
        .mixerNoiseLevel: 32,

        .filterType: 70,
        .filterCutoff: 74,
        .filterResonance: 71,
        .filterDrive: 33,
        .filterKeyboardTracking: 34,
        .filterEnvelopeAmount: 35,

        .ampAttack: 73,
        .ampDecay: 75,
        .ampSustain: 36,
        .ampRelease: 72,

        .filterAttack: 37,
        .filterDecay: 38,
        .filterSustain: 39,
        .filterRelease: 40,

        .lfoRate: 76,
        .lfoAmount: 77,
        .lfoWaveform: 41,
        .lfoDestination: 42,

        .voicePolyphony: 43,
        .voiceUnison: 44,
        .voiceDetune: 45,
        .voiceStereoSpread: 46,
    ]

    static func controlChange(for parameterID: ParameterID) -> UInt8? {
        controlChanges[parameterID]
    }

    static var mappingTable: [(parameter: ParameterID, cc: UInt8)] {
        controlChanges
            .map { (parameter: $0.key, cc: $0.value) }
            .sorted { $0.cc < $1.cc }
    }
}
