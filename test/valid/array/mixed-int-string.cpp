TEST(valid_array_tests, test_mixed_int_string)
{
    const string toml{"strings-and-ints = [\"hi\", 42]\n", 30};

    const Table expected{{ "strings-and-ints", Value(Array({Value(std::string("hi")),Value(INT64_C(42))})) }};

    assert_parsed(toml, expected);
}
