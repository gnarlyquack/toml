TEST(valid_inline_table_tests, test_bool)
{
    const string toml{"a = {a = true, b = false}\n", 26};

    const Table expected{{ "a", Value::of_table({{ "a", Value::of_boolean(true) },{ "b", Value::of_boolean(false) }}) }};

    assert_parsed(toml, expected);
}
