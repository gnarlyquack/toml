TEST(valid_table_tests, test_no_eol)
{
    const string toml("[table]\n", 8);

    const Table expected = {{ "table", Value(Table({})) }};

    assert_parsed(toml, expected);
}
