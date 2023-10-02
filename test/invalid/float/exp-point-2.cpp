TEST(invalid_float_tests, test_exp_point_2)
{
    const string toml("exp-point-2 = 1.e2\n", 19);

    const vector<Error> expected = {
        { 14, 1, 15, "Invalid value: 1.e2" },
    };

    assert_errors(toml, expected);
}
