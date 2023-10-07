TEST(valid_integer_tests, test_long)
{
    const string toml("int64-max = 9223372036854775807\nint64-max-neg = -9223372036854775808\n", 69);

    const Table expected = {{ std::string("int64-max", 9), Value(INT64_C(9223372036854775807)) },{ std::string("int64-max-neg", 13), Value(static_cast<int64_t>(-9223372036854775808ULL)) }};

    assert_parsed(toml, expected);
}
