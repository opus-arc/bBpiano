//
//  CPUMeterView.swift
//  bBpiano
//
//  Created by opus arc on 2026/5/16.
//

import SwiftUI
import Darwin.Mach
import Combine

struct PerformanceMeterPanel: View {
    
    @State private var engineRate: Double = 0.0
    @State private var cpuUsage: Double = 0.0
    @State private var showsEngineRate: Bool = true

    private let timer = Timer.publish(
        every: 0.5,
        on: .main,
        in: .common
    ).autoconnect()

    var body: some View {
        Text(meterText)
            .font(.system(size: 12, weight: .medium, design: .monospaced))
            .foregroundStyle(.primary)
            .frame(width: 72, height: 24)
            .contentShape(Rectangle())
            .onTapGesture {
                showsEngineRate.toggle()
            }
            .onAppear {
                refreshMeters()
            }
            .onReceive(timer) { _ in
                refreshMeters()
            }
    }

    private var meterText: String {
        if showsEngineRate {
            return String(format: "ENG %.0f%%", engineRate * 100.0)
        } else {
            return String(format: "CPU %.0f%%", cpuUsage)
        }
    }

    private func refreshMeters() {
        engineRate = get_engineRate()
        cpuUsage = CPUUsageReader.currentProcessCPUUsage()
    }
}


private enum CPUUsageReader {
    static func currentProcessCPUUsage() -> Double {
        var threadList: thread_act_array_t?
        var threadCount = mach_msg_type_number_t(0)

        let result = task_threads(
            mach_task_self_,
            &threadList,
            &threadCount
        )

        guard result == KERN_SUCCESS, let threadList else {
            return 0.0
        }

        defer {
            let size = vm_size_t(Int(threadCount) * MemoryLayout<thread_t>.stride)
            vm_deallocate(
                mach_task_self_,
                vm_address_t(bitPattern: threadList),
                size
            )
        }

        var totalCPU: Double = 0.0

        for index in 0..<Int(threadCount) {
            var threadInfo = thread_basic_info()
            var threadInfoCount = mach_msg_type_number_t(
                MemoryLayout<thread_basic_info>.size / MemoryLayout<natural_t>.size
            )

            let infoResult = withUnsafeMutablePointer(to: &threadInfo) { pointer in
                pointer.withMemoryRebound(to: integer_t.self, capacity: Int(threadInfoCount)) { reboundPointer in
                    thread_info(
                        threadList[index],
                        thread_flavor_t(THREAD_BASIC_INFO),
                        reboundPointer,
                        &threadInfoCount
                    )
                }
            }

            guard infoResult == KERN_SUCCESS else {
                continue
            }

            let isIdle = (threadInfo.flags & TH_FLAGS_IDLE) != 0
            if !isIdle {
                totalCPU += Double(threadInfo.cpu_usage) / Double(TH_USAGE_SCALE) * 100.0
            }
        }

        return totalCPU
    }
}

#Preview {
    PerformanceMeterPanel()
        .padding()
}
