TEST(invalid_float_tests, test_trailing_point_plus)
{
    const string toml("trailing-point-plus = +1.\n", 26);

    const vector<Error> expected = {
        { 22, 1, 23, "Invalid value: +1." },
    };

    assert_errors(toml, expected);
}
