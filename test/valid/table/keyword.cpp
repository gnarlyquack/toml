TEST(valid_table_tests, test_keyword)
{
    const string toml("[true]\n\n[false]\n\n[inf]\n\n[nan]\n\n\n", 32);

    const Table expected = {{ std::string("true", 4), Value(Table({})) },{ std::string("false", 5), Value(Table({})) },{ std::string("inf", 3), Value(Table({})) },{ std::string("nan", 3), Value(Table({})) }};

    assert_parsed(toml, expected);
}
