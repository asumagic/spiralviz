cmake_minimum_required(VERSION 3.16)

project(spiralviz LANGUAGES CXX)

# https://github.com/eliasdaler/imgui-sfml-fetchcontent/tree/master
# TODO: doesn't fucking work
set(USE_SYSTEM_DEPS ON CACHE BOOL "Use system SFML by default")
set(SFML_BUILD_AUDIO ON CACHE BOOL "Compile SFML with audio support if we must")
add_subdirectory(deps/imgui-sfml)

find_package(PkgConfig REQUIRED)
pkg_search_module(FFTWF REQUIRED fftw3f IMPORTED_TARGET)
include_directories(PkgConfig::FFTWF)
link_libraries(PkgConfig::FFTWF)

add_executable(
    spiralviz
    src/main.cpp
    src/app.cpp
    src/fftstreamer.cpp
    src/audio/recorder.cpp
    src/gui/fftdebug.cpp
    src/gui/vizshader.cpp
    src/dsp/windowedfft.cpp
    src/dsp/windowfuncs.cpp
)

set_property(TARGET spiralviz PROPERTY CXX_STANDARD 20)

target_precompile_headers(
    spiralviz
    PUBLIC
    "include/spiralviz/pch.hpp"
)

target_compile_options(
    spiralviz
    PUBLIC
    "-Wall"
    "-Wextra"
    "-Wpedantic"
    "-gsplit-dwarf"
    "-O1" # lto blesses us
)

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(
        spiralviz
        PUBLIC
        "-fpch-instantiate-templates"
        "-flto=thin"
    )

    target_link_options(
        spiralviz
        PUBLIC
        "-flto=thin"
        "-Wl,--thinlto-cache-dir=${PROJECT_BINARY_DIR}/lto.cache"
    )
endif()

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(
        spiralviz
        PUBLIC
        "_GLIBCXX_DEBUG"
    )

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address,undefined")
endif()

target_link_libraries(
    spiralviz
    PUBLIC
    # fmt
    # spdlog
    ImGui-SFML::ImGui-SFML
    sfml-audio # TODO: remove this?
    PkgConfig::FFTWF
)

target_include_directories(
    spiralviz
    PUBLIC
    include
)

target_link_options(
    spiralviz
    PUBLIC
    "-fuse-ld=mold"
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)