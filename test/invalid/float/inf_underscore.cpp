TEST(invalid_float_tests, test_inf_underscore)
{
    const string toml{"inf_underscore = in_f\n", 22};

    const vector<Error> expected{{ 1, 18, "Invalid value: in_f" },};

    assert_errors(toml, expected);
}
