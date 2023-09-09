TEST(valid_inline_table_tests, test_inline_table)
{
    const string toml{"name = { first = \"Tom\", last = \"Preston-Werner\" }\npoint = { x = 1, y = 2 }\nsimple = { a = 1 }\nstr-key = { \"a\" = 1 }\ntable-array = [{ \"a\" = 1 }, { \"b\" = 2 }]\n", 157};

    const Table expected{{ "name", Value(Table({{ "first", Value(std::string("Tom")) },{ "last", Value(std::string("Preston-Werner")) }})) },{ "point", Value(Table({{ "x", Value(INT64_C(1)) },{ "y", Value(INT64_C(2)) }})) },{ "simple", Value(Table({{ "a", Value(INT64_C(1)) }})) },{ "str-key", Value(Table({{ "a", Value(INT64_C(1)) }})) },{ "table-array", Value(Array({Value(Table({{ "a", Value(INT64_C(1)) }})),Value(Table({{ "b", Value(INT64_C(2)) }}))})) }};

    assert_parsed(toml, expected);
}
