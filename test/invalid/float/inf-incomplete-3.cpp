TEST(invalid_float_tests, test_inf_incomplete_3)
{
    const string toml("inf-incomplete-3 = -in\n", 23);

    const vector<Error> expected = {
        { 19, 1, 20, "Invalid value: -in" },
    };

    assert_errors(toml, expected);
}
