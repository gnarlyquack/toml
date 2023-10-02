TEST(invalid_float_tests, test_trailing_us)
{
    const string toml("trailing-us = 1.2_\n", 19);

    const vector<Error> expected = {
        { 14, 1, 15, "Invalid value: 1.2_" },
    };

    assert_errors(toml, expected);
}
