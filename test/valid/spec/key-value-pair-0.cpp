TEST(valid_spec_tests, test_key_value_pair_0)
{
    const string toml("key = \"value\"\n", 14);

    const Table expected = {{ std::string("key", 3), Value(std::string("value", 5)) }};

    assert_parsed(toml, expected);
}
