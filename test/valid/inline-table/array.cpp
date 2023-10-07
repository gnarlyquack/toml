TEST(valid_inline_table_tests, test_array)
{
    const string toml("people = [{first_name = \"Bruce\", last_name = \"Springsteen\"},\n          {first_name = \"Eric\", last_name = \"Clapton\"},\n          {first_name = \"Bob\", last_name = \"Seger\"}]\n", 170);

    const Table expected = {{ std::string("people", 6), Value(Array({Value(Table({{ std::string("first_name", 10), Value(std::string("Bruce", 5)) },{ std::string("last_name", 9), Value(std::string("Springsteen", 11)) }})),Value(Table({{ std::string("first_name", 10), Value(std::string("Eric", 4)) },{ std::string("last_name", 9), Value(std::string("Clapton", 7)) }})),Value(Table({{ std::string("first_name", 10), Value(std::string("Bob", 3)) },{ std::string("last_name", 9), Value(std::string("Seger", 5)) }}))})) }};

    assert_parsed(toml, expected);
}
