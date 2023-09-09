TEST(valid_array_tests, test_string_quote_comma_2)
{
    const string toml{"title = [ \" \\\", \",]\n", 20};

    const Table expected{{ "title", Value(Array({Value(std::string(" \", "))})) }};

    assert_parsed(toml, expected);
}
