import Combine
import CoreMIDI
import Foundation

@MainActor
final class MIDIManager: ObservableObject {
    enum State: Equatable {
        case starting
        case online
        case failed(OSStatus)

        var label: String {
            switch self {
            case .starting:
                return "STARTING"
            case .online:
                return "MIDI ONLINE"
            case .failed(let status):
                return "MIDI ERROR \(status)"
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
        if virtualSource != 0 {
            MIDIEndpointDispose(virtualSource)
        }
        if client != 0 {
            MIDIClientDispose(client)
        }
    }

    func sendControlChange(controller: UInt8, value: UInt8, channel: UInt8 = 0) {
        send([
            0xB0 | (channel & 0x0F),
            controller & 0x7F,
            value & 0x7F,
        ], description: "CC \(controller)  •  \(value)")
    }

    /// Reserved for the future keyboard/UI and synth-engine bridge.
    func sendNoteOn(note: UInt8, velocity: UInt8, channel: UInt8 = 0) {
        send([
            0x90 | (channel & 0x0F),
            note & 0x7F,
            velocity & 0x7F,
        ], description: "NOTE ON \(note)  •  \(velocity)")
    }

    /// A zero-velocity note-on is intentionally not substituted, so consumers
    /// can distinguish the explicit MIDI Note Off message.
    func sendNoteOff(note: UInt8, velocity: UInt8 = 0, channel: UInt8 = 0) {
        send([
            0x80 | (channel & 0x0F),
            note & 0x7F,
            velocity & 0x7F,
        ], description: "NOTE OFF \(note)")
    }

    /// Reserved for patch management. Program numbers are zero based in MIDI.
    func sendProgramChange(program: UInt8, channel: UInt8 = 0) {
        send([
            0xC0 | (channel & 0x0F),
            program & 0x7F,
        ], description: "PROGRAM \(program)")
    }

    private func createVirtualOutput() {
        let clientStatus = MIDIClientCreate(
            "bBsynth MIDI Client" as CFString,
            nil,
            nil,
            &client
        )
        guard clientStatus == noErr else {
            state = .failed(clientStatus)
            return
        }

        let sourceStatus = MIDISourceCreate(
            client,
            Self.virtualOutputName as CFString,
            &virtualSource
        )
        guard sourceStatus == noErr else {
            state = .failed(sourceStatus)
            return
        }

        MIDIObjectSetStringProperty(
            virtualSource,
            kMIDIPropertyManufacturer,
            "bBsynth" as CFString
        )
        MIDIObjectSetStringProperty(
            virtualSource,
            kMIDIPropertyModel,
            "Analog Control Surface" as CFString
        )

        state = .online
        lastMessage = "Virtual source published"
    }

    private func send(_ bytes: [UInt8], description: String) {
        guard state == .online, virtualSource != 0 else { return }

        var packetList = MIDIPacketList()
        let status: OSStatus = bytes.withUnsafeBufferPointer { buffer in
            let packet = MIDIPacketListInit(&packetList)
            guard let baseAddress = buffer.baseAddress,
                  MIDIPacketListAdd(
                    &packetList,
                    MemoryLayout<MIDIPacketList>.size,
                    packet,
                    0,
                    buffer.count,
                    baseAddress
                  ) != nil
            else {
                return kMIDIInvalidData
            }
            return MIDIReceived(virtualSource, &packetList)
        }

        if status == noErr {
            lastMessage = description
        } else {
            state = .failed(status)
        }
    }
}
