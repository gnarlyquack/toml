TEST(valid_table_tests, test_whitespace)
{
    const string toml("[\"valid key\"]\n", 14);

    const Table expected = {{ std::string("valid key", 9), Value(Table({})) }};

    assert_parsed(toml, expected);
}
