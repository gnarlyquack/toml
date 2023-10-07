TEST(valid_string_tests, test_escaped_escape)
{
    const string toml("answer = \"\\\\x64\"\n", 17);

    const Table expected = {{ std::string("answer", 6), Value(std::string("\\x64", 4)) }};

    assert_parsed(toml, expected);
}
