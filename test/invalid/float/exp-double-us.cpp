TEST(invalid_float_tests, test_exp_double_us)
{
    const string toml("exp-double-us = 1e__23\n", 23);

    const vector<Error> expected = {
        { 18, 1, 19, "'_' must separate digits in exponential part of decimal number: 1e__23" },
    };

    assert_errors(toml, expected);
}
