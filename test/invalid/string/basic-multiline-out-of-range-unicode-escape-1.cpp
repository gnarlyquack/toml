TEST(invalid_string_tests, test_basic_multiline_out_of_range_unicode_escape_1)
{
    const string toml("a = \"\"\"\\UFFFFFFFF\"\"\"\n", 21);

    const vector<Error> expected= {
        { 7, 1, 8, "Invalid Unicode codepoint: U+FFFFFFFF" },
    };

    assert_errors(toml, expected);
}
