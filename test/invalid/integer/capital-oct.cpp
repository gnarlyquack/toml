TEST(invalid_integer_tests, test_capital_oct)
{
    const string toml("capital-oct = 0O0\n", 18);

    const vector<Error> expected = {
        { 14, 1, 15, "Invalid value: 0O0" },
    };

    assert_errors(toml, expected);
}
