//
//  ParametersStruct.swift
//  bBpiano
//
//  Created by opus arc on 2026/4/2.
//
//  This document is not AI-assisted.
//

import CoreData

// Bundle / Document / Codable
// 靠尼玛的这个东西问题怎么这么多
//let url = Bundle.main.url(
//    forResource: "userPreset",
//    withExtension: "json"
//)
// 这里面初始化也会产生很大影响




// MARK: Preset
// 指 json 和 swift 可逆转换的类型 代表对 json 类型的强大支持
struct Preset: Codable {
    var version: Double
    var name: String
    var tuning: Tuning
    
    init(){
        version = 1.0
        name = "placeholder"
        tuning = Tuning()
    }
}

// MARK: - Tuning
struct Tuning: Codable {
    
    // 基准音高 (ReferenceHz)
    // 基准音 A4 的频率
    /// 440.0（标准）、442.0（偏亮）、438.0（偏柔）
    var referenceHz: Double
    
    // 律制（Temperament）
    // 决定一个八度内 12 个音之间的频率分布关系
    /// - "equal"（十二平均律）
    /// - "pythagore"（毕达哥拉斯律）
    /// - "zarlino"（纯律）
    /// - "meantone"（中庸律）
    var temperament: Temperament
    
    // 齐音宽度（Unison Width）
    // 控制同一音对应的多根弦之间的微小频率偏移（detune）
    // 影响声音的“厚度 / 宽度 / 活性”
    /// - 值小：声音更纯净、集中（接近单弦）、偏电子乐感
    /// - 值大：产生拍频（beating），声音更宽、更“活”、偏真实钢琴
    var unisonWidth: Double
    
    // 直达声持续时间（Direct Sound Duration）
    // 控制击弦后“主声音能量”保持的时间，以及向共鸣系统扩散的速度
    // 也就是说这个值越小，越多的能量就流向共鸣系统去扩散
    // 本质上是控制 direct sound 与 resonance 之间的能量分配
    /// - 值大：
    ///   - 能量更集中在弦本体
    ///   - 声音更清晰、干净
    ///   - 共鸣扩散较慢
    /// - 值小：
    ///   - 能量更快进入 soundboard / resonance
    ///   - 声音更“铺开”、更有空间感
    ///   - 尾音更明显（听感更长）
    var directSoundDuration: Double
    
    init(){
        referenceHz = 440.0
        temperament = Temperament.equal
        unisonWidth = 1.00
        directSoundDuration = 1.00
    }
}

// MARK: -- Temperament
/// 所有 case 对应不同历史或理论调律系统，会直接影响和声色彩与调性感
enum Temperament: String, Codable {

    /// 十二平均律（Equal Temperament）
    /// 将一个八度均分为 12 个相等的半音（对数均分）
    /// - 优点：所有调完全等价，可自由转调
    /// - 缺点：音程不是绝对纯净（尤其是三度）
    /// - 现代钢琴默认标准
    case equal

    /// 毕达哥拉斯律（Pythagorean Tuning）
    /// 基于纯五度（3:2）构建音阶
    /// - 优点：五度非常纯净、明亮
    /// - 缺点：大三度偏尖，和声较硬
    /// - 适合早期复调音乐
    case pythagore

    /// 纯律（Just Intonation / Zarlino）
    /// 基于简单整数比（如 5:4、6:5）优化三和弦
    /// - 优点：和弦非常纯净、柔和
    /// - 缺点：无法自由转调，依赖调中心
    /// - 适合和声纯净的音乐环境
    case zarlino

    /// 中庸律（Meantone, 1/4 comma）
    /// 略微压缩五度以换取更纯的大三度
    /// - 优点：常用调和声非常优美
    /// - 缺点：远调会出现“狼音程”
    /// - 介于纯律与平均律之间的折中方案
    case meantone

    /// 好律（Well Temperament）
    /// 一类调律方法（非单一公式），允许所有调可用但各具性格
    /// - 优点：不同调有不同色彩（调性个性）
    /// - 缺点：不完全均匀
    /// - 常见于巴洛克音乐（如巴赫）
    case well

    /// Werckmeister III（魏尔克迈斯特第三律）
    /// 一种具体的好律方案
    /// - 优点：在保证可转调的同时保留调性差异
    /// - 特点：部分五度纯净，部分五度调整
    /// - 听感：不同调具有明显情绪差别
    case werckmeister
    
    
    var displayName: String {
        switch self {
        case .equal: return "Equal temperament"
        case .pythagore: return "Pythagore"
        case .zarlino: return "Zarlino"
        case .meantone: return "Meantone (1/4 comma)"
        case .well: return "Well temperament"
        case .werckmeister: return "Werckmeister III"
        }
    }
}

// MARK: - Voicing


