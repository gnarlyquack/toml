    add_executable(test_toml
        lexer.cpp
        parser.cpp
        valid_table_tests.cpp
        valid_float_tests.cpp
        valid_integer_tests.cpp
        valid_key_tests.cpp
        valid_inline_table_tests.cpp
        valid_bool_tests.cpp
        valid_datetime_tests.cpp
        valid_array_tests.cpp
        valid_string_tests.cpp
        valid_comment_tests.cpp
        valid_tests.cpp
        invalid_table_tests.cpp
        #invalid_float_tests.cpp
        #invalid_encoding_tests.cpp
        #invalid_integer_tests.cpp
        #invalid_key_tests.cpp
        #invalid_inline_table_tests.cpp
        #invalid_bool_tests.cpp
        #invalid_datetime_tests.cpp
        #invalid_array_tests.cpp
        #invalid_string_tests.cpp
        #invalid_control_tests.cpp
        )
    target_include_directories(test_toml PRIVATE ${PROJECT_SOURCE_DIR}/src)
    target_link_libraries(test_toml PRIVATE cxxflags toml gtest_main)

    add_custom_target(run_toml_tests COMMAND test_toml)
