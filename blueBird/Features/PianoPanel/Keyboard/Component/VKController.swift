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
        print("按下：Midi" + String(note) + "按下速度: " + String(velocity) + "\n")
    }

    static func NoteOff(note: Int, velocity: Double) {
        print("释放：Midi" + String(note) + "释放速度: " + String(velocity) + "\n")
    }

    static func PolyAftertouch(note: Int, pressure: Double) {
        print("触后：Midi" + String(note) + "压力: " + String(pressure) + "\n")
    }

    static func ChannelAftertouch(pressure: Double) {
        print("通道压力:" + String(pressure) + "\n")
    }

    static func ControlChange_pedal(pedal: Int, targetStatus: Bool) {
        switch pedal {
        case 1:
            if targetStatus {
                print("Soft pedal (una corda) has been pressed.")
            } else {
                print("Soft pedal (una corda) has been released.")
            }

        case 2:
            if targetStatus {
                print("Harmonic pedal has been pressed.")
            } else {
                print("Harmonic pedal has been released.")
            }

        case 3:
            if targetStatus {
                print("Sostenuto pedal has been pressed.")
            } else {
                print("Sostenuto pedal has been released.")
            }

        case 4:
            if targetStatus {
                print("Sustain pedal (damper) has been pressed.")
            } else {
                print("Sustain pedal (damper) has been released.")
            }

        default:
            fatalError("Received incorrect pedal press signal")
        }
    }
}
