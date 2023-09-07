TEST(valid_table_tests, test_array_implicit)
{
    const string toml{"[[albums.songs]]\nname = \"Glory Days\"\n", 37};

    const Table expected{{ "albums", Value::of_table({{ "songs", Value::of_array({Value::of_table({{ "name", Value::of_string("Glory Days") }})}) }}) }};

    assert_parsed(toml, expected);
}
