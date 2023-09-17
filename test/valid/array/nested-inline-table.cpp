TEST(valid_array_tests, test_nested_inline_table)
{
    const string toml("a = [ { b = {} } ]\n", 19);

    const Table expected = {{ "a", Value(Array({Value(Table({{ "b", Value(Table({})) }}))})) }};

    assert_parsed(toml, expected);
}
