TEST(valid_key_tests, test_empty)
{
    const string toml("\"\" = \"blank\"\n", 13);

    const Table expected = {{ "", Value(std::string("blank")) }};

    assert_parsed(toml, expected);
}
