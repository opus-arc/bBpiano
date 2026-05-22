//
//  MidiService.swift
//  bBpiano
//
//  Created by opus arc on 2026/4/8.
//
//  AI Assisted
//

import Foundation
import CoreMIDI

fileprivate enum MidiParseError: Error, LocalizedError {
    case invalidFile
    case unsupportedFormat(UInt16)
    case missingHeader
    case corruptTrack
    case unsupportedTimeDivision

    var errorDescription: String? {
        switch self {
        case .invalidFile:
            return "Invalid MIDI file."
        case .unsupportedFormat(let format):
            return "Unsupported MIDI format: \(format)."
        case .missingHeader:
            return "Missing MIDI header chunk."
        case .corruptTrack:
            return "Corrupt MIDI track chunk."
        case .unsupportedTimeDivision:
            return "SMPTE time division is not supported."
        }
    }
}

private final class MidiPlaybackState {
    private let lock = NSLock()
    private var cancelled = false
    private(set) var activeNotes = Set<Int>()

    func cancel() {
        lock.lock()
        cancelled = true
        let notes = activeNotes
        activeNotes.removeAll()
        lock.unlock()

        for note in notes {
            VKController.NoteOff(note: note, velocity: 0)
        }
    }

    func isCancelled() -> Bool {
        lock.lock()
        defer { lock.unlock() }
        return cancelled
    }

    func noteOn(_ note: Int) {
        lock.lock()
        activeNotes.insert(note)
        lock.unlock()
    }

    func noteOff(_ note: Int) {
        lock.lock()
        activeNotes.remove(note)
        lock.unlock()
    }
}

public enum MidiService {
    private struct MidiEvent {
        let timeInSeconds: TimeInterval
        let status: UInt8
        let data1: UInt8
        let data2: UInt8
        let trackName: String?

        var channel: Int {
            Int(status & 0x0F)
        }

        var statusType: UInt8 {
            status & 0xF0
        }

        var isNoteOn: Bool {
            statusType == 0x90 && data2 > 0
        }

        var isNoteOff: Bool {
            statusType == 0x80 || (statusType == 0x90 && data2 == 0)
        }
    }

    private static var playbackState: MidiPlaybackState?
    private static let playbackQueue = DispatchQueue(label: "MidiService.playbackQueue", qos: .userInitiated)

    /// Plays the piano part of a Standard MIDI File.
    ///
    /// - Parameters:
    ///   - playbackRate: Playback multiplier. `1.0` is original speed, `2.0` is twice as fast.
    ///   - startTime: Start offset in seconds from the beginning of the MIDI performance.
    ///   - midiFileURL: Local MIDI file URL.
    public static func play(
        playbackRate: Double,
        startTime: TimeInterval,
        midiFileURL: URL
    ) throws {
        guard playbackRate > 0 else {
            throw NSError(
                domain: "MidiService",
                code: 1,
                userInfo: [NSLocalizedDescriptionKey: "playbackRate must be greater than 0."]
            )
        }

        Self.stop()

        let data = try Data(contentsOf: midiFileURL)
        let allEvents = try Self.parseMidiFile(data)
        let pianoEvents = Self.selectPianoEvents(from: allEvents)
            .filter { $0.timeInSeconds >= max(0, startTime) }
            .sorted { $0.timeInSeconds < $1.timeInSeconds }

        let state = MidiPlaybackState()
        Self.playbackState = state

        Self.playbackQueue.async {
            Self.playEvents(
                pianoEvents,
                playbackRate: playbackRate,
                startTime: max(0, startTime),
                state: state
            )
        }
    }

    public static func stop() {
        playbackState?.cancel()
        playbackState = nil
    }

    private static func playEvents(
        _ events: [MidiEvent],
        playbackRate: Double,
        startTime: TimeInterval,
        state: MidiPlaybackState
    ) {
        var lastScheduledTime = startTime

        for event in events {
            if state.isCancelled() {
                return
            }

            let delta = max(0, event.timeInSeconds - lastScheduledTime) / playbackRate
            if delta > 0 {
                Thread.sleep(forTimeInterval: delta)
            }
            lastScheduledTime = event.timeInSeconds

            if state.isCancelled() {
                return
            }

            let note = Int(event.data1)
            let velocity = Double(event.data2)

            if event.isNoteOn {
                state.noteOn(note)
                VKController.NoteOn(note: note, velocity: velocity)
            } else if event.isNoteOff {
                state.noteOff(note)
                VKController.NoteOff(note: note, velocity: velocity)
            }
        }
    }

    private static func selectPianoEvents(from events: [MidiEvent]) -> [MidiEvent] {
        let noteEvents = events.filter { $0.isNoteOn || $0.isNoteOff }

        let explicitlyNamedPianoEvents = noteEvents.filter { event in
            guard let name = event.trackName?.lowercased() else { return false }
            return name.contains("piano") || name.contains("keyboard") || name.contains("grand")
        }

        if !explicitlyNamedPianoEvents.isEmpty {
            return explicitlyNamedPianoEvents
        }

        let channelZeroEvents = noteEvents.filter { $0.channel == 0 }
        if !channelZeroEvents.isEmpty {
            return channelZeroEvents
        }

        return noteEvents
    }

    private static func parseMidiFile(_ data: Data) throws -> [MidiEvent] {
        var reader = MidiReader(data: data)

        guard reader.readString(length: 4) == "MThd" else {
            throw MidiParseError.missingHeader
        }

        let headerLength = try reader.readUInt32()
        let format = try reader.readUInt16()
        let trackCount = try reader.readUInt16()
        let division = try reader.readUInt16()

        guard format == 0 || format == 1 else {
            throw MidiParseError.unsupportedFormat(format)
        }

        guard (division & 0x8000) == 0 else {
            throw MidiParseError.unsupportedTimeDivision
        }

        let ticksPerQuarterNote = Double(division)
        if headerLength > 6 {
            try reader.skip(Int(headerLength - 6))
        }

        var events: [MidiEvent] = []

        for _ in 0..<trackCount {
            guard reader.readString(length: 4) == "MTrk" else {
                throw MidiParseError.corruptTrack
            }

            let trackLength = Int(try reader.readUInt32())
            let trackEndIndex = reader.index + trackLength
            var tempoMicrosecondsPerQuarter = 500_000.0
            var seconds = 0.0
            var runningStatus: UInt8?
            var trackName: String?

            while reader.index < trackEndIndex {
                let deltaTicks = try reader.readVariableLengthQuantity()
                seconds += (Double(deltaTicks) * tempoMicrosecondsPerQuarter / 1_000_000.0) / ticksPerQuarterNote

                let firstByte = try reader.readByte()
                var status: UInt8
                var data1: UInt8?

                if firstByte < 0x80 {
                    guard let previousStatus = runningStatus else {
                        throw MidiParseError.corruptTrack
                    }
                    status = previousStatus
                    data1 = firstByte
                } else {
                    status = firstByte
                }

                if status == 0xFF {
                    let metaType = try reader.readByte()
                    let length = Int(try reader.readVariableLengthQuantity())
                    let metaData = try reader.readData(length: length)

                    if metaType == 0x03, let name = String(data: metaData, encoding: .utf8) {
                        trackName = name
                    } else if metaType == 0x51, metaData.count == 3 {
                        tempoMicrosecondsPerQuarter = Double(metaData[0]) * 65_536
                            + Double(metaData[1]) * 256
                            + Double(metaData[2])
                    }

                    runningStatus = nil
                    continue
                }

                if status == 0xF0 || status == 0xF7 {
                    let length = Int(try reader.readVariableLengthQuantity())
                    try reader.skip(length)
                    runningStatus = nil
                    continue
                }

                runningStatus = status

                let statusType = status & 0xF0
                let firstDataByte = try data1 ?? reader.readByte()

                switch statusType {
                case 0x80, 0x90:
                    let velocity = try reader.readByte()
                    events.append(
                        MidiEvent(
                            timeInSeconds: seconds,
                            status: status,
                            data1: firstDataByte,
                            data2: velocity,
                            trackName: trackName
                        )
                    )

                case 0xA0, 0xB0, 0xE0:
                    _ = try reader.readByte()

                case 0xC0, 0xD0:
                    break

                default:
                    throw MidiParseError.corruptTrack
                }
            }

            if reader.index != trackEndIndex {
                reader.index = trackEndIndex
            }
        }

        return events.sorted { $0.timeInSeconds < $1.timeInSeconds }
    }
}

private struct MidiReader {
    let data: Data
    var index: Int = 0

    mutating func readByte() throws -> UInt8 {
        guard index < data.count else { throw MidiParseError.invalidFile }
        let byte = data[index]
        index += 1
        return byte
    }

    mutating func readData(length: Int) throws -> Data {
        guard length >= 0, index + length <= data.count else {
            throw MidiParseError.invalidFile
        }
        let subdata = data[index..<(index + length)]
        index += length
        return Data(subdata)
    }

    mutating func readString(length: Int) -> String? {
        guard length >= 0, index + length <= data.count else { return nil }
        let subdata = data[index..<(index + length)]
        index += length
        return String(data: Data(subdata), encoding: .ascii)
    }

    mutating func readUInt16() throws -> UInt16 {
        let high = UInt16(try readByte())
        let low = UInt16(try readByte())
        return (high << 8) | low
    }

    mutating func readUInt32() throws -> UInt32 {
        let byte1 = UInt32(try readByte())
        let byte2 = UInt32(try readByte())
        let byte3 = UInt32(try readByte())
        let byte4 = UInt32(try readByte())
        return (byte1 << 24) | (byte2 << 16) | (byte3 << 8) | byte4
    }

    mutating func readVariableLengthQuantity() throws -> UInt32 {
        var value: UInt32 = 0

        for _ in 0..<4 {
            let byte = try readByte()
            value = (value << 7) | UInt32(byte & 0x7F)
            if (byte & 0x80) == 0 {
                return value
            }
        }

        throw MidiParseError.invalidFile
    }

    mutating func skip(_ count: Int) throws {
        guard count >= 0, index + count <= data.count else {
            throw MidiParseError.invalidFile
        }
        index += count
    }
}
