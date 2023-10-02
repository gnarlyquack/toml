TEST(invalid_float_tests, test_trailing_us_exp)
{
    const string toml("# trailing underscore in integer part is not allowed\ntrailing-us-exp = 1_e2\n# trailing underscore in float part is not allowed\ntrailing-us-exp2 = 1.2_e2\n", 153);

    const vector<Error> expected = {
        {  71, 2, 19, "Invalid value: 1_e2" },
        { 146, 4, 20, "Invalid value: 1.2_e2" },
    };

    assert_errors(toml, expected);
}
