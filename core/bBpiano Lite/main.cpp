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

// MARK: version & logo

static constexpr const char *version = "L2-Vera";
static constexpr const char *logo =
    R"(
        ┌────────────────────────────┐──╭────╮
        │                            │==│╲╱╲╱│
        │  bBpiano 2                 │==│╱╲╱╲│
        │  L2-Vera/261101            │==│╲╱╲╱│
        │                            │==│╱╲╱╲│
        │  Physical Modeling Piano   │==│╲╱╲╱│
        │                            │==│╱╲╱╲│
        │  Developed by              │==│╲╱╲╱│
        │    Ziyang Tan              │==│╱╲╱╲│
        └────────────────────────────┘──╰────╯

        With Special Thanks to
        Zhuoran Chen
        for the conversations and inspiration
    
        bBpiano CLI Use and Extension License
        Version 1.2 · October 27, 2026
    
        bbpl Technology
        Copyright © 2026 Ziyang Tan
        bBSonicLab

    )";

// --------------------------------------------------------------------
// MARK: library

#include "./core/bbpl/piano_controller.hpp"
#include "./core/service_controller.hpp"

#include <cstdlib>
#include <exception>
#include <iostream>

int main(int argc, char *argv[]) {
//   --------------------------------------------------------------------
//   MARK: arg tmp test
//          char* testArgv[] = {
//              argv[0],
//              "-t"
//          };
//          argc = 2;
//          argv = testArgv;

//              char* testArgv[] = {
//                  argv[0],
//                  "-e",
//                  "/Users/opusarc/Projects/XCodeProjects/bBpiano/core/bBpiano Lite/core/bbpl/piano/test a4/D4_velocity127.mid"
//              };
//              argc = 3;
//              argv = testArgv;

  //            char* testArgv[] = {
  //                argv[0],
  //                "-m",
  //                "/Users/opusarc/Projects/XCodeProjects/bBpiano/Cli/bBpiano
  //                Lite/core/bbpl/resource/midi_samples/Fancy's
  //                selection/midi/Sonata in B Min._2006.midi"
  //            };
  //            argc = 3;
  //            argv = testArgv;
  //    char* testArgv[] = {
  //                argv[0],
  //                "-m",
  //                "/Users/opusarc/Projects/XCodeProjects/bBpiano/Cli/bBpiano
  //                Lite/core/bbpl/resource/midi_samples/Fancy's
  //                selection/midi/Sonata No. 28 in A Major, Op. 101
  //                (Complete)_2014.midi"
  //            };
  //            argc = 3;
  //            argv = testArgv;
  //
  try {

    return cli_entry(argc, argv, version, logo);

  } catch (const std::exception &error) {

    std::cerr << "bbpl: " << error.what() << '\n';
    return EXIT_FAILURE;

  } catch (...) {

    std::cerr << "bbpl: unknown fatal error.\n";
    return EXIT_FAILURE;
  }
}
