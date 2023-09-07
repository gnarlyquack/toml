TEST(valid_array_tests, test_nested_inline_table)
{
    const string toml{"a = [ { b = {} } ]\n", 19};

    const Table expected{{ "a", Value::of_array({Value::of_table({{ "b", Value::of_table({}) }})}) }};

    assert_parsed(toml, expected);
}
