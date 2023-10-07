TEST(valid_table_tests, test_array_one)
{
    const string toml("[[people]]\nfirst_name = \"Bruce\"\nlast_name = \"Springsteen\"\n", 58);

    const Table expected = {{ std::string("people", 6), Value(Array({Value(Table({{ std::string("first_name", 10), Value(std::string("Bruce", 5)) },{ std::string("last_name", 9), Value(std::string("Springsteen", 11)) }}))})) }};

    assert_parsed(toml, expected);
}
