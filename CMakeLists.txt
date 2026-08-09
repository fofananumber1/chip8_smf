cmake_minimum_required(VERSION 4.4.0)

project(CHIP-8)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(CHIP-8)

target_sources(CHIP-8
  PRIVATE
    src/main.cpp
    src/emu.cpp
)

add_executable(CHIP-8-tests)

target_sources(CHIP-8-tests
  PRIVATE
    tests/tests.cpp
    src/emu.cpp
)
