cmake_minimum_required(VERSION 3.25)
project(NESClone VERSION 0.4.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(NESCLONE_BUILD_TESTS "Build unit tests" ON)
option(NESCLONE_BUILD_SCRIPTING "Build NESLogic scripting prototype" ON)
option(NESCLONE_BUILD_TOOLS "Build ROM and asset diagnostic tools" ON)

add_library(nes_core
    src/apu/apu.cpp
    src/cartridge/ines.cpp
    src/cartridge/mapper_factory.cpp
    src/cartridge/nrom.cpp
    src/controller/controller.cpp
    src/cpu/cpu6502.cpp
    src/debugger/debugger.cpp
    src/debugger/disassembler.cpp
    src/frontend/input.cpp
    src/frontend/library_service.cpp
    src/frontend/settings_store.cpp
    src/frontend/xmb_model.cpp
    src/nes/bus.cpp
    src/nes/nes.cpp
    src/ppu/ppu.cpp
    src/video/framebuffer.cpp
)
target_include_directories(nes_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

if(MSVC)
    target_compile_options(nes_core PRIVATE /W4 /permissive-)
else()
    target_compile_options(nes_core PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
endif()

add_executable(nesclone src/main.cpp)
target_link_libraries(nesclone PRIVATE nes_core)

if(NESCLONE_BUILD_SCRIPTING)
    add_library(neslogic_runtime
        src/neslogic/value.cpp
        src/neslogic/lexer.cpp
        src/neslogic/parser.cpp
        src/neslogic/runtime.cpp
        src/neslogic/interpreter.cpp
    )
    target_include_directories(neslogic_runtime PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
    if(MSVC)
        target_compile_options(neslogic_runtime PRIVATE /W4 /permissive-)
    else()
        target_compile_options(neslogic_runtime PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
    endif()
    add_executable(neslogic src/neslogic/main.cpp)
    target_link_libraries(neslogic PRIVATE neslogic_runtime)
endif()

if(NESCLONE_BUILD_TOOLS)
    add_executable(nesrom tools/romtool/main.cpp)
    target_link_libraries(nesrom PRIVATE nes_core)
    add_executable(inesutil tools/inesutil/main.cpp)
    target_link_libraries(inesutil PRIVATE nes_core)
    add_executable(chrtool tools/chrtool/main.cpp)
    target_link_libraries(chrtool PRIVATE nes_core)
    add_executable(namtool tools/namtool/main.cpp)
    add_executable(nesdis tools/nesdis/main.cpp)
    target_link_libraries(nesdis PRIVATE nes_core)
endif()

if(NESCLONE_BUILD_TESTS)
    enable_testing()
    add_executable(nes_core_tests tests/test_main.cpp)
    target_link_libraries(nes_core_tests PRIVATE nes_core)
    add_test(NAME nes_core_tests COMMAND nes_core_tests)
    add_executable(nes_frontend_tests tests/frontend_tests.cpp)
    target_link_libraries(nes_frontend_tests PRIVATE nes_core)
    add_test(NAME nes_frontend_tests COMMAND nes_frontend_tests)
endif()
