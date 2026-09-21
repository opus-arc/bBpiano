////
////  ParametersManager.swift
////  bBpiano
////
////  Created by opus arc on 2026/4/3.
////
////  This document is not AI-assisted.
////
//
//import Foundation
//
//public class PresetManager {
//
//    static let shared: PresetManager = PresetManager()
//
//    var currPreset: Preset
//    
//    private init() {
//        // TODO: 初始可换成常用的一套 preset
//            currPreset = Self.readPreset()
//    }
//
//    static func readPreset() -> Preset {
//        let url = URL(
//            filePath:
//                "/Users/opusarc/XCodeProjects/bBpiano/Core/SwiftCore/ParametersManager/ModelParameters/ModelParameters.json"
//        )
//
//        do {
//            let data = try Data(contentsOf: url)
//            let preset: Preset =
//                try JSONDecoder().decode(Preset.self, from: data)
//            
//            return preset
//            
//        } catch {
//            print("Failed to read preset\(error)");
//            return Preset()
//        }
//    }
//    
//    // 用到这个函数，通常是整体切换 preset 面板，单独改值不会用这种方式
//    static func changePreset(_ preset: Preset) {
//        shared.currPreset = preset
//    }
//    
//    
//}
//
//
//extension PresetManager {
//
//    func setReferenceHz(_ value: Double) {
//        currPreset.tuning.referenceHz = value
//    }
//
//    func setTemperament(_ value: Temperament) {
//        currPreset.tuning.temperament = value
//    }
//
//    func setUnisonWidth(_ value: Double) {
//        currPreset.tuning.unisonWidth = value
//    }
//
//    func setDirectSoundDuration(_ value: Double) {
//        currPreset.tuning.directSoundDuration = value
//    }
//
//    func setName(_ name: StringModel) {
//        currPreset.name = name
//    }
//
//    func setVersion(_ version: Double) {
//        currPreset.version = version
//    }
//}
