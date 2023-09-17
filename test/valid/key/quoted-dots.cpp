TEST(valid_key_tests, test_quoted_dots)
{
    const string toml("plain = 1\n\"with.dot\" = 2\n\n[plain_table]\nplain = 3\n\"with.dot\" = 4\n\n[table.withdot]\nplain = 5\n\"key.with.dots\" = 6\n", 112);

    const Table expected = {{ "plain", Value(INT64_C(1)) },{ "plain_table", Value(Table({{ "plain", Value(INT64_C(3)) },{ "with.dot", Value(INT64_C(4)) }})) },{ "table", Value(Table({{ "withdot", Value(Table({{ "key.with.dots", Value(INT64_C(6)) },{ "plain", Value(INT64_C(5)) }})) }})) },{ "with.dot", Value(INT64_C(2)) }};

    assert_parsed(toml, expected);
}
