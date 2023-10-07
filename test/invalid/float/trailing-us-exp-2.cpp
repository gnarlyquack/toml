TEST(invalid_float_tests, test_trailing_us_exp_2)
{
    const string toml("trailing-us-exp-2 = 1.2_e2\n", 27);

    const vector<Error> expected = {
        { 20, 1, 21, "Invalid value: 1.2_e2" },
    };

    assert_errors(toml, expected);
}
