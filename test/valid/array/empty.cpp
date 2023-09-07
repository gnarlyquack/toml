TEST(valid_array_tests, test_empty)
{
    const string toml{"thevoid = [[[[[]]]]]\n", 21};

    const Table expected{{ "thevoid", Value::of_array({Value::of_array({Value::of_array({Value::of_array({Value::of_array({})})})})}) }};

    assert_parsed(toml, expected);
}
