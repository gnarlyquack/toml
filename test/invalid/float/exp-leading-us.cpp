TEST(invalid_float_tests, test_exp_leading_us)
{
    const string toml("exp-leading-us = 1e_23\n", 23);

    const vector<Error> expected = {
        { 17, 1, 18, "Invalid value: 1e_23" },
    };

    assert_errors(toml, expected);
}
