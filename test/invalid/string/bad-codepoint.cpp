TEST(invalid_string_tests, test_bad_codepoint)
{
    const string toml("invalid-codepoint = \"This string contains a non scalar unicode codepoint \\uD801\"\n", 81);

    const vector<Error> expected = {
        { 73, 1, 74, "Unicode codepoint U+D801 is not allowed." },
    };

    assert_errors(toml, expected);
}
