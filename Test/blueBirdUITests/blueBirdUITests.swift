//
//  blueBirdUITests.swift
//  blueBirdUITests
//
//  Created by opus arc on 2026/3/29.
//

/**
 *
 * UI 测试用例
 *
 * 它关注的是：界面行为和用户操作流程是否正确。
 *    •    App 启动后首页是否出现
 *    •    点击按钮后是否进入下一个页面
 *    •    输入文本后是否显示正确结果
 *    •    某个列表项是否存在
 *
 */

import XCTest

final class blueBirdUITests: XCTestCase {

    override func setUpWithError() throws {
        // Put setup code here. This method is called before the invocation of each test method in the class.

        // In UI tests it is usually best to stop immediately when a failure occurs.
        continueAfterFailure = false

        // In UI tests it’s important to set the initial state - such as interface orientation - required for your tests before they run. The setUp method is a good place to do this.
    }

    override func tearDownWithError() throws {
        // Put teardown code here. This method is called after the invocation of each test method in the class.
    }

    @MainActor
    func testExample() throws {
        // UI tests must launch the application that they test.
        let app = XCUIApplication()
        app.launch()

        // Use XCTAssert and related functions to verify your tests produce the correct results.
    }

    @MainActor
    func testLaunchPerformance() throws {
        // This measures how long it takes to launch your application.
        measure(metrics: [XCTApplicationLaunchMetric()]) {
            XCUIApplication().launch()
        }
    }
}
