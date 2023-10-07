TEST(valid_float_tests, test_inf_and_nan)
{
    const string toml("# We don't encode +nan and -nan back with the signs; many languages don't\n# support a sign on NaN (it doesn't really make much sense).\nnan = nan\nnan_neg = -nan\nnan_plus = +nan\ninfinity = inf\ninfinity_neg = -inf\ninfinity_plus = +inf\n", 232);

    const Table expected = {{ std::string("infinity", 8), Value(numeric_limits<double>::infinity()) },{ std::string("infinity_neg", 12), Value(-numeric_limits<double>::infinity()) },{ std::string("infinity_plus", 13), Value(+numeric_limits<double>::infinity()) },{ std::string("nan", 3), Value(numeric_limits<double>::quiet_NaN()) },{ std::string("nan_neg", 7), Value(numeric_limits<double>::quiet_NaN()) },{ std::string("nan_plus", 8), Value(numeric_limits<double>::quiet_NaN()) }};

    assert_parsed(toml, expected);
}
