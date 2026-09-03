// ============================================================================
// bBpiano Physical Modeling Engine
// Backend Command Line Interface
//
// Authorship:
//   Ziyang Tan & Zhuoran Chen
//
// Design & Architecture:
//   Ziyang Tan
//
// Conceptual Development & Engineering:
//   Ziyang Tan & Zhuoran Chen
//
// AI Disclosure:
//   AI tools were used for learning programming techniques and domain knowledge
//   in physical modeling pianos and synthesizers.
//
//   CORE CLI IMPLEMENTATION:
//   DESIGNED AND DEVELOPED WITHOUT AI-GENERATED CODE.
//
// Project Initiated:
//   April 1, 2026
// ============================================================================

// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
// [ZERO AI-GENERATED CODE]
// Every line in this file is written and understood by its author. Every result is
// reproducible, every assumption is open to inspection, and every implementation
// stands open to criticism and challenge.
// AI may be used for non-core, replaceable, engineering work;
// this file, however, contains core logic that the author considers
// necessary to understand firsthand, explain line by line, and take full responsibility for,
// and is therefore implemented entirely by hand.
// ---------------------------------------------------------------------------
// [本文件承诺不含任何 AI 生成代码]
// 每一行代码均由作者亲自编写，并确知其意义。一切结果可以复现，一切假设可经受检验，一切实现经得起批评与质疑。
// AI 可用于非核心、可替代的工程工作；
// 本文件承载作者认为必须亲自理解、能够逐行解释并为之负责的核心逻辑，因此刻意保持完全人工实现。
//
// Ziyang Tan
// 2026-09-03
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

// --------------------------------------------------------------------
// MARK: version & logo

static constexpr const char* version = "L0-Synthesis";
static constexpr const char* logo =
    R"(
        ┌────────────────────────────┐──╭────╮
        │                            │==│╲╱╲╱│
        │  bBpiano 0                 │==│╱╲╱╲│
        │  L0-Synthesis/260903       │==│╲╱╲╱│
        │                            │==│╱╲╱╲│
        │  Physical Modeling Piano   │==│╲╱╲╱│
        │                            │==│╱╲╱╲│
        │  Developed by              │==│╲╱╲╱│
        │    Ziyang Tan              │==│╱╲╱╲│
        │    Zhuoran Chen            │==│╱╲╱╲│
        │                            │==│╲╱╲╱│
        │  bBSonicLab                │==│╱╲╱╲│
        └────────────────────────────┘──╰────╯

    )";

// --------------------------------------------------------------------
// MARK: library
// cli entry
#include "./hardware/cli_helper.hpp"


int main(int argc, char* argv[]) {
    
    // --------------------------------------------------------------------
    // MARK: arg tmp test
    char* testArgv[] = {
        argv[0]
    };
    argc = 1;
    argv = testArgv;
    
    
    return cli_helper(argc, argv, version, logo);
    
}


