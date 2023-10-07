TEST(valid_float_tests, test_exponent)
{
    const string toml("lower = 3e2\nupper = 3E2\nneg = 3e-2\npos = 3E+2\nzero = 3e0\npointlower = 3.1e2\npointupper = 3.1E2\nminustenth = -1E-1\n", 114);

    const Table expected = {{ std::string("lower", 5), Value(300.0) },{ std::string("minustenth", 10), Value(-0.1) },{ std::string("neg", 3), Value(0.03) },{ std::string("pointlower", 10), Value(310.0) },{ std::string("pointupper", 10), Value(310.0) },{ std::string("pos", 3), Value(300.0) },{ std::string("upper", 5), Value(300.0) },{ std::string("zero", 4), Value(3.0) }};

    assert_parsed(toml, expected);
}
