TEST(invalid_table_tests, test_rrbrace)
{
    const string toml("[[table] ]\n", 11);

    const vector<Error> expected = {
        { 8, 1, 9, "Expected closing ']' for table array header." },
    };

    assert_errors(toml, expected);
}
