TEST(valid_table_tests, test_empty)
{
    const string toml("[a]\n", 4);

    const Table expected = {{ "a", Value(Table({})) }};

    assert_parsed(toml, expected);
}
