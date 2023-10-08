TEST(invalid_inline_table_tests, test_empty)
{
    const string toml("t = {,}\n", 8);

    const vector<Error> expected = {
        { 5, 1, 6, "Expected a key-value pair." },
        { 6, 1, 7, "Expected a key-value pair." },
    };

    assert_errors(toml, expected);
}
