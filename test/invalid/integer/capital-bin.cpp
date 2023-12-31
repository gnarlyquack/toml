TEST(invalid_integer_tests, test_capital_bin)
{
    const string toml("capital-bin = 0B0\n", 18);

    const vector<Error> expected = {
        { 14, 1, 15, "Invalid value: 0B0" },
    };

    assert_errors(toml, expected);
}
