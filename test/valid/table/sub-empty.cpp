TEST(valid_table_tests, test_sub_empty)
{
    const string toml("[a]\n[a.b]\n", 10);

    const Table expected = {{ std::string("a", 1), Value(Table({{ std::string("b", 1), Value(Table({})) }})) }};

    assert_parsed(toml, expected);
}
