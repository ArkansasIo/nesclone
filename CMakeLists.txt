cmake_minimum_required(VERSION 3.25)
project(NESClone VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(NESCLONE_BUILD_TESTS "Build unit tests" ON)

add_library(nes_core
    src/cartridge/ines.cpp
    src/cartridge/nrom.cpp
    src/controller/controller.cpp
    src/cpu/cpu6502.cpp
    src/nes/bus.cpp
    src/nes/nes.cpp
)
target_include_directories(nes_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

if(MSVC)
    target_compile_options(nes_core PRIVATE /W4 /permissive-)
else()
    target_compile_options(nes_core PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
endif()

add_executable(nesclone src/main.cpp)
target_link_libraries(nesclone PRIVATE nes_core)

if(NESCLONE_BUILD_TESTS)
    enable_testing()
    add_executable(nes_core_tests tests/test_main.cpp)
    target_link_libraries(nes_core_tests PRIVATE nes_core)
    add_test(NAME nes_core_tests COMMAND nes_core_tests)
endif()
