TEST(invalid_string_tests, test_basic_multiline_out_of_range_unicode_escape_2)
{
    const string toml("a = \"\"\"\\U00D80000\"\"\"\n", 21);

    const vector<Error> expected = {
        { 7, 1, 8, "Unicode escape sequence specified an invalid or non-allowed codepoint." },
    };

    assert_errors(toml, expected);
}
