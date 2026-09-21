#include "./bbpl_c_api.h"

#include "../service_controller.hpp"

#include <stop_token>
#include <thread>

namespace {

static constexpr const char *version_c_api = "L1-Clavier";

static constexpr const char *logo_c_api =
    R"(
            ┌────────────────────────────┐──╭────╮
            │                            │==│╲╱╲╱│
            │  bBpiano 1                 │==│╱╲╱╲│
            │  L1-Clavier/260903         │==│╲╱╲╱│
            │                            │==│╱╲╱╲│
            │  Physical Modeling Piano   │==│╲╱╲╱│
            │                            │==│╱╲╱╲│
            │  Developed by              │==│╲╱╲╱│
            │    Ziyang Tan              │==│╱╲╱╲│
            │                            │==│╲╱╲╱│
            │  bBSonicLab                │==│╱╲╱╲│
            └────────────────────────────┘──╰────╯

            With Special Thanks to
            Zhuoran Chen
            for the conversations and inspiration

            bBSonicLab Technology
            Copyright (c) 2026 Ziyang Tan.
            All rights reserved.

        )";

// Each service owns one independent jthread.
//
// request_stop() only affects the corresponding invocation.
// cli_entry() receives the jthread stop_token and bridges it into
// its internal service_stop source.
std::jthread midi_thread;
std::jthread piano_thread;
std::jthread keyboard_thread;
std::jthread export_thread;
std::jthread record_thread;
std::jthread test_thread;
std::jthread internal_test_thread;

// ========================
// Thread helper
// ========================

void stop_thread(std::jthread &thread) {
  if (!thread.joinable()) {
    return;
  }

  thread.request_stop();
  thread.join();
}

} // namespace

// ========================
// MIDI Service
// ========================

void midi_service_start(const char *midi_path) {
  if (midi_path == nullptr) {
    return;
  }

  stop_thread(midi_thread);

  std::string path(midi_path);

  midi_thread = std::jthread(
      [path = std::move(path)](std::stop_token stop_token) mutable {
        char program[] = "bbpl";
        char option[] = "-m";

        char *argv[] = {program, option, path.data()};

        cli_entry(3, argv, version_c_api, logo_c_api, stop_token);
      });
}

void midi_service_stop(void) { stop_thread(midi_thread); }

// ========================
// Piano Service
// ========================

void piano_service_start(void) {
  stop_thread(piano_thread);

  piano_thread = std::jthread([](std::stop_token stop_token) {
    char program[] = "bbpl";
    char option[] = "-p";

    char *argv[] = {program, option};

    cli_entry(2, argv, version_c_api, logo_c_api, stop_token);
  });
}

void piano_service_stop(void) { stop_thread(piano_thread); }

// ========================
// Keyboard Service
// ========================

void keyboard_service_start(void) {
  stop_thread(keyboard_thread);

  keyboard_thread = std::jthread([](std::stop_token stop_token) {
    char program[] = "bbpl";
    char option[] = "-k";

    char *argv[] = {program, option};

    cli_entry(2, argv, version_c_api, logo_c_api, stop_token);
  });
}

void keyboard_service_stop(void) { stop_thread(keyboard_thread); }

// ========================
// Export Service
// ========================

void export_service_start(const char *midi_path) {
  if (midi_path == nullptr) {
    return;
  }

  stop_thread(export_thread);

  std::string path(midi_path);

  export_thread = std::jthread(
      [path = std::move(path)](std::stop_token stop_token) mutable {
        char program[] = "bbpl";
        char option[] = "-e";

        char *argv[] = {program, option, path.data()};

        cli_entry(3, argv, version_c_api, logo_c_api, stop_token);
      });
}

void export_service_stop(void) { stop_thread(export_thread); }

// ========================
// Record Service
// ========================

void record_service_start(void) {
  stop_thread(record_thread);

  record_thread = std::jthread([](std::stop_token stop_token) {
    char program[] = "bbpl";
    char option[] = "-r";

    char *argv[] = {program, option};

    cli_entry(2, argv, version_c_api, logo_c_api, stop_token);
  });
}

void record_service_stop(void) { stop_thread(record_thread); }

// ========================
// Test Service
// ========================

void test_service_start(void) {
  stop_thread(test_thread);

  test_thread = std::jthread([](std::stop_token stop_token) {
    char program[] = "bbpl";
    char option[] = "-t";

    char *argv[] = {program, option};

    cli_entry(2, argv, version_c_api, logo_c_api, stop_token);
  });
}

void test_service_stop(void) { stop_thread(test_thread); }

// ========================
// Internal Test Service
// ========================

void internal_test_service_start(void) {
  stop_thread(internal_test_thread);

  internal_test_thread = std::jthread([](std::stop_token stop_token) {
    char program[] = "bbpl";
    char option[] = "-i";

    char *argv[] = {program, option};

    cli_entry(2, argv, version_c_api, logo_c_api, stop_token);
  });
}

void internal_test_service_stop(void) { stop_thread(internal_test_thread); }

// ========================
// Shutdown All Services
// ========================

void close_all_service(void) {
  midi_service_stop();
  piano_service_stop();
  keyboard_service_stop();
  export_service_stop();
  record_service_stop();
  test_service_stop();
  internal_test_service_stop();
}
