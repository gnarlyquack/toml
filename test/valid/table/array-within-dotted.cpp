TEST(valid_table_tests, test_array_within_dotted)
{
    const string toml("[fruit]\napple.color = \"red\"\n\n[[fruit.apple.seeds]]\nsize = 2\n", 60);

    const Table expected = {{ std::string("fruit", 5), Value(Table({{ std::string("apple", 5), Value(Table({{ std::string("color", 5), Value(std::string("red", 3)) },{ std::string("seeds", 5), Value(Array({Value(Table({{ std::string("size", 4), Value(INT64_C(2)) }}))})) }})) }})) }};

    assert_parsed(toml, expected);
}
