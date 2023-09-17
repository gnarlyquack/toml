TEST(valid_table_tests, test_array_one)
{
    const string toml("[[people]]\nfirst_name = \"Bruce\"\nlast_name = \"Springsteen\"\n", 58);

    const Table expected = {{ "people", Value(Array({Value(Table({{ "first_name", Value(std::string("Bruce")) },{ "last_name", Value(std::string("Springsteen")) }}))})) }};

    assert_parsed(toml, expected);
}
