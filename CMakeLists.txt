cmake_minimum_required(VERSION 3.16)
project(OptimizingBytecodeCompiler LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Source files for main compiler executable
set(COMPILER_SOURCES
    src/main.cpp
    src/lexer.cpp
    src/parser.cpp
    src/ast.cpp
    src/codegen.cpp
    src/vm.cpp
    src/optimizer.cpp
)

# Library sources (shared between compiler and tests)
set(LIB_SOURCES
    src/lexer.cpp
    src/parser.cpp
    src/ast.cpp
    src/codegen.cpp
    src/vm.cpp
    src/optimizer.cpp
)

# Main compiler executable
add_executable(compiler ${COMPILER_SOURCES})
target_include_directories(compiler PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

if(MSVC)
    target_compile_options(compiler PRIVATE /W4 /WX)
else()
    target_compile_options(compiler PRIVATE -Wall -Wextra -Werror -pedantic)
endif()

# Testing setup
include(FetchContent)
FetchContent_Declare(googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Enable testing
enable_testing()

add_executable(tests
    tests/test_lexer.cpp
    tests/test_parser.cpp
    tests/test_codegen.cpp
    tests/test_vm.cpp
    tests/test_optimizer.cpp
    tests/test_end_to_end.cpp
    tests/test_control_flow.cpp
    tests/test_arrays.cpp
    tests/test_bubblesort.cpp
    ${LIB_SOURCES}
)

# Link to gtest
target_link_libraries(tests PRIVATE GTest::gtest GTest::gtest_main)

target_include_directories(tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Note: We rely on GTest::gtest target to provide correct include directories
# to avoid conflicts with system headers.

if(MSVC)
    target_compile_options(tests PRIVATE /W4 /WX)
else()
    # Note: We relax -Werror for tests to avoid issues with GTest macros
    target_compile_options(tests PRIVATE -Wall -Wextra -pedantic)
endif()

add_test(NAME AllTests COMMAND tests)
