TEST(valid_spec_tests, test_table_6)
{
    const string toml("# RECOMMENDED\n[fruit.apple]\n[fruit.orange]\n[animal]\n", 52);

    const Table expected = {{ std::string("animal", 6), Value(Table({})) },{ std::string("fruit", 5), Value(Table({{ std::string("apple", 5), Value(Table({})) },{ std::string("orange", 6), Value(Table({})) }})) }};

    assert_parsed(toml, expected);
}
