TEST(invalid_float_tests, test_exp_trailing_us)
{
    const string toml("exp-trailing-us = 1e23_\n", 24);

    const vector<Error> expected = {
        { 18, 1, 19, "Invalid value: 1e23_" },
    };

    assert_errors(toml, expected);
}
