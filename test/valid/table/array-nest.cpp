TEST(valid_table_tests, test_array_nest)
{
    const string toml("[[albums]]\nname = \"Born to Run\"\n\n  [[albums.songs]]\n  name = \"Jungleland\"\n\n  [[albums.songs]]\n  name = \"Meeting Across the River\"\n\n[[albums]]\nname = \"Born in the USA\"\n  \n  [[albums.songs]]\n  name = \"Glory Days\"\n\n  [[albums.songs]]\n  name = \"Dancing in the Dark\"\n", 262);

    const Table expected = {{ std::string("albums", 6), Value(Array({Value(Table({{ std::string("name", 4), Value(std::string("Born to Run", 11)) },{ std::string("songs", 5), Value(Array({Value(Table({{ std::string("name", 4), Value(std::string("Jungleland", 10)) }})),Value(Table({{ std::string("name", 4), Value(std::string("Meeting Across the River", 24)) }}))})) }})),Value(Table({{ std::string("name", 4), Value(std::string("Born in the USA", 15)) },{ std::string("songs", 5), Value(Array({Value(Table({{ std::string("name", 4), Value(std::string("Glory Days", 10)) }})),Value(Table({{ std::string("name", 4), Value(std::string("Dancing in the Dark", 19)) }}))})) }}))})) }};

    assert_parsed(toml, expected);
}
