TEST(valid_array_tests, test_nested)
{
    const string toml{"nest = [[\"a\"], [\"b\"]]\n", 22};

    const Table expected{{ "nest", Value::of_array({Value::of_array({Value::of_string("a")}),Value::of_array({Value::of_string("b")})}) }};

    assert_parsed(toml, expected);
}
