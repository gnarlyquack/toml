TEST(valid_spec_tests, test_float_0)
{
    const string toml("# fractional\nflt1 = +1.0\nflt2 = 3.1415\nflt3 = -0.01\n\n# exponent\nflt4 = 5e+22\nflt5 = 1e06\nflt6 = -2E-2\n\n# both\nflt7 = 6.626e-34\n", 127);

    const Table expected = {{ std::string("flt1", 4), Value(1.0) },{ std::string("flt2", 4), Value(3.1415) },{ std::string("flt3", 4), Value(-0.01) },{ std::string("flt4", 4), Value(5e+22) },{ std::string("flt5", 4), Value(1e+06) },{ std::string("flt6", 4), Value(-0.02) },{ std::string("flt7", 4), Value(6.626e-34) }};

    assert_parsed(toml, expected);
}
