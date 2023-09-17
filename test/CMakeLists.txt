add_custom_command(
    OUTPUT
        ${CMAKE_CURRENT_SOURCE_DIR}/valid_tests.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/invalid_tests.cpp
    COMMAND ${PROJECT_SOURCE_DIR}/tool/gentests
    )

add_executable(test_toml
    parser.cpp
    valid_tests.cpp
    invalid_tests.cpp
    )
target_include_directories(test_toml PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(test_toml PRIVATE cxxflags toml gtest_main)


add_custom_target(run_toml_tests COMMAND test_toml)
