TEST(invalid_control_tests, test_string_bs)
{
    const string toml("string-bs = \"backspace\b\"\n", 25);

    const vector<Error> expected = {
        { 22, 1, 23, "Unicode codepoint U+0008 is not allowed." },
    };

    assert_errors(toml, expected);
}
