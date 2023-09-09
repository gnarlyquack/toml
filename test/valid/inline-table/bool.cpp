TEST(valid_inline_table_tests, test_bool)
{
    const string toml{"a = {a = true, b = false}\n", 26};

    const Table expected{{ "a", Value(Table({{ "a", Value(true) },{ "b", Value(false) }})) }};

    assert_parsed(toml, expected);
}
