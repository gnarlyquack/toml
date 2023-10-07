TEST(valid_inline_table_tests, test_inline_table)
{
    const string toml("name = { first = \"Tom\", last = \"Preston-Werner\" }\npoint = { x = 1, y = 2 }\nsimple = { a = 1 }\nstr-key = { \"a\" = 1 }\ntable-array = [{ \"a\" = 1 }, { \"b\" = 2 }]\n", 157);

    const Table expected = {{ std::string("name", 4), Value(Table({{ std::string("first", 5), Value(std::string("Tom", 3)) },{ std::string("last", 4), Value(std::string("Preston-Werner", 14)) }})) },{ std::string("point", 5), Value(Table({{ std::string("x", 1), Value(INT64_C(1)) },{ std::string("y", 1), Value(INT64_C(2)) }})) },{ std::string("simple", 6), Value(Table({{ std::string("a", 1), Value(INT64_C(1)) }})) },{ std::string("str-key", 7), Value(Table({{ std::string("a", 1), Value(INT64_C(1)) }})) },{ std::string("table-array", 11), Value(Array({Value(Table({{ std::string("a", 1), Value(INT64_C(1)) }})),Value(Table({{ std::string("b", 1), Value(INT64_C(2)) }}))})) }};

    assert_parsed(toml, expected);
}
