TEST(invalid_float_tests, test_trailing_point_min)
{
    const string toml{"trailing-point-min = -1.\n", 25};

    const vector<Error> expected{{ 1, 25, "Missing fractional part of decimal number." },};

    assert_errors(toml, expected);
}
