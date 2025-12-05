cmake_minimum_required(VERSION 3.16)
project(OptimizingBytecodeCompiler LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler flags - warnings as errors
if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Werror -pedantic)
endif()

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Source files for main compiler executable
set(COMPILER_SOURCES
    src/main.cpp
    src/lexer.cpp
    src/parser.cpp
    src/ast.cpp
)

# Main compiler executable
add_executable(compiler ${COMPILER_SOURCES})
target_include_directories(compiler PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Testing setup with GoogleTest
include(FetchContent)
FetchContent_Declare(googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)

# Enable testing
enable_testing()

# Test executable
add_executable(tests
    tests/test_lexer.cpp
    tests/test_parser.cpp
    src/lexer.cpp
    src/parser.cpp
    src/ast.cpp
)

target_link_libraries(tests
    gtest_main
    gtest
)

target_include_directories(tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_test(NAME AllTests COMMAND tests)

# Custom test_all target
add_custom_target(test_all
    COMMAND ${CMAKE_CTEST_COMMAND} --verbose
    DEPENDS tests
)

# Optional: add compiler as dependency for potential integration tests
add_dependencies(test_all compiler)

# Print build configuration
message(STATUS "Optimizing Bytecode Compiler Configuration:")
message(STATUS "  Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Compiler: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "  Lexer: enabled")
message(STATUS "  Parser: enabled (skeleton)")
message(STATUS "  AST: enabled")
