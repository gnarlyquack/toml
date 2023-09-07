TEST(valid_array_tests, test_table_array_string_backslash)
{
    const string toml{"foo = [ { bar=\"\\\"{{baz}}\\\"\"} ]\n", 31};

    const Table expected{{ "foo", Value::of_array({Value::of_table({{ "bar", Value::of_string("\"{{baz}}\"") }})}) }};

    assert_parsed(toml, expected);
}
