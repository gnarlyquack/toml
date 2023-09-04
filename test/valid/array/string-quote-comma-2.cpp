TEST(valid_array_tests, test_string_quote_comma_2)
{
    const string toml{"title = [ \" \\\", \",]\n", 20};

    const Table expected{{ "title", new ArrayValue({new StringValue(" \", "),}) },};

    assert_parsed(toml, expected);
}
