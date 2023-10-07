TEST(valid_inline_table_tests, test_bool)
{
    const string toml("a = {a = true, b = false}\n", 26);

    const Table expected = {{ std::string("a", 1), Value(Table({{ std::string("a", 1), Value(true) },{ std::string("b", 1), Value(false) }})) }};

    assert_parsed(toml, expected);
}
