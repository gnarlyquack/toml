TEST(valid_float_tests, test_zero)
{
    const string toml("zero = 0.0\nsigned-pos = +0.0\nsigned-neg = -0.0\nexponent = 0e0\nexponent-two-0 = 0e00\nexponent-signed-pos = +0e0\nexponent-signed-neg = -0e0\n", 138);

    const Table expected = {{ std::string("zero", 4), Value(0.0) },{ std::string("signed-pos", 10), Value(0.0) },{ std::string("signed-neg", 10), Value(0.0) },{ std::string("exponent", 8), Value(0.0) },{ std::string("exponent-two-0", 14), Value(0.0) },{ std::string("exponent-signed-pos", 19), Value(0.0) },{ std::string("exponent-signed-neg", 19), Value(0.0) }};

    assert_parsed(toml, expected);
}
