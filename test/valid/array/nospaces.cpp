TEST(valid_array_tests, test_nospaces)
{
    const string toml{"ints = [1,2,3]\n", 15};

    const Table expected{{ "ints", Value::of_array({Value::of_integer(1ULL),Value::of_integer(2ULL),Value::of_integer(3ULL)}) }};

    assert_parsed(toml, expected);
}
