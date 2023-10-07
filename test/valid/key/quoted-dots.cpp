TEST(valid_key_tests, test_quoted_dots)
{
    const string toml("plain = 1\n\"with.dot\" = 2\n\n[plain_table]\nplain = 3\n\"with.dot\" = 4\n\n[table.withdot]\nplain = 5\n\"key.with.dots\" = 6\n", 112);

    const Table expected = {{ std::string("plain", 5), Value(INT64_C(1)) },{ std::string("plain_table", 11), Value(Table({{ std::string("plain", 5), Value(INT64_C(3)) },{ std::string("with.dot", 8), Value(INT64_C(4)) }})) },{ std::string("table", 5), Value(Table({{ std::string("withdot", 7), Value(Table({{ std::string("key.with.dots", 13), Value(INT64_C(6)) },{ std::string("plain", 5), Value(INT64_C(5)) }})) }})) },{ std::string("with.dot", 8), Value(INT64_C(2)) }};

    assert_parsed(toml, expected);
}
