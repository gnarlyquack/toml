TEST(valid_table_tests, test_array_one)
{
    const string toml{"[[people]]\nfirst_name = \"Bruce\"\nlast_name = \"Springsteen\"\n", 58};

    const Table expected{{ "people", Value::of_array({Value::of_table({{ "first_name", Value::of_string("Bruce") },{ "last_name", Value::of_string("Springsteen") }})}) }};

    assert_parsed(toml, expected);
}
