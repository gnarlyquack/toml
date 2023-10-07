TEST(valid_spec_tests, test_table_8)
{
    const string toml("fruit.apple.color = \"red\"\n# Defines a table named fruit\n# Defines a table named fruit.apple\n\nfruit.apple.taste.sweet = true\n# Defines a table named fruit.apple.taste\n# fruit and fruit.apple were already created\n", 211);

    const Table expected = {{ std::string("fruit", 5), Value(Table({{ std::string("apple", 5), Value(Table({{ std::string("color", 5), Value(std::string("red", 3)) },{ std::string("taste", 5), Value(Table({{ std::string("sweet", 5), Value(true) }})) }})) }})) }};

    assert_parsed(toml, expected);
}
