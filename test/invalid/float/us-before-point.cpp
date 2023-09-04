TEST(invalid_float_tests, test_us_before_point)
{
    const string toml{"us-before-point = 1_.2\n", 23};

    const vector<Error> expected{{ 1, 19, "'_' must separate digits in whole part of decimal number: 1_" },};

    assert_errors(toml, expected);
}
