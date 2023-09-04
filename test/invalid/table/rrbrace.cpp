TEST(invalid_table_tests, test_rrbrace)
{
    const string toml{"[[table] ]\n", 11};

    const vector<Error> expected{
        { 1, 8, "Missing closing ']' for table array header." },
        { 1, 10, "Expected the end of the line but got: ]" },
    };

    assert_errors(toml, expected);
}
