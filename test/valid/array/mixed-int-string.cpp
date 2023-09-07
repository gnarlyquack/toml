TEST(valid_array_tests, test_mixed_int_string)
{
    const string toml{"strings-and-ints = [\"hi\", 42]\n", 30};

    const Table expected{{ "strings-and-ints", Value::of_array({Value::of_string("hi"),Value::of_integer(42ULL)}) }};

    assert_parsed(toml, expected);
}
