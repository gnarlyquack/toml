TEST(valid_spec_tests, test_keys_4)
{
    const string toml("fruit.name = \"banana\"     # this is best practice\nfruit. color = \"yellow\"    # same as fruit.color\nfruit . flavor = \"banana\"   # same as fruit.flavor\n", 150);

    const Table expected = {{ std::string("fruit", 5), Value(Table({{ std::string("color", 5), Value(std::string("yellow", 6)) },{ std::string("flavor", 6), Value(std::string("banana", 6)) },{ std::string("name", 4), Value(std::string("banana", 6)) }})) }};

    assert_parsed(toml, expected);
}
