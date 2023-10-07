TEST(invalid_float_tests, test_trailing_us_exp_1)
{
    const string toml("trailing-us-exp-1 = 1_e2\n", 25);

    const vector<Error> expected = {
        { 20, 1, 21, "Invalid value: 1_e2" },
    };

    assert_errors(toml, expected);
}
