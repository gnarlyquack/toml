TEST(valid_array_tests, test_strings)
{
    const string toml("string_array = [ \"all\", 'strings', \"\"\"are the same\"\"\", '''type''']\n", 67);

    const Table expected = {{ std::string("string_array", 12), Value(Array({Value(std::string("all", 3)),Value(std::string("strings", 7)),Value(std::string("are the same", 12)),Value(std::string("type", 4))})) }};

    assert_parsed(toml, expected);
}
