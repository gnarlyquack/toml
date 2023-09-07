TEST(valid_integer_tests, test_underscore)
{
    const string toml{"kilo = 1_000\nx = 1_1_1_1\n", 25};

    const Table expected{{ "kilo", Value::of_integer(1000ULL) },{ "x", Value::of_integer(1111ULL) }};

    assert_parsed(toml, expected);
}
