//
//  VirtualMIDIController.swift
//  bBpiano
//
//  Created by opus arc on 2026/9/19.
//

import CoreMIDI

final class VirtualMIDIController {

    private var client = MIDIClientRef()
    private var source = MIDIEndpointRef()
    
    
//    MIDIReceivedEventList(source, &eventList)

    init() {
        createVirtualSource()
    }

    private func createVirtualSource() {

        MIDIClientCreateWithBlock(
            "bbpl MIDI Client" as CFString,
            &client
        ) { notification in
            // MIDI 系统状态变化
        }

        MIDISourceCreateWithProtocol(
            client,
            "BBPL Virtual Keyboard" as CFString,
            ._1_0,
            &source
        )
    }
    
    public func noteOn(note: Int8, velocity: Int8) {
//        MIDIReceived()
    }
}
