TEST(invalid_control_tests, test_multi_lf)
{
    const string toml("multi-lf = \"\"\"null\020\"\"\"\n", 23);

    const vector<Error> expected = {
        { 18, 1, 19, "Unicode codepoint U+0010 is not allowed." },
    };

    assert_errors(toml, expected);
}
