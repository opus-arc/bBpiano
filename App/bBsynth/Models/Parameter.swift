import Foundation

enum ParameterID: String, CaseIterable, Identifiable {
    case osc1Waveform = "oscillator.1.waveform"
    case osc1Tune = "oscillator.1.tune"
    case osc1FineTune = "oscillator.1.fineTune"
    case osc1Level = "oscillator.1.level"
    case osc2Waveform = "oscillator.2.waveform"
    case osc2Tune = "oscillator.2.tune"
    case osc2FineTune = "oscillator.2.fineTune"
    case osc2Level = "oscillator.2.level"
    case oscillatorSync = "oscillator.sync"
    case pulseWidth = "oscillator.pulseWidth"

    case mixerOsc1Level = "mixer.oscillator1"
    case mixerOsc2Level = "mixer.oscillator2"
    case mixerNoiseLevel = "mixer.noise"

    case filterType = "filter.type"
    case filterCutoff = "filter.cutoff"
    case filterResonance = "filter.resonance"
    case filterDrive = "filter.drive"
    case filterKeyboardTracking = "filter.keyboardTracking"
    case filterEnvelopeAmount = "filter.envelopeAmount"

    case ampAttack = "envelope.amp.attack"
    case ampDecay = "envelope.amp.decay"
    case ampSustain = "envelope.amp.sustain"
    case ampRelease = "envelope.amp.release"

    case filterAttack = "envelope.filter.attack"
    case filterDecay = "envelope.filter.decay"
    case filterSustain = "envelope.filter.sustain"
    case filterRelease = "envelope.filter.release"

    case lfoRate = "lfo.rate"
    case lfoAmount = "lfo.amount"
    case lfoWaveform = "lfo.waveform"
    case lfoDestination = "lfo.destination"

    case voicePolyphony = "voice.polyphony"
    case voiceUnison = "voice.unison"
    case voiceDetune = "voice.detune"
    case voiceStereoSpread = "voice.stereoSpread"

    var id: String { rawValue }
}

enum ParameterType {
    case continuous
    case discrete(options: [String])
    case toggle(off: String, on: String)
}

enum MappingCurve {
    case linear
    case logarithmic
    case exponential(exponent: Double)

    func normalized(value: Double, minimum: Double, maximum: Double) -> Double {
        guard maximum > minimum else { return 0 }
        let clamped = min(max(value, minimum), maximum)

        switch self {
        case .linear:
            return (clamped - minimum) / (maximum - minimum)
        case .logarithmic:
            guard minimum > 0 else {
                return (clamped - minimum) / (maximum - minimum)
            }
            return log(clamped / minimum) / log(maximum / minimum)
        case .exponential(let exponent):
            let linear = (clamped - minimum) / (maximum - minimum)
            return pow(linear, 1 / max(exponent, 0.001))
        }
    }

    func value(normalized: Double, minimum: Double, maximum: Double) -> Double {
        let amount = min(max(normalized, 0), 1)

        switch self {
        case .linear:
            return minimum + amount * (maximum - minimum)
        case .logarithmic:
            guard minimum > 0 else {
                return minimum + amount * (maximum - minimum)
            }
            return minimum * pow(maximum / minimum, amount)
        case .exponential(let exponent):
            return minimum + pow(amount, max(exponent, 0.001)) * (maximum - minimum)
        }
    }
}

enum ParameterDisplayUnit {
    case hertz
    case percent
    case semitones
    case cents
    case envelopeTime
    case plain
}

struct ParameterDefinition {
    let id: ParameterID
    let name: String
    let type: ParameterType
    let minimumValue: Double
    let maximumValue: Double
    let defaultValue: Double
    let mappingCurve: MappingCurve
    let displayUnit: ParameterDisplayUnit

    var midiCC: UInt8? {
        MIDIParameterMapper.controlChange(for: id)
    }
}
