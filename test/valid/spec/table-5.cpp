TEST(valid_spec_tests, test_table_5)
{
    const string toml("# VALID BUT DISCOURAGED\n[fruit.apple]\n[animal]\n[fruit.orange]\n", 62);

    const Table expected = {{ std::string("animal", 6), Value(Table({})) },{ std::string("fruit", 5), Value(Table({{ std::string("apple", 5), Value(Table({})) },{ std::string("orange", 6), Value(Table({})) }})) }};

    assert_parsed(toml, expected);
}
