TEST(invalid_float_tests, test_nan_capital)
{
    const string toml("v = NaN\n", 8);

    const vector<Error> expected = {
        { 4, 1, 5, "Invalid value: NaN" },
    };

    assert_errors(toml, expected);
}
