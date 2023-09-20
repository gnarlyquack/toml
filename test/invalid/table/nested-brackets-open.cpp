TEST(invalid_table_tests, test_nested_brackets_open)
{
    const string toml("[a[b]\nzyx = 42\n", 15);

    const vector<Error> expected= {
        { 1, 1, 2, "Invalid key: a[b" },
    };

    assert_errors(toml, expected);
}
