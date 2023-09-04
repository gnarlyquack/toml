TEST(valid_table_tests, test_array_implicit)
{
    const string toml{"[[albums.songs]]\nname = \"Glory Days\"\n", 37};

    const Table expected{{ "albums", new TableValue({{ "songs", new ArrayValue({new TableValue({{ "name", new StringValue("Glory Days") },}),}) },}) },};

    assert_parsed(toml, expected);
}
