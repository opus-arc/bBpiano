cmake_minimum_required(VERSION 3.20)
project(bBpianoLite VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(.)
include_directories(core)
include_directories(core/piano)
include_directories(core/piano/Component)
include_directories(core/piano/ModelParameters)
include_directories(core/piano/Utils)
include_directories(external)
include_directories(hardware)

add_executable(bbl
        core/piano/Component/Src/DamperModel.cpp
        core/piano/Component/Src/HammerModel.cpp
        core/piano/Component/Src/KeyModel.cpp
        core/piano/Component/Src/PedalModel.cpp
        core/piano/Component/Src/StringModel.cpp
        core/piano/Component/DamperModel.hpp
        core/piano/Component/HammerModel.hpp
        core/piano/Component/KeyModel.hpp
        core/piano/Component/PedalModel.hpp
        core/piano/Component/StringModel.hpp
        core/piano/ModelParameters/ModelParameters.cpp
        core/piano/ModelParameters/ModelParameters.hpp
        core/piano/ModelParameters/PrecomputedValue.cpp
        core/piano/ModelParameters/PrecomputedValue.hpp
        core/piano/Utils/MyCSVReader.cpp
        core/piano/Utils/MyCSVReader.hpp
        core/piano/PianoModel.cpp
        core/piano/PianoModel.hpp
        core/controller.cpp
        core/controller.hpp
        external/RtAudio.cpp
        external/RtAudio.h
        external/RtMidi.cpp
        external/RtMidi.h
        hardware/midiKeyboard.hpp
        hardware/midiService.hpp
        hardware/pcKeyboard.hpp
        hardware/soundCard.hpp
        main.cpp
        main.hpp)

set_target_properties(bbl PROPERTIES
        OUTPUT_NAME "bbl"
)

# Backward-compatible target name for existing CLion run/build configurations.
# The actual executable target remains `bbl`.
add_custom_target(bBpiano_Lite_1_0_0 DEPENDS bbl)

if(WIN32)
        target_compile_definitions(bbl PRIVATE BBPIANO_PLATFORM_WINDOWS)
elseif(APPLE)
        target_compile_definitions(bbl PRIVATE BBPIANO_PLATFORM_MACOS)
elseif(UNIX)
        target_compile_definitions(bbl PRIVATE BBPIANO_PLATFORM_LINUX)
endif()
