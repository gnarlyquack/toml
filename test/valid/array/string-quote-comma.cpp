TEST(valid_array_tests, test_string_quote_comma)
{
    const string toml{"title = [\n\"Client: \\\"XXXX\\\", Job: XXXX\",\n\"Code: XXXX\"\n]\n", 56};

    const Table expected{{ "title", Value(Array({Value(std::string("Client: \"XXXX\", Job: XXXX")),Value(std::string("Code: XXXX"))})) }};

    assert_parsed(toml, expected);
}
