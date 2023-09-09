TEST(valid_table_tests, test_array_implicit)
{
    const string toml{"[[albums.songs]]\nname = \"Glory Days\"\n", 37};

    const Table expected{{ "albums", Value(Table({{ "songs", Value(Array({Value(Table({{ "name", Value(std::string("Glory Days")) }}))})) }})) }};

    assert_parsed(toml, expected);
}
