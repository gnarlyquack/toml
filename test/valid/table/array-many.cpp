TEST(valid_table_tests, test_array_many)
{
    const string toml{"[[people]]\nfirst_name = \"Bruce\"\nlast_name = \"Springsteen\"\n\n[[people]]\nfirst_name = \"Eric\"\nlast_name = \"Clapton\"\n\n[[people]]\nfirst_name = \"Bob\"\nlast_name = \"Seger\"\n", 163};

    const Table expected{{ "people", new ArrayValue({new TableValue({{ "first_name", new StringValue("Bruce") },{ "last_name", new StringValue("Springsteen") },}),new TableValue({{ "first_name", new StringValue("Eric") },{ "last_name", new StringValue("Clapton") },}),new TableValue({{ "first_name", new StringValue("Bob") },{ "last_name", new StringValue("Seger") },}),}) },};

    assert_parsed(toml, expected);
}
