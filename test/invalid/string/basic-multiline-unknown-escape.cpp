TEST(invalid_string_tests, test_basic_multiline_unknown_escape)
{
    const string toml{"a = \"\"\"\\@\"\"\"\n", 13};

    const vector<Error> expected{{ 1, 8, "Invalid escape sequence." },};

    assert_errors(toml, expected);
}
