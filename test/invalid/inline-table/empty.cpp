TEST(invalid_inline_table_tests, test_empty)
{
    const string toml("t = {,}\n", 8);

    const vector<Error> expected = {
        { 5, 1, 6, "Missing inline table value." },
        { 6, 1, 7, "Unexpected end of table: a value is required after a comma." },
    };

    assert_errors(toml, expected);
}
