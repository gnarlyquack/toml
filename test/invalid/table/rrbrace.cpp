TEST(invalid_table_tests, test_rrbrace)
{
    const string toml("[[table] ]\n", 11);

    const vector<Error> expected = {
        { 7, 1, 8, "Expected closing ']]' for table array header." },
    };

    assert_errors(toml, expected);
}
