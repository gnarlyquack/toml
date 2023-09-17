TEST(valid_inline_table_tests, test_array)
{
    const string toml("people = [{first_name = \"Bruce\", last_name = \"Springsteen\"},\n          {first_name = \"Eric\", last_name = \"Clapton\"},\n          {first_name = \"Bob\", last_name = \"Seger\"}]\n", 170);

    const Table expected = {{ "people", Value(Array({Value(Table({{ "first_name", Value(std::string("Bruce")) },{ "last_name", Value(std::string("Springsteen")) }})),Value(Table({{ "first_name", Value(std::string("Eric")) },{ "last_name", Value(std::string("Clapton")) }})),Value(Table({{ "first_name", Value(std::string("Bob")) },{ "last_name", Value(std::string("Seger")) }}))})) }};

    assert_parsed(toml, expected);
}
