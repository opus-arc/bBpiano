//
//  blueBirdUITestsLaunchTests.swift
//  blueBirdUITests
//
//  Created by opus arc on 2026/3/29.
//

/**
 *
 * 启动测试
 *
 * 重点检查 App 是否能正常拉起，以及启动时的一些基础状态。
 *    •    测试 App 能否成功启动
 *    •    在启动后截图
 *    •    验证启动页/初始页面是否存在
 *    •    做最基础的 smoke test（冒烟测试）
 *
 */
 

import XCTest

final class blueBirdUITestsLaunchTests: XCTestCase {

    override class var runsForEachTargetApplicationUIConfiguration: Bool {
        true
    }

    override func setUpWithError() throws {
        continueAfterFailure = false
    }

    @MainActor
    func testLaunch() throws {
        let app = XCUIApplication()
        app.launch()

        // Insert steps here to perform after app launch but before taking a screenshot,
        // such as logging into a test account or navigating somewhere in the app

        let attachment = XCTAttachment(screenshot: app.screenshot())
        attachment.name = "Launch Screen"
        attachment.lifetime = .keepAlways
        add(attachment)
    }
}
