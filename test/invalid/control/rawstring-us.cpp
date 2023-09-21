TEST(invalid_control_tests, test_rawstring_us)
{
    const string toml("rawstring-us = 'null\037'\n", 23);

    const vector<Error> expected = {
        { 20, 1, 21, "Unicode codepoint U+001F is not allowed." },
    };

    assert_errors(toml, expected);
}
