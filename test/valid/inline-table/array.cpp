TEST(valid_inline_table_tests, test_array)
{
    const string toml{"people = [{first_name = \"Bruce\", last_name = \"Springsteen\"},\n          {first_name = \"Eric\", last_name = \"Clapton\"},\n          {first_name = \"Bob\", last_name = \"Seger\"}]\n", 170};

    const Table expected{{ "people", Value::of_array({Value::of_table({{ "first_name", Value::of_string("Bruce") },{ "last_name", Value::of_string("Springsteen") }}),Value::of_table({{ "first_name", Value::of_string("Eric") },{ "last_name", Value::of_string("Clapton") }}),Value::of_table({{ "first_name", Value::of_string("Bob") },{ "last_name", Value::of_string("Seger") }})}) }};

    assert_parsed(toml, expected);
}
