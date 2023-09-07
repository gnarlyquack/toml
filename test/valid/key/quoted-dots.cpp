TEST(valid_key_tests, test_quoted_dots)
{
    const string toml{"plain = 1\n\"with.dot\" = 2\n\n[plain_table]\nplain = 3\n\"with.dot\" = 4\n\n[table.withdot]\nplain = 5\n\"key.with.dots\" = 6\n", 112};

    const Table expected{{ "plain", Value::of_integer(1ULL) },{ "plain_table", Value::of_table({{ "plain", Value::of_integer(3ULL) },{ "with.dot", Value::of_integer(4ULL) }}) },{ "table", Value::of_table({{ "withdot", Value::of_table({{ "key.with.dots", Value::of_integer(6ULL) },{ "plain", Value::of_integer(5ULL) }}) }}) },{ "with.dot", Value::of_integer(2ULL) }};

    assert_parsed(toml, expected);
}
