TEST(valid_float_tests, test_zero)
{
    const string toml("zero = 0.0\nsigned-pos = +0.0\nsigned-neg = -0.0\nexponent = 0e0\nexponent-two-0 = 0e00\nexponent-signed-pos = +0e0\nexponent-signed-neg = -0e0\n", 138);

    const Table expected = {{ "zero", Value(0.0) },{ "signed-pos", Value(0.0) },{ "signed-neg", Value(0.0) },{ "exponent", Value(0.0) },{ "exponent-two-0", Value(0.0) },{ "exponent-signed-pos", Value(0.0) },{ "exponent-signed-neg", Value(0.0) }};

    assert_parsed(toml, expected);
}
