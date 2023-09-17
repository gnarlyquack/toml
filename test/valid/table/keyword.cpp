TEST(valid_table_tests, test_keyword)
{
    const string toml("[true]\n\n[false]\n\n[inf]\n\n[nan]\n\n\n", 32);

    const Table expected = {{ "true", Value(Table({})) },{ "false", Value(Table({})) },{ "inf", Value(Table({})) },{ "nan", Value(Table({})) }};

    assert_parsed(toml, expected);
}
