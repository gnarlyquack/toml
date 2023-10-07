TEST(valid_spec_tests, test_string_1)
{
    const string toml("str1 = \"\"\"\nRoses are red\nViolets are blue\"\"\"\n", 45);

    const Table expected = {{ std::string("str1", 4), Value(std::string("Roses are red\nViolets are blue", 30)) }};

    assert_parsed(toml, expected);
}
