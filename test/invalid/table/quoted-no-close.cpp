TEST(invalid_table_tests, test_quoted_no_close)
{
    const string toml{"[\"where will it end]\nname = value\n", 34};

    const vector<Error> expected{
        { 1, 21, "Unterminated string." },
        { 1, 21, "Missing closing ']' for table header." },
        { 2, 8, "Invalid value: value" },
    };

    assert_errors(toml, expected);
}
