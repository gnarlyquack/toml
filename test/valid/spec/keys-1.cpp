TEST(valid_spec_tests, test_keys_1)
{
    const string toml("\"127.0.0.1\" = \"value\"\n\"character encoding\" = \"value\"\n\"\312\216\307\235\312\236\" = \"value\"\n'key2' = \"value\"\n'quoted \"value\"' = \"value\"\n", 116);

    const Table expected = {{ std::string("127.0.0.1", 9), Value(std::string("value", 5)) },{ std::string("character encoding", 18), Value(std::string("value", 5)) },{ std::string("key2", 4), Value(std::string("value", 5)) },{ std::string("quoted \"value\"", 14), Value(std::string("value", 5)) },{ std::string("ʎǝʞ", 6), Value(std::string("value", 5)) }};

    assert_parsed(toml, expected);
}
