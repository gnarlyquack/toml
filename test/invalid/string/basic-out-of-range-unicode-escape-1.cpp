TEST(invalid_string_tests, test_basic_out_of_range_unicode_escape_1)
{
    const string toml("a = \"\\UFFFFFFFF\"\n", 17);

    const vector<Error> expected= {
        { 5, 1, 6, "Invalid Unicode codepoint: U+FFFFFFFF" },
    };

    assert_errors(toml, expected);
}
