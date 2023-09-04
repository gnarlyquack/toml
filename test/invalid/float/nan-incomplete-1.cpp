TEST(invalid_float_tests, test_nan_incomplete_1)
{
    const string toml{"nan-incomplete-1 = na\n", 22};

    const vector<Error> expected{{ 1, 20, "Invalid value: na" },};

    assert_errors(toml, expected);
}
