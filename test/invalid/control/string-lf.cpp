TEST(invalid_control_tests, test_string_lf)
{
    const string toml("string-lf = \"null\020\"\n", 20);

    const vector<Error> expected = {
        { 17, 1, 18, "Unicode codepoint U+0010 is not allowed." },
    };

    assert_errors(toml, expected);
}
