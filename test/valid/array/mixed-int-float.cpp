TEST(valid_array_tests, test_mixed_int_float)
{
    const string toml("ints-and-floats = [1, 1.1]\n", 27);

    const Table expected = {{ "ints-and-floats", Value(Array({Value(INT64_C(1)),Value(1.1)})) }};

    assert_parsed(toml, expected);
}
