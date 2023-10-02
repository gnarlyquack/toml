TEST(invalid_integer_tests, test_double_us)
{
    const string toml("double-us = 1__23\n", 18);

    const vector<Error> expected = {
        { 12, 1, 13, "Invalid value: 1__23" },
    };

    assert_errors(toml, expected);
}
