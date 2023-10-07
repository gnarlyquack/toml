TEST(valid_string_tests, test_double_quote_escape)
{
    const string toml("test = \"\\\"one\\\"\"\n", 17);

    const Table expected = {{ std::string("test", 4), Value(std::string("\"one\"", 5)) }};

    assert_parsed(toml, expected);
}
