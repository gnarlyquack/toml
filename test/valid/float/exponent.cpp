TEST(valid_float_tests, test_exponent)
{
    const string toml{"lower = 3e2\nupper = 3E2\nneg = 3e-2\npos = 3E+2\nzero = 3e0\npointlower = 3.1e2\npointupper = 3.1E2\nminustenth = -1E-1\n", 114};

    const Table expected{{ "lower", Value::of_float(300.0) },{ "minustenth", Value::of_float(-0.1) },{ "neg", Value::of_float(0.03) },{ "pointlower", Value::of_float(310.0) },{ "pointupper", Value::of_float(310.0) },{ "pos", Value::of_float(300.0) },{ "upper", Value::of_float(300.0) },{ "zero", Value::of_float(3.0) }};

    assert_parsed(toml, expected);
}
