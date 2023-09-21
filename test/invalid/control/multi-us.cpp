TEST(invalid_control_tests, test_multi_us)
{
    const string toml("multi-us = \"\"\"null\037\"\"\"\n", 23);

    const vector<Error> expected = {
        { 18, 1, 19, "Unicode codepoint U+001F is not allowed." },
    };

    assert_errors(toml, expected);
}
