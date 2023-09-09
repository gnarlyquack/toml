TEST(valid_array_tests, test_strings)
{
    const string toml{"string_array = [ \"all\", 'strings', \"\"\"are the same\"\"\", '''type''']\n", 67};

    const Table expected{{ "string_array", Value(Array({Value(std::string("all")),Value(std::string("strings")),Value(std::string("are the same")),Value(std::string("type"))})) }};

    assert_parsed(toml, expected);
}
