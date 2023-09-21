TEST(invalid_control_tests, test_multi_null)
{
    const string toml("multi-null = \"\"\"null\000\"\"\"\n", 25);

    const vector<Error> expected = {
        { 20, 1, 21, "Unicode codepoint U+0000 is not allowed." },
    };

    assert_errors(toml, expected);
}
