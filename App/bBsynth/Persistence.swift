import Combine
import CoreMIDI
import Foundation

// This file is an existing bBsynth-only compilation boundary in the Xcode
// project. The types remain separated by responsibility so they can move into
// dedicated groups later without changing their public interfaces.

// MARK: - Parameter Model

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
            guard minimum > 0 else { return (clamped - minimum) / (maximum - minimum) }
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
            guard minimum > 0 else { return minimum + amount * (maximum - minimum) }
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

    var midiCC: UInt8? { MIDIParameterMapper.controlChange(for: id) }
}

@MainActor
final class SynthParameter: ObservableObject, Identifiable {
    let definition: ParameterDefinition
    private let onChange: (SynthParameter) -> Void

    @Published private(set) var currentValue: Double

    init(definition: ParameterDefinition, onChange: @escaping (SynthParameter) -> Void) {
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
        mappingCurve.normalized(value: currentValue, minimum: minimumValue, maximum: maximumValue)
    }

    var midiValue: UInt8 {
        UInt8((normalizedValue * 127).rounded().clamped(to: 0...127))
    }

    var displayValue: String {
        if case .discrete(let options) = type {
            let index = Int(currentValue.rounded()).clamped(to: 0...max(options.count - 1, 0))
            return options.isEmpty ? "—" : options[index]
        }
        if case .toggle(let off, let on) = type {
            return currentValue >= 0.5 ? on : off
        }
        switch definition.displayUnit {
        case .hertz:
            if currentValue >= 1_000 {
                return currentValue >= 10_000
                    ? String(format: "%.1f kHz", currentValue / 1_000)
                    : String(format: "%.2f kHz", currentValue / 1_000)
            }
            return currentValue < 1
                ? String(format: "%.2f Hz", currentValue)
                : String(format: "%.1f Hz", currentValue)
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
        guard Swift.abs(sanitized - currentValue) > 0.000_000_1 else { return }
        currentValue = sanitized
        if sendMIDI { onChange(self) }
    }

    func setNormalizedValue(_ value: Double, sendMIDI: Bool = true) {
        setValue(
            mappingCurve.value(normalized: value, minimum: minimumValue, maximum: maximumValue),
            sendMIDI: sendMIDI
        )
    }

    func resetToDefault() {
        setValue(defaultValue)
    }
}

// MARK: - Parameter Catalog

enum ParameterCatalog {
    static let definitions: [ParameterDefinition] = [
        discrete(.osc1Waveform, "WAVEFORM", ["Sine", "Triangle", "Saw", "Square", "Pulse"], 2),
        continuous(.osc1Tune, "TUNE", -24, 24, 0, .linear, .semitones),
        continuous(.osc1FineTune, "FINE", -100, 100, 0, .linear, .cents),
        continuous(.osc1Level, "LEVEL", 0, 100, 80, .linear, .percent),
        discrete(.osc2Waveform, "WAVEFORM", ["Sine", "Triangle", "Saw", "Square", "Pulse"], 1),
        continuous(.osc2Tune, "TUNE", -24, 24, 0, .linear, .semitones),
        continuous(.osc2FineTune, "FINE", -100, 100, 0, .linear, .cents),
        continuous(.osc2Level, "LEVEL", 0, 100, 65, .linear, .percent),
        toggle(.oscillatorSync, "HARD SYNC", "OFF", "ON", false),
        continuous(.pulseWidth, "PULSE WIDTH", 0, 100, 50, .linear, .percent),
        continuous(.mixerOsc1Level, "OSC 1", 0, 100, 80, .linear, .percent),
        continuous(.mixerOsc2Level, "OSC 2", 0, 100, 65, .linear, .percent),
        continuous(.mixerNoiseLevel, "NOISE", 0, 100, 0, .linear, .percent),
        discrete(.filterType, "FILTER MODE", ["LP", "HP", "BP", "Notch"], 0),
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
        discrete(.lfoWaveform, "WAVEFORM", ["Sine", "Triangle", "Square", "Random"], 0),
        discrete(.lfoDestination, "DESTINATION", ["Pitch", "Filter", "Amplitude"], 1),
        discrete(.voicePolyphony, "POLYPHONY", ["1", "2", "4", "8", "16"], 3),
        discrete(.voiceUnison, "UNISON", ["1", "2", "4", "8"], 0),
        continuous(.voiceDetune, "DETUNE", 0, 100, 8, .linear, .cents),
        continuous(.voiceStereoSpread, "STEREO", 0, 100, 35, .linear, .percent),
    ]

    private static func continuous(
        _ id: ParameterID, _ name: String, _ minimum: Double, _ maximum: Double,
        _ defaultValue: Double, _ curve: MappingCurve, _ unit: ParameterDisplayUnit
    ) -> ParameterDefinition {
        ParameterDefinition(
            id: id, name: name, type: .continuous, minimumValue: minimum,
            maximumValue: maximum, defaultValue: defaultValue,
            mappingCurve: curve, displayUnit: unit
        )
    }

    private static func discrete(
        _ id: ParameterID, _ name: String, _ options: [String], _ defaultIndex: Int
    ) -> ParameterDefinition {
        ParameterDefinition(
            id: id, name: name, type: .discrete(options: options), minimumValue: 0,
            maximumValue: Double(max(options.count - 1, 0)), defaultValue: Double(defaultIndex),
            mappingCurve: .linear, displayUnit: .plain
        )
    }

    private static func toggle(
        _ id: ParameterID, _ name: String, _ off: String, _ on: String, _ defaultOn: Bool
    ) -> ParameterDefinition {
        ParameterDefinition(
            id: id, name: name, type: .toggle(off: off, on: on), minimumValue: 0,
            maximumValue: 1, defaultValue: defaultOn ? 1 : 0,
            mappingCurve: .linear, displayUnit: .plain
        )
    }
}

@MainActor
final class ParameterStore {
    private var parameters: [ParameterID: SynthParameter] = [:]

    init(onChange: @escaping (SynthParameter) -> Void) {
        for definition in ParameterCatalog.definitions {
            parameters[definition.id] = SynthParameter(definition: definition, onChange: onChange)
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

// MARK: - Central MIDI Mapping

enum MIDIParameterMapper {
    private static let controlChanges: [ParameterID: UInt8] = [
        .osc1Waveform: 20, .osc1Tune: 21, .osc1FineTune: 22, .osc1Level: 23,
        .osc2Waveform: 24, .osc2Tune: 25, .osc2FineTune: 26, .osc2Level: 27,
        .oscillatorSync: 28, .pulseWidth: 29,
        .mixerOsc1Level: 30, .mixerOsc2Level: 31, .mixerNoiseLevel: 32,
        .filterType: 70, .filterCutoff: 74, .filterResonance: 71,
        .filterDrive: 33, .filterKeyboardTracking: 34, .filterEnvelopeAmount: 35,
        .ampAttack: 73, .ampDecay: 75, .ampSustain: 36, .ampRelease: 72,
        .filterAttack: 37, .filterDecay: 38, .filterSustain: 39, .filterRelease: 40,
        .lfoRate: 76, .lfoAmount: 77, .lfoWaveform: 41, .lfoDestination: 42,
        .voicePolyphony: 43, .voiceUnison: 44, .voiceDetune: 45, .voiceStereoSpread: 46,
    ]

    static func controlChange(for parameterID: ParameterID) -> UInt8? {
        controlChanges[parameterID]
    }

    static var mappingTable: [(parameter: ParameterID, cc: UInt8)] {
        controlChanges.map { ($0.key, $0.value) }.sorted { $0.cc < $1.cc }
    }
}

// MARK: - CoreMIDI Virtual Output

@MainActor
final class MIDIManager: ObservableObject {
    enum State: Equatable {
        case starting
        case online
        case failed(OSStatus)

        var label: String {
            switch self {
            case .starting: return "STARTING"
            case .online: return "MIDI ONLINE"
            case .failed(let status): return "MIDI ERROR \(status)"
            }
        }
    }

    static let virtualOutputName = "bBsynth MIDI Out"

    @Published private(set) var state: State = .starting
    @Published private(set) var lastMessage = "Ready"

    private var client = MIDIClientRef()
    private var virtualSource = MIDIEndpointRef()

    init() {
        createVirtualOutput()
    }

    deinit {
        if virtualSource != 0 { MIDIEndpointDispose(virtualSource) }
        if client != 0 { MIDIClientDispose(client) }
    }

    func sendControlChange(controller: UInt8, value: UInt8, channel: UInt8 = 0) {
        send(
            [0xB0 | (channel & 0x0F), controller & 0x7F, value & 0x7F],
            description: "CC \(controller)  •  \(value)"
        )
    }

    func sendNoteOn(note: UInt8, velocity: UInt8, channel: UInt8 = 0) {
        send(
            [0x90 | (channel & 0x0F), note & 0x7F, velocity & 0x7F],
            description: "NOTE ON \(note)  •  \(velocity)"
        )
    }

    func sendNoteOff(note: UInt8, velocity: UInt8 = 0, channel: UInt8 = 0) {
        send(
            [0x80 | (channel & 0x0F), note & 0x7F, velocity & 0x7F],
            description: "NOTE OFF \(note)"
        )
    }

    func sendProgramChange(program: UInt8, channel: UInt8 = 0) {
        send(
            [0xC0 | (channel & 0x0F), program & 0x7F],
            description: "PROGRAM \(program)"
        )
    }

    private func createVirtualOutput() {
        let clientStatus = MIDIClientCreate(
            "bBsynth MIDI Client" as CFString, nil, nil, &client
        )
        guard clientStatus == noErr else {
            state = .failed(clientStatus)
            return
        }

        let sourceStatus = MIDISourceCreate(
            client, Self.virtualOutputName as CFString, &virtualSource
        )
        guard sourceStatus == noErr else {
            state = .failed(sourceStatus)
            return
        }

        MIDIObjectSetStringProperty(virtualSource, kMIDIPropertyManufacturer, "bBsynth" as CFString)
        MIDIObjectSetStringProperty(
            virtualSource, kMIDIPropertyModel, "Analog Control Surface" as CFString
        )
        state = .online
        lastMessage = "Virtual source published"
    }

    private func send(_ bytes: [UInt8], description: String) {
        guard state == .online, virtualSource != 0 else { return }
        var packetList = MIDIPacketList()
        let status: OSStatus = bytes.withUnsafeBufferPointer { buffer in
            let packet = MIDIPacketListInit(&packetList)
            guard let baseAddress = buffer.baseAddress else { return OSStatus(-50) }
            MIDIPacketListAdd(
                &packetList, MemoryLayout<MIDIPacketList>.size, packet,
                0, buffer.count, baseAddress
            )
            return MIDIReceived(virtualSource, &packetList)
        }
        if status == noErr {
            lastMessage = description
        } else {
            state = .failed(status)
        }
    }
}

// MARK: - Controller / Future DSP Boundary

@MainActor
final class SynthController: ObservableObject {
    let midiManager: MIDIManager
    let parameters: ParameterStore

    init() {
        let midiManager = MIDIManager()
        self.midiManager = midiManager
        parameters = ParameterStore { [weak midiManager] parameter in
            guard let cc = parameter.midiCC else { return }
            midiManager?.sendControlChange(controller: cc, value: parameter.midiValue)
        }
    }

    func resetPatch() {
        parameters.resetAll()
    }

    func sendNoteOn(note: UInt8, velocity: UInt8) {
        midiManager.sendNoteOn(note: note, velocity: velocity)
    }

    func sendNoteOff(note: UInt8, velocity: UInt8 = 0) {
        midiManager.sendNoteOff(note: note, velocity: velocity)
    }

    func sendProgramChange(_ program: UInt8) {
        midiManager.sendProgramChange(program: program)
    }
}

extension Comparable {
    fileprivate func clamped(to range: ClosedRange<Self>) -> Self {
        min(max(self, range.lowerBound), range.upperBound)
    }
}
