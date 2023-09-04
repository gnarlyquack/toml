TEST(invalid_inline_table_tests, test_empty)
{
    const string toml{"t = {,}\n", 8};

    const vector<Error> expected{
        { 1, 6, "Missing value for inline table." },
        { 1, 6, "Trailing ',' is not allowed in an inline table." },
    };

    assert_errors(toml, expected);
}
