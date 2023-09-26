TEST(valid_float_tests, test_inf_and_nan)
{
    const string toml("# We don't encode +nan and -nan back with the signs; many languages don't\n# support a sign on NaN (it doesn't really make much sense).\nnan = nan\nnan_neg = -nan\nnan_plus = +nan\ninfinity = inf\ninfinity_neg = -inf\ninfinity_plus = +inf\n", 232);

    const Table expected = {{ "infinity", Value(numeric_limits<double>::infinity()) },{ "infinity_neg", Value(-numeric_limits<double>::infinity()) },{ "infinity_plus", Value(+numeric_limits<double>::infinity()) },{ "nan", Value(numeric_limits<double>::quiet_NaN()) },{ "nan_neg", Value(numeric_limits<double>::quiet_NaN()) },{ "nan_plus", Value(numeric_limits<double>::quiet_NaN()) }};

    assert_parsed(toml, expected);
}
