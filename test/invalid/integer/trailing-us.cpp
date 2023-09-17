TEST(invalid_integer_tests, test_trailing_us)
{
    const string toml("trailing-us = 123_\n", 19);

    const vector<Error> expected = {
        { 14, 1, 15, "'_' must separate digits in decimal number: 123_" },
    };

    assert_errors(toml, expected);
}
