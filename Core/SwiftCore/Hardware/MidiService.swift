////
////  MidiService.swift
////  bBpiano
////
////  Created by opus arc on 2026/4/8.
////
////  AI Assisted
////
//
//import Foundation
//import CoreMIDI
//
//public final class MidiService {
//
//    // MARK: - Public
//
//    public init() {}
//
//    deinit {
//        stop()
//    }
//
//    public func start() {
//        guard !isStarted else { return }
//
//        createClientIfNeeded()
//        createInputPortIfNeeded()
//        connectAllSources()
//
//        isStarted = true
//        print("[MidiService] Started.")
//    }
//
//    public func stop() {
//        guard isStarted else { return }
//
//        disconnectAllSources()
//
//        if inputPort != 0 {
//            MIDIPortDispose(inputPort)
//            inputPort = 0
//        }
//
//        if client != 0 {
//            MIDIClientDispose(client)
//            client = 0
//        }
//
//        connectedSourceIDs.removeAll()
//        isStarted = false
//        print("[MidiService] Stopped.")
//    }
//
//    public func refreshConnections() {
//        guard isStarted else { return }
//        disconnectAllSources()
//        connectAllSources()
//        print("[MidiService] Connections refreshed.")
//    }
//
//    public func printAvailableSources() {
//        let count = MIDIGetNumberOfSources()
//        print("[MidiService] Available MIDI Sources: \(count)")
//
//        for index in 0..<count {
//            let source = MIDIGetSource(index)
//            let name = endpointName(for: source) ?? "Unknown Source"
//            print("  [\(index)] \(name)")
//        }
//    }
//
//    // MARK: - Private
//
//    private var client: MIDIClientRef = 0
//    private var inputPort: MIDIPortRef = 0
//    private var isStarted = false
//    private var connectedSourceIDs: Set<MIDIUniqueID> = []
//
//    private func createClientIfNeeded() {
//        guard client == 0 else { return }
//
//        let status = MIDIClientCreateWithBlock("bBpiano.MidiClient" as CFString, &client) { [weak self] notificationPtr in
//            guard let self else { return }
//            self.handleMIDINotification(notificationPtr.pointee)
//        }
//
//        guard status == noErr else {
//            print("[MidiService] Failed to create MIDI client. OSStatus: \(status)")
//            return
//        }
//    }
//
//    private func createInputPortIfNeeded() {
//        guard inputPort == 0 else { return }
//        guard client != 0 else { return }
//
//        let status = MIDIInputPortCreate(
//            client,
//            "bBpiano.InputPort" as CFString,
//            { packetList, _, _ in
//                let packets = packetList.pointee
//                var packet = packets.packet
//
//                for _ in 0..<packets.numPackets {
//                    let status = packet.data.0
//                    let data1 = packet.data.1
//                    let data2 = packet.data.2
//
//                    self.parseChannelVoiceMessage(
//                        status: status,
//                        data1: data1,
//                        data2: data2
//                    )
//
//                    packet = MIDIPacketNext(&packet).pointee
//                }
//            },
//            nil,
//            &inputPort
//        )
//
//        if status != noErr {
//            print("[MidiService] Failed to create MIDI input port. OSStatus: \(status)")
//        }
//    }
//
//    private func connectAllSources() {
//        guard inputPort != 0 else { return }
//
//        let count = MIDIGetNumberOfSources()
//        for index in 0..<count {
//            let source = MIDIGetSource(index)
//
//            var uniqueID: MIDIUniqueID = 0
//            MIDIObjectGetIntegerProperty(source, kMIDIPropertyUniqueID, &uniqueID)
//
//            if connectedSourceIDs.contains(uniqueID) {
//                continue
//            }
//
//            let status = MIDIPortConnectSource(inputPort, source, nil)
//            if status == noErr {
//                connectedSourceIDs.insert(uniqueID)
//                let name = endpointName(for: source) ?? "Unknown Source"
//                print("[MidiService] Connected source: \(name)")
//            } else {
//                print("[MidiService] Failed to connect source[\(index)]. OSStatus: \(status)")
//            }
//        }
//    }
//
//    private func disconnectAllSources() {
//        guard inputPort != 0 else { return }
//
//        let count = MIDIGetNumberOfSources()
//        for index in 0..<count {
//            let source = MIDIGetSource(index)
//            MIDIPortDisconnectSource(inputPort, source)
//        }
//
//        connectedSourceIDs.removeAll()
//    }
//
//    private func endpointName(for endpoint: MIDIEndpointRef) -> String? {
//        var unmanagedName: Unmanaged<CFString>?
//        let status = MIDIObjectGetStringProperty(endpoint, kMIDIPropertyDisplayName, &unmanagedName)
//
//        if status == noErr, let cfName = unmanagedName?.takeRetainedValue() {
//            return cfName as String
//        }
//
//        return nil
//    }
//
//    private func handleMIDINotification(_ notification: MIDINotification) {
//        switch notification.messageID {
//        case .msgObjectAdded, .msgObjectRemoved, .msgSetupChanged:
//            if isStarted {
//                refreshConnections()
//            }
//        default:
//            break
//        }
//    }
//
//    // MARK: - MIDI 1.0 Event Parsing
//
//    private func handleEventList(_ eventListPtr: UnsafePointer<MIDIEventList>) {
//        let eventList = eventListPtr.pointee
//        var packetPtr = UnsafeMutablePointer<MIDIEventPacket>(mutating: &eventList.packet)
//
//        for _ in 0..<eventList.numPackets {
//            let packet = packetPtr.pointee
//            parseWords(packet.words.0, packet.wordCount: Int(packet.wordCount))
//            packetPtr = MIDIEventPacketNext(packetPtr)
//        }
//    }
//
//    private func parseWords(_ firstWord: UInt32, packet.wordCount: Int) {
//        // 这里只取每个 word 的低 3 byte，当作传统 MIDI 1.0 消息处理。
//        // 对多数键盘/控制器足够了；System Exclusive 等复杂消息你后面可自行扩展。
//        var words: [UInt32] = [firstWord]
//
//        if packet.wordCount > 1 {
//            // 当前只先处理首 word 的常见消息。
//            // 如需更完整支持，可把 packet 的全部 words 拿出来继续展开。
//        }
//
//        for word in words {
//            let status = UInt8(word & 0xFF)
//            let data1  = UInt8((word >> 8) & 0xFF)
//            let data2  = UInt8((word >> 16) & 0xFF)
//
//            guard status >= 0x80 else { continue }
//            parseChannelVoiceMessage(status: status, data1: data1, data2: data2)
//        }
//    }
//
//    private func parseChannelVoiceMessage(status: UInt8, data1: UInt8, data2: UInt8) {
//        let messageType = status & 0xF0
//        let channel = Int(status & 0x0F) + 1
//
//        switch messageType {
//
//        case 0x80: // Note Off
//            let note = Int(data1)
//            let velocity = midi7bitToUnitDouble(data2)
//            VKController.NoteOff(note: note, velocity: velocity)
//
//        case 0x90: // Note On (velocity 0 == Note Off)
//            let note = Int(data1)
//            let velocity = midi7bitToUnitDouble(data2)
//
//            if data2 == 0 {
//                VKController.NoteOff(note: note, velocity: 0)
//            } else {
//                VKController.NoteOn(note: note, velocity: velocity)
//            }
//
//        case 0xA0: // Poly Aftertouch
//            let note = Int(data1)
//            let pressure = midi7bitToUnitDouble(data2)
//            VKController.PolyAftertouch(note: note, pressure: pressure)
//
//        case 0xB0: // Control Change
//            let cc = Int(data1)
//            let value = Int(data2)
//            handleControlChange(channel: channel, cc: cc, value: value)
//
//        case 0xC0: // Program Change
//            let program = Int(data1)
//            print("Program Change    (ch: \(channel), program: \(program))")
//
//        case 0xD0: // Channel Aftertouch
//            let pressure = midi7bitToUnitDouble(data1)
//            print("Channel Aftertouch    (ch: \(channel), pressure: \(pressure))")
//
//        case 0xE0: // Pitch Bend
//            let lsb = Int(data1)
//            let msb = Int(data2)
//            let rawValue = (msb << 7) | lsb
//            let centeredValue = rawValue - 8192
//            print("Pitch Bend    (ch: \(channel), value: \(centeredValue))")
//
//        default:
//            break
//        }
//    }
//
//    private func handleControlChange(channel: Int, cc: Int, value: Int) {
//        let depth = midi7bitToUnitDouble(UInt8(value))
//        let pressed = value >= 64
//
//        switch cc {
//        case 67: // Soft pedal (una corda)
//            VKController.ControlChange_pedal(
//                pedal: 1,
//                targetStatus: pressed,
//                depth: depth
//            )
//
//        case 69: // Hold 2 / often reused by some devices, here treated as harmonic pedal slot
//            VKController.ControlChange_pedal(
//                pedal: 2,
//                targetStatus: pressed,
//                depth: depth
//            )
//
//        case 66: // Sostenuto
//            VKController.ControlChange_pedal(
//                pedal: 3,
//                targetStatus: pressed,
//                depth: depth
//            )
//
//        case 64: // Sustain / Damper
//            VKController.ControlChange_pedal(
//                pedal: 4,
//                targetStatus: pressed,
//                depth: depth
//            )
//
//        default:
//            // 不是预定义名称的踏板，不送进 VKController，避免触发 fatalError
//            // 但仍然按你要的风格打印出来。
//            if isPedalLikeCC(cc) {
//                let pedalLabel = genericPedalLabel(for: cc)
//                if pressed {
//                    print("PedalPressed: \(pedalLabel) has been pressed.")
//                    print("Pedal depth: \(depth)\n")
//                } else {
//                    print("PedalReleased: \(pedalLabel) has been released.\n")
//                }
//            } else {
//                print("Control Change    (ch: \(channel), cc: \(cc), value: \(value))")
//            }
//        }
//    }
//
//    private func isPedalLikeCC(_ cc: Int) -> Bool {
//        // 常见踏板 / 脚控相关控制器区间
//        // 64 sustain, 65 portamento, 66 sostenuto, 67 soft, 68 legato, 69 hold2
//        return (64...69).contains(cc)
//    }
//
//    private func genericPedalLabel(for cc: Int) -> String {
//        switch cc {
//        case 65: return "Pedal1"
//        case 68: return "Pedal2"
//        default: return "Pedal\(cc)"
//        }
//    }
//
//    private func midi7bitToUnitDouble(_ value: UInt8) -> Double {
//        Double(value) / 127.0
//    }
//}
