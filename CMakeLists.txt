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


set(CMAKE_POLICY_DEFAULT_CMP0077 NEW) # for datetime lib
include(FetchContent)


find_package(Python)


# Build instructions from date/CMakeLists.txt
FetchContent_Declare(
    datetime
    GIT_REPOSITORY  https://github.com/HowardHinnant/date.git
    GIT_TAG         v3.0.1
    #GIT_TAG         master
    )
set(BUILD_TZ_LIB ON)
set(ENABLE_DATE_TESTING ON)
set(USE_SYSTEM_TZ_DB ON)
FetchContent_MakeAvailable(datetime)


add_library(toml
    src/lexer.cpp
    src/parser.cpp
    )
target_link_libraries(toml PUBLIC date-tz PRIVATE cxxflags)


# Build instructions from googletest/googletest/README.md
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


add_subdirectory(test)
