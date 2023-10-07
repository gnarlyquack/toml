TEST(valid_spec_tests, test_float_2)
{
    const string toml("# infinity\nsf1 = inf  # positive infinity\nsf2 = +inf # positive infinity\nsf3 = -inf # negative infinity\n\n# not a number\nsf4 = nan  # actual sNaN/qNaN encoding is implementation-specific\nsf5 = +nan # same as `nan`\nsf6 = -nan # valid, actual encoding is implementation-specific\n", 276);

    const Table expected = {{ std::string("sf1", 3), Value(+numeric_limits<double>::infinity()) },{ std::string("sf2", 3), Value(+numeric_limits<double>::infinity()) },{ std::string("sf3", 3), Value(-numeric_limits<double>::infinity()) },{ std::string("sf4", 3), Value(numeric_limits<double>::quiet_NaN()) },{ std::string("sf5", 3), Value(numeric_limits<double>::quiet_NaN()) },{ std::string("sf6", 3), Value(numeric_limits<double>::quiet_NaN()) }};

    assert_parsed(toml, expected);
}
