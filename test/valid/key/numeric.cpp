TEST(valid_key_tests, test_numeric)
{
    const string toml("1 = 1\n", 6);

    const Table expected = {{ "1", Value(INT64_C(1)) }};

    assert_parsed(toml, expected);
}
