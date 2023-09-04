TEST(invalid_string_tests, test_multiline_escape_space)
{
    const string toml{"a = \"\"\"\n  foo \\ \\n\n  bar\"\"\"\n", 28};

    const vector<Error> expected{{ 2, 7, "Invalid escape sequence." },};

    assert_errors(toml, expected);
}
