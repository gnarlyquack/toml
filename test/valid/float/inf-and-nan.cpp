TEST(valid_float_tests, test_inf_and_nan)
{
    const string toml{"# We don't encode +nan and -nan back with the signs; many languages don't\n# support a sign on NaN (it doesn't really make much sense).\nnan = nan\nnan_neg = -nan\nnan_plus = +nan\ninfinity = inf\ninfinity_neg = -inf\ninfinity_plus = +inf\n", 232};

    const Table expected{{ "infinity", new FloatValue(INF64) },{ "infinity_neg", new FloatValue(-INF64) },{ "infinity_plus", new FloatValue(+INF64) },{ "nan", new FloatValue(NAN64) },{ "nan_neg", new FloatValue(NAN64) },{ "nan_plus", new FloatValue(NAN64) },};

    assert_parsed(toml, expected);
}
