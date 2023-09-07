TEST(valid_inline_table_tests, test_inline_table)
{
    const string toml{"name = { first = \"Tom\", last = \"Preston-Werner\" }\npoint = { x = 1, y = 2 }\nsimple = { a = 1 }\nstr-key = { \"a\" = 1 }\ntable-array = [{ \"a\" = 1 }, { \"b\" = 2 }]\n", 157};

    const Table expected{{ "name", Value::of_table({{ "first", Value::of_string("Tom") },{ "last", Value::of_string("Preston-Werner") }}) },{ "point", Value::of_table({{ "x", Value::of_integer(1ULL) },{ "y", Value::of_integer(2ULL) }}) },{ "simple", Value::of_table({{ "a", Value::of_integer(1ULL) }}) },{ "str-key", Value::of_table({{ "a", Value::of_integer(1ULL) }}) },{ "table-array", Value::of_array({Value::of_table({{ "a", Value::of_integer(1ULL) }}),Value::of_table({{ "b", Value::of_integer(2ULL) }})}) }};

    assert_parsed(toml, expected);
}
