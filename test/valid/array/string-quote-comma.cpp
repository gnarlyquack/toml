TEST(valid_array_tests, test_string_quote_comma)
{
    const string toml("title = [\n\"Client: \\\"XXXX\\\", Job: XXXX\",\n\"Code: XXXX\"\n]\n", 56);

    const Table expected = {{ std::string("title", 5), Value(Array({Value(std::string("Client: \"XXXX\", Job: XXXX", 25)),Value(std::string("Code: XXXX", 10))})) }};

    assert_parsed(toml, expected);
}
