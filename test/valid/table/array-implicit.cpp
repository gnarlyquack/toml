TEST(valid_table_tests, test_array_implicit)
{
    const string toml("[[albums.songs]]\nname = \"Glory Days\"\n", 37);

    const Table expected = {{ std::string("albums", 6), Value(Table({{ std::string("songs", 5), Value(Array({Value(Table({{ std::string("name", 4), Value(std::string("Glory Days", 10)) }}))})) }})) }};

    assert_parsed(toml, expected);
}
