TEST(valid_spec_tests, test_table_0)
{
    const string toml("[table]\n", 8);

    const Table expected = {{ std::string("table", 5), Value(Table({})) }};

    assert_parsed(toml, expected);
}
