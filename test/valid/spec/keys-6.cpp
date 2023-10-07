TEST(valid_spec_tests, test_keys_6)
{
    const string toml("# RECOMMENDED\n\napple.type = \"fruit\"\napple.skin = \"thin\"\napple.color = \"red\"\n\norange.type = \"fruit\"\norange.skin = \"thick\"\norange.color = \"orange\"\n", 145);

    const Table expected = {{ std::string("apple", 5), Value(Table({{ std::string("color", 5), Value(std::string("red", 3)) },{ std::string("skin", 4), Value(std::string("thin", 4)) },{ std::string("type", 4), Value(std::string("fruit", 5)) }})) },{ std::string("orange", 6), Value(Table({{ std::string("color", 5), Value(std::string("orange", 6)) },{ std::string("skin", 4), Value(std::string("thick", 5)) },{ std::string("type", 4), Value(std::string("fruit", 5)) }})) }};

    assert_parsed(toml, expected);
}
