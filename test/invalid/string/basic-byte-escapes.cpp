TEST(invalid_string_tests, test_basic_byte_escapes)
{
    const string toml{"answer = \"\\x33\"\n", 16};

    const vector<Error> expected{{ 1, 11, "Invalid escape sequence." },};

    assert_errors(toml, expected);
}
