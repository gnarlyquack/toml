TEST(valid_array_tests, test_string_with_comma_2)
{
    const string toml("title = [\n\"\"\"Client: XXXX,\nJob: XXXX\"\"\",\n\"Code: XXXX\"\n]\n", 56);

    const Table expected = {{ std::string("title", 5), Value(Array({Value(std::string("Client: XXXX,\nJob: XXXX", 23)),Value(std::string("Code: XXXX", 10))})) }};

    assert_parsed(toml, expected);
}
