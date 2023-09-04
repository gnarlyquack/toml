TEST(invalid_float_tests, test_inf_incomplete_2)
{
    const string toml{"inf-incomplete-2 = +in\n", 23};

    const vector<Error> expected{{ 1, 20, "Invalid value: +in" },};

    assert_errors(toml, expected);
}
