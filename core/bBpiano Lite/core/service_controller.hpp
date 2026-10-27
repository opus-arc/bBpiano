#ifndef controller_hardware_hpp
#define controller_hardware_hpp

#include <stop_token>
#include <string>
#include "bbpl/configuration/configuration.hpp"

// ======================== ======================== ========================
// Basic configurations
// 基础设置
// ======================== ======================== ========================
static constexpr double sample_rate = configuration.basic_presets.get_sample_rate();

// ======================== ======================== ========================
// Basic support
// 基础支持
// ======================== ======================== ========================
int cli_entry(int argc, char *argv[], const char *version, const char *logo,
              std::stop_token external_stop = {});

// ======================== ======================== ========================
// Services
// 服务
// ======================== ======================== ========================
void midi_service(const std::string &midi_path, std::stop_token stop_token);
void piano_service(std::stop_token stop_token);
void keyboard_service(std::stop_token stop_token);
void export_service(std::string export_midi_path_string,
                    std::stop_token stop_token, std::string output_folder = {});
void record_service(std::stop_token stop_token);
void test_service(std::stop_token stop_token);
void internal_test_service(std::stop_token stop_token);

#endif /* controller_hardware_hpp */
