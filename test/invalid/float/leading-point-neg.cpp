TEST(invalid_float_tests, test_leading_point_neg)
{
    const string toml{"leading-point-neg = -.12345\n", 28};

    const vector<Error> expected{{ 1, 22, "Invalid decimal value: .12345" },};

    assert_errors(toml, expected);
}
