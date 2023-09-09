TEST(valid_integer_tests, test_underscore)
{
    const string toml{"kilo = 1_000\nx = 1_1_1_1\n", 25};

    const Table expected{{ "kilo", Value(INT64_C(1000)) },{ "x", Value(INT64_C(1111)) }};

    assert_parsed(toml, expected);
}
