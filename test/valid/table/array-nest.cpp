TEST(valid_table_tests, test_array_nest)
{
    const string toml{"[[albums]]\nname = \"Born to Run\"\n\n  [[albums.songs]]\n  name = \"Jungleland\"\n\n  [[albums.songs]]\n  name = \"Meeting Across the River\"\n\n[[albums]]\nname = \"Born in the USA\"\n  \n  [[albums.songs]]\n  name = \"Glory Days\"\n\n  [[albums.songs]]\n  name = \"Dancing in the Dark\"\n", 262};

    const Table expected{{ "albums", Value(Array({Value(Table({{ "name", Value(std::string("Born to Run")) },{ "songs", Value(Array({Value(Table({{ "name", Value(std::string("Jungleland")) }})),Value(Table({{ "name", Value(std::string("Meeting Across the River")) }}))})) }})),Value(Table({{ "name", Value(std::string("Born in the USA")) },{ "songs", Value(Array({Value(Table({{ "name", Value(std::string("Glory Days")) }})),Value(Table({{ "name", Value(std::string("Dancing in the Dark")) }}))})) }}))})) }};

    assert_parsed(toml, expected);
}
