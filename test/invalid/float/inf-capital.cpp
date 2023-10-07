TEST(invalid_float_tests, test_inf_capital)
{
    const string toml("v = Inf\n", 8);

    const vector<Error> expected = {
        { 4, 1, 5, "Invalid value: Inf" },
    };

    assert_errors(toml, expected);
}
