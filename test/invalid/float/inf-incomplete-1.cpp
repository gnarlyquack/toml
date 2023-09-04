TEST(invalid_float_tests, test_inf_incomplete_1)
{
    const string toml{"inf-incomplete-1 = in\n", 22};

    const vector<Error> expected{{ 1, 20, "Invalid value: in" },};

    assert_errors(toml, expected);
}
