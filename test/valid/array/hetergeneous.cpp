TEST(valid_array_tests, test_hetergeneous)
{
    const string toml{"mixed = [[1, 2], [\"a\", \"b\"], [1.1, 2.1]]\n", 41};

    const Table expected{{ "mixed", Value::of_array({Value::of_array({Value::of_integer(1ULL),Value::of_integer(2ULL)}),Value::of_array({Value::of_string("a"),Value::of_string("b")}),Value::of_array({Value::of_float(1.1),Value::of_float(2.1)})}) }};

    assert_parsed(toml, expected);
}
