TEST(invalid_table_tests, test_nested_brackets_close)
{
    const string toml("[a]b]\nzyx = 42\n", 15);

    const vector<Error> expected = {
        { 3, 1, 4, "Expected the end of the line but got: b]" },
    };

    assert_errors(toml, expected);
}
