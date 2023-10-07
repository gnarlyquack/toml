TEST(valid_key_tests, test_empty)
{
    const string toml("\"\" = \"blank\"\n", 13);

    const Table expected = {{ std::string("", 0), Value(std::string("blank", 5)) }};

    assert_parsed(toml, expected);
}
