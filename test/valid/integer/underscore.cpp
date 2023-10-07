TEST(valid_integer_tests, test_underscore)
{
    const string toml("kilo = 1_000\nx = 1_1_1_1\n", 25);

    const Table expected = {{ std::string("kilo", 4), Value(INT64_C(1000)) },{ std::string("x", 1), Value(INT64_C(1111)) }};

    assert_parsed(toml, expected);
}
