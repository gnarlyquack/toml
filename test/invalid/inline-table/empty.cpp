TEST(invalid_inline_table_tests, test_empty)
{
    const string toml("t = {,}\n", 8);

    const vector<Error> expected = {
        { 5, 1, 6, "Missing inline table value." },
        { 5, 1, 6, "Trailing ',' is not allowed in an inline table." },
    };

    assert_errors(toml, expected);
}
