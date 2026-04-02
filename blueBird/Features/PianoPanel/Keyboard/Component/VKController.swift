//
//  Untitled.swift
//  blueBird
//
//  Created by opus arc on 2026/3/30.
//

/**

 Note On    (ch, note, velocity)    -   按下的音符与按下速度:  Press speed 常量处理
 Note Off    (ch, note, velocity)    -   释放的音符与释放速度:  Release speed 常量处理

 Poly Aftertouch           (ch, note, pressure)     -    单音压力
 Channel Aftertouch     (ch, pressure)              -    通道压力

 Control Change   (ch, cc, value)   -   控制器变化

 Pitch Bend   (ch, value)   -   弯音值 音轮控制

 Program Change   (ch, program)   -   音色控制

 */

public class VKController {
    static func NoteOn(note: Int, velocity: Double) {
        print("NoteOn: Midi" + String(note) + "PressVelocity: " + String(velocity) + "\n")
    }

    static func NoteOff(note: Int, velocity: Double) {
        print("NoteOff：Midi" + String(note) + "ReleaseVelocity: " + String(velocity) + "\n")
    }

    static func PolyAftertouch(note: Int, pressure: Double) {
        print("PolyAfterTouch：Midi" + String(note) + "Pressure: " + String(pressure) + "\n")
    }

    static func ChannelAftertouch(pressure: Double) {
        print("ChannelAftertouch: Pressure: " + String(pressure) + "\n")
    }

    static func ControlChange_pedal(
        pedal: Int,
        targetStatus: Bool,
        depth: Double
    ) {
        switch pedal {
        case 1:
            if targetStatus {
                print("PedalPressed: Soft pedal (una corda) has been pressed.")
                print("Pedal depth: " + String(depth) + "\n")
            } else {
                print("PedalReleased: Soft pedal (una corda) has been released.\n")
            }

        case 2:
            if targetStatus {
                print("PedalPressed: Harmonic pedal has been pressed.")
                print("Pedal depth: " + String(depth) + "\n")
            } else {
                print("PedalReleased: Harmonic pedal has been released.\n")
            }

        case 3:
            if targetStatus {
                print("PedalPressed: Sostenuto pedal has been pressed.")
                print("Pedal depth: " + String(depth) + "\n")
            } else {
                print("PedalReleased: Sostenuto pedal has been released.\n")
            }

        case 4:
            if targetStatus {
                print("PedalPressed: Sustain pedal (damper) has been pressed.")
                print("Pedal depth: " + String(depth) + "\n")
            } else {
                print("PedalReleased: Sustain pedal (damper) has been released.\n")
            }

        default:
            fatalError("Received incorrect pedal press signal.\n")
        }
    }
}
