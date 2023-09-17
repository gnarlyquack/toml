TEST(valid_table_tests, test_array_many)
{
    const string toml("[[people]]\nfirst_name = \"Bruce\"\nlast_name = \"Springsteen\"\n\n[[people]]\nfirst_name = \"Eric\"\nlast_name = \"Clapton\"\n\n[[people]]\nfirst_name = \"Bob\"\nlast_name = \"Seger\"\n", 163);

    const Table expected = {{ "people", Value(Array({Value(Table({{ "first_name", Value(std::string("Bruce")) },{ "last_name", Value(std::string("Springsteen")) }})),Value(Table({{ "first_name", Value(std::string("Eric")) },{ "last_name", Value(std::string("Clapton")) }})),Value(Table({{ "first_name", Value(std::string("Bob")) },{ "last_name", Value(std::string("Seger")) }}))})) }};

    assert_parsed(toml, expected);
}
