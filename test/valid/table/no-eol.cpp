TEST(valid_table_tests, test_no_eol)
{
    const string toml("[table]\n", 8);

    const Table expected = {{ std::string("table", 5), Value(Table({})) }};

    assert_parsed(toml, expected);
}
