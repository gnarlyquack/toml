TEST(invalid_float_tests, test_nan_underscore)
{
    const string toml{"nan_underscore = na_n\n", 22};

    const vector<Error> expected{{ 1, 18, "Invalid value: na_n" },};

    assert_errors(toml, expected);
}
