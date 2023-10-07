TEST(valid_spec_tests, test_keys_0)
{
    const string toml("key = \"value\"\nbare_key = \"value\"\nbare-key = \"value\"\n1234 = \"value\"\n", 67);

    const Table expected = {{ std::string("1234", 4), Value(std::string("value", 5)) },{ std::string("bare-key", 8), Value(std::string("value", 5)) },{ std::string("bare_key", 8), Value(std::string("value", 5)) },{ std::string("key", 3), Value(std::string("value", 5)) }};

    assert_parsed(toml, expected);
}
