TEST(valid_integer_tests, test_long)
{
    const string toml{"int64-max = 9223372036854775807\nint64-max-neg = -9223372036854775808\n", 69};

    const Table expected{{ "int64-max", Value::of_integer(9223372036854775807ULL) },{ "int64-max-neg", Value::of_integer(-9223372036854775808ULL) }};

    assert_parsed(toml, expected);
}
