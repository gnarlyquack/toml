TEST(invalid_float_tests, test_trailing_us_exp)
{
    const string toml("# trailing underscore in integer part is not allowed\ntrailing-us-exp = 1_e2\n# trailing underscore in float part is not allowed\ntrailing-us-exp2 = 1.2_e2\n", 153);

    const vector<Error> expected = {
        {  71, 2, 19, "'_' must separate digits in whole part of decimal number: 1_" },
        { 148, 4, 22, "'_' must separate digits in fractional part of decimal number: 1.2_" },
    };

    assert_errors(toml, expected);
}
