TEST(valid_spec_tests, test_table_2)
{
    const string toml("[dog.\"tater.man\"]\ntype.name = \"pug\"\n", 36);

    const Table expected = {{ std::string("dog", 3), Value(Table({{ std::string("tater.man", 9), Value(Table({{ std::string("type", 4), Value(Table({{ std::string("name", 4), Value(std::string("pug", 3)) }})) }})) }})) }};

    assert_parsed(toml, expected);
}
