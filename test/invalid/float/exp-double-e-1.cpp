TEST(invalid_float_tests, test_exp_double_e_1)
{
    const string toml("exp-double-e-1 = 1ee2\n", 22);

    const vector<Error> expected = {
        { 17, 1, 18, "Invalid value: 1ee2" },
    };

    assert_errors(toml, expected);
}
