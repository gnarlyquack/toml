TEST(valid_table_tests, test_array_nest)
{
    const string toml{"[[albums]]\nname = \"Born to Run\"\n\n  [[albums.songs]]\n  name = \"Jungleland\"\n\n  [[albums.songs]]\n  name = \"Meeting Across the River\"\n\n[[albums]]\nname = \"Born in the USA\"\n  \n  [[albums.songs]]\n  name = \"Glory Days\"\n\n  [[albums.songs]]\n  name = \"Dancing in the Dark\"\n", 262};

    const Table expected{{ "albums", Value::of_array({Value::of_table({{ "name", Value::of_string("Born to Run") },{ "songs", Value::of_array({Value::of_table({{ "name", Value::of_string("Jungleland") }}),Value::of_table({{ "name", Value::of_string("Meeting Across the River") }})}) }}),Value::of_table({{ "name", Value::of_string("Born in the USA") },{ "songs", Value::of_array({Value::of_table({{ "name", Value::of_string("Glory Days") }}),Value::of_table({{ "name", Value::of_string("Dancing in the Dark") }})}) }})}) }};

    assert_parsed(toml, expected);
}
