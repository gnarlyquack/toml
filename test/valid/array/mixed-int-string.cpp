TEST(valid_array_tests, test_mixed_int_string)
{
    const string toml("strings-and-ints = [\"hi\", 42]\n", 30);

    const Table expected = {{ std::string("strings-and-ints", 16), Value(Array({Value(std::string("hi", 2)),Value(INT64_C(42))})) }};

    assert_parsed(toml, expected);
}
