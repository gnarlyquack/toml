TEST(invalid_control_tests, test_string_null)
{
    const string toml{"string-null = \"null\000\"\n", 22};

    const vector<Error> expected{{ 1, 20, "Unicode codepoint U+0000 is not allowed." },};

    assert_errors(toml, expected);
}
