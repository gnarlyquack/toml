TEST(valid_array_tests, test_table_array_string_backslash)
{
    const string toml("foo = [ { bar=\"\\\"{{baz}}\\\"\"} ]\n", 31);

    const Table expected = {{ std::string("foo", 3), Value(Array({Value(Table({{ std::string("bar", 3), Value(std::string("\"{{baz}}\"", 9)) }}))})) }};

    assert_parsed(toml, expected);
}
