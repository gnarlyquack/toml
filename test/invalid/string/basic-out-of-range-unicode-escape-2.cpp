TEST(invalid_string_tests, test_basic_out_of_range_unicode_escape_2)
{
    const string toml("a = \"\\U00D80000\"\n", 17);

    const vector<Error> expected = {
        { 5, 1, 6, "Invalid Unicode codepoint: U+D80000" },
    };

    assert_errors(toml, expected);
}
