TEST(invalid_float_tests, test_exp_trailing_us)
{
    const string toml("exp-trailing-us = 1e_23_\n", 25);

    const vector<Error> expected = {
        { 18, 1, 19, "Invalid value: 1e_23_" },
    };

    assert_errors(toml, expected);
}
