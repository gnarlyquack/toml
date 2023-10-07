TEST(valid_spec_tests, test_keys_5)
{
    const string toml("# VALID BUT DISCOURAGED\n\napple.type = \"fruit\"\norange.type = \"fruit\"\n\napple.skin = \"thin\"\norange.skin = \"thick\"\n\napple.color = \"red\"\norange.color = \"orange\"\n", 156);

    const Table expected = {{ std::string("apple", 5), Value(Table({{ std::string("color", 5), Value(std::string("red", 3)) },{ std::string("skin", 4), Value(std::string("thin", 4)) },{ std::string("type", 4), Value(std::string("fruit", 5)) }})) },{ std::string("orange", 6), Value(Table({{ std::string("color", 5), Value(std::string("orange", 6)) },{ std::string("skin", 4), Value(std::string("thick", 5)) },{ std::string("type", 4), Value(std::string("fruit", 5)) }})) }};

    assert_parsed(toml, expected);
}
