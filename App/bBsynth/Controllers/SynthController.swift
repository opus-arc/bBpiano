import Combine
import Foundation

@MainActor
final class SynthController: ObservableObject {
    let midiManager: MIDIManager
    let parameters: ParameterStore

    init() {
        let midiManager = MIDIManager()
        self.midiManager = midiManager
        parameters = ParameterStore { [weak midiManager] parameter in
            guard let cc = parameter.midiCC else { return }
            midiManager?.sendControlChange(
                controller: cc,
                value: parameter.midiValue
            )
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
