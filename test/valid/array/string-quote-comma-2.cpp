TEST(valid_array_tests, test_string_quote_comma_2)
{
    const string toml("title = [ \" \\\", \",]\n", 20);

    const Table expected = {{ std::string("title", 5), Value(Array({Value(std::string(" \", ", 4))})) }};

    assert_parsed(toml, expected);
}
