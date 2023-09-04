TEST(valid_inline_table_tests, test_array)
{
    const string toml{"people = [{first_name = \"Bruce\", last_name = \"Springsteen\"},\n          {first_name = \"Eric\", last_name = \"Clapton\"},\n          {first_name = \"Bob\", last_name = \"Seger\"}]\n", 170};

    const Table expected{{ "people", new ArrayValue({new TableValue({{ "first_name", new StringValue("Bruce") },{ "last_name", new StringValue("Springsteen") },}),new TableValue({{ "first_name", new StringValue("Eric") },{ "last_name", new StringValue("Clapton") },}),new TableValue({{ "first_name", new StringValue("Bob") },{ "last_name", new StringValue("Seger") },}),}) },};

    assert_parsed(toml, expected);
}
