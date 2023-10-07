TEST(valid_array_tests, test_nested_inline_table)
{
    const string toml("a = [ { b = {} } ]\n", 19);

    const Table expected = {{ std::string("a", 1), Value(Array({Value(Table({{ std::string("b", 1), Value(Table({})) }}))})) }};

    assert_parsed(toml, expected);
}
