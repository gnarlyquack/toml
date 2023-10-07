TEST(valid_spec_tests, test_inline_table_0)
{
    const string toml("name = { first = \"Tom\", last = \"Preston-Werner\" }\npoint = { x = 1, y = 2 }\nanimal = { type.name = \"pug\" }\n", 106);

    const Table expected = {{ std::string("animal", 6), Value(Table({{ std::string("type", 4), Value(Table({{ std::string("name", 4), Value(std::string("pug", 3)) }})) }})) },{ std::string("name", 4), Value(Table({{ std::string("first", 5), Value(std::string("Tom", 3)) },{ std::string("last", 4), Value(std::string("Preston-Werner", 14)) }})) },{ std::string("point", 5), Value(Table({{ std::string("x", 1), Value(INT64_C(1)) },{ std::string("y", 1), Value(INT64_C(2)) }})) }};

    assert_parsed(toml, expected);
}
