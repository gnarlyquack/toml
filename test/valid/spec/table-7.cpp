TEST(valid_spec_tests, test_table_7)
{
    const string toml("# Top-level table begins.\nname = \"Fido\"\nbreed = \"pug\"\n\n# Top-level table ends.\n[owner]\nname = \"Regina Dogman\"\nmember_since = 1999-08-04\n", 136);

    const Table expected = {{ std::string("breed", 5), Value(std::string("pug", 3)) },{ std::string("name", 4), Value(std::string("Fido", 4)) },{ std::string("owner", 5), Value(Table({{ std::string("member_since", 12), Value(LocalDate(date::year(1999) / date::month(8) / date::day(4))) },{ std::string("name", 4), Value(std::string("Regina Dogman", 13)) }})) }};

    assert_parsed(toml, expected);
}
