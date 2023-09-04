TEST(invalid_integer_tests, test_double_us)
{
    const string toml{"double-us = 1__23\n", 18};

    const vector<Error> expected{{ 1, 13, "'_' must separate digits in decimal number: 1__23" },};

    assert_errors(toml, expected);
}
