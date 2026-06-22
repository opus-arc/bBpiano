//
//  HammerStringTest.hpp
//  HammerStringEvaluationTests
//
//  Created by opus arc on 2026/5/23.
//

#ifndef HammerStringTest_hpp
#define HammerStringTest_hpp

// Runs the production bBpiano Lite HammerModel/StringModel coupling diagnostic.
// The implementation is enabled only for the standalone diagnostic build so
// the existing XCTest bundle does not accidentally link against a second
// PianoModel implementation.
int runHammerStringCouplingTest();

#endif /* HammerStringTest_hpp */
