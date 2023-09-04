TEST(valid_key_tests, test_quoted_dots)
{
    const string toml{"plain = 1\n\"with.dot\" = 2\n\n[plain_table]\nplain = 3\n\"with.dot\" = 4\n\n[table.withdot]\nplain = 5\n\"key.with.dots\" = 6\n", 112};

    const Table expected{{ "plain", new IntegerValue(1ULL) },{ "plain_table", new TableValue({{ "plain", new IntegerValue(3ULL) },{ "with.dot", new IntegerValue(4ULL) },}) },{ "table", new TableValue({{ "withdot", new TableValue({{ "key.with.dots", new IntegerValue(6ULL) },{ "plain", new IntegerValue(5ULL) },}) },}) },{ "with.dot", new IntegerValue(2ULL) },};

    assert_parsed(toml, expected);
}
