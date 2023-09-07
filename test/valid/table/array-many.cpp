TEST(valid_table_tests, test_array_many)
{
    const string toml{"[[people]]\nfirst_name = \"Bruce\"\nlast_name = \"Springsteen\"\n\n[[people]]\nfirst_name = \"Eric\"\nlast_name = \"Clapton\"\n\n[[people]]\nfirst_name = \"Bob\"\nlast_name = \"Seger\"\n", 163};

    const Table expected{{ "people", Value::of_array({Value::of_table({{ "first_name", Value::of_string("Bruce") },{ "last_name", Value::of_string("Springsteen") }}),Value::of_table({{ "first_name", Value::of_string("Eric") },{ "last_name", Value::of_string("Clapton") }}),Value::of_table({{ "first_name", Value::of_string("Bob") },{ "last_name", Value::of_string("Seger") }})}) }};

    assert_parsed(toml, expected);
}
