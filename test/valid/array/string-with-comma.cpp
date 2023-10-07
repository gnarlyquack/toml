TEST(valid_array_tests, test_string_with_comma)
{
    const string toml("title = [\n\"Client: XXXX, Job: XXXX\",\n\"Code: XXXX\"\n]\n", 52);

    const Table expected = {{ std::string("title", 5), Value(Array({Value(std::string("Client: XXXX, Job: XXXX", 23)),Value(std::string("Code: XXXX", 10))})) }};

    assert_parsed(toml, expected);
}
