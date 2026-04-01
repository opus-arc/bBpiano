//
//  blueBirdTests.swift
//  blueBirdTests
//
//  Created by opus arc on 2026/3/29.
//

/**
 *
 * 单元测试
 *
 * 主要用于测试业务逻辑是否正确，而不是测试界面操作
 *    •   某个函数是否返回正确结果
 *    •   某个模型转换是否正确
 *    •   某段业务逻辑在边界条件下是否正常
 *    •   数据层读写是否符合预期
 *
 */

import XCTest
@testable import blueBird

final class blueBirdTests: XCTestCase {

    override func setUpWithError() throws {
        // Put setup code here. This method is called before the invocation of each test method in the class.
    }

    override func tearDownWithError() throws {
        // Put teardown code here. This method is called after the invocation of each test method in the class.
    }

    func testExample() throws {
        // This is an example of a functional test case.
        // Use XCTAssert and related functions to verify your tests produce the correct results.
        // Any test you write for XCTest can be annotated as throws and async.
        // Mark your test throws to produce an unexpected failure when your test encounters an uncaught error.
        // Mark your test async to allow awaiting for asynchronous code to complete. Check the results with assertions afterwards.
    }

    func testPerformanceExample() throws {
        // This is an example of a performance test case.
        self.measure {
            // Put the code you want to measure the time of here.
        }
    }

}
