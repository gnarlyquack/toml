TEST(valid_array_tests, test_string_quote_comma_2)
{
    const string toml{"title = [ \" \\\", \",]\n", 20};

    const Table expected{{ "title", Value::of_array({Value::of_string(" \", ")}) }};

    assert_parsed(toml, expected);
}
