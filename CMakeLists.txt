cmake_minimum_required(VERSION 3.17)

project(toml VERSION 1.0.0 LANGUAGES CXX)


add_library(cxxflags INTERFACE)
target_compile_features(cxxflags INTERFACE cxx_std_11)
target_compile_options(cxxflags INTERFACE
    -g
    -Werror
    -Wall -Wextra -Wpedantic
    -Wcast-align
    -Wcast-qual
    -Wconversion
    -Wmissing-declarations
    -Wshadow
    -Wunused
    -Wvla
    -Wwrite-strings
    -Wzero-as-null-pointer-constant
    )


add_library(toml
    src/lexer.cpp
    src/parser.cpp
    )
target_link_libraries(toml PRIVATE cxxflags)


# Build instructions from googletest/googletest/README.md
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY  https://github.com/google/googletest.git
    # Last version to support C++11
    GIT_TAG         release-1.12.1
    )
set(BUILD_GMOCK OFF)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)


add_executable(test_toml
    test/lexer.cpp
    test/parser.cpp
    )
target_include_directories(test_toml PRIVATE src)
target_link_libraries(test_toml PRIVATE cxxflags toml gtest_main m)

add_custom_target(run_toml_tests COMMAND test_toml)
