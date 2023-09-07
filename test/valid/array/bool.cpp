TEST(valid_array_tests, test_bool)
{
    const string toml{"a = [true, false]\n", 18};

    const Table expected{{ "a", Value::of_array({Value::of_boolean(true),Value::of_boolean(false)}) }};

    assert_parsed(toml, expected);
}
