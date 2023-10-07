TEST(valid_table_tests, test_array_many)
{
    const string toml("[[people]]\nfirst_name = \"Bruce\"\nlast_name = \"Springsteen\"\n\n[[people]]\nfirst_name = \"Eric\"\nlast_name = \"Clapton\"\n\n[[people]]\nfirst_name = \"Bob\"\nlast_name = \"Seger\"\n", 163);

    const Table expected = {{ std::string("people", 6), Value(Array({Value(Table({{ std::string("first_name", 10), Value(std::string("Bruce", 5)) },{ std::string("last_name", 9), Value(std::string("Springsteen", 11)) }})),Value(Table({{ std::string("first_name", 10), Value(std::string("Eric", 4)) },{ std::string("last_name", 9), Value(std::string("Clapton", 7)) }})),Value(Table({{ std::string("first_name", 10), Value(std::string("Bob", 3)) },{ std::string("last_name", 9), Value(std::string("Seger", 5)) }}))})) }};

    assert_parsed(toml, expected);
}
