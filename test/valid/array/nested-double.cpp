TEST(valid_array_tests, test_nested_double)
{
    const string toml{"nest = [\n\t[\n\t\t[\"a\"],\n\t\t[1, 2, [3]]\n\t]\n]\n", 40};

    const Table expected{{ "nest", Value::of_array({Value::of_array({Value::of_array({Value::of_string("a")}),Value::of_array({Value::of_integer(1ULL),Value::of_integer(2ULL),Value::of_array({Value::of_integer(3ULL)})})})}) }};

    assert_parsed(toml, expected);
}
