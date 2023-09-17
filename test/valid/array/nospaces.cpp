TEST(valid_array_tests, test_nospaces)
{
    const string toml("ints = [1,2,3]\n", 15);

    const Table expected = {{ "ints", Value(Array({Value(INT64_C(1)),Value(INT64_C(2)),Value(INT64_C(3))})) }};

    assert_parsed(toml, expected);
}
