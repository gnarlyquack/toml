TEST(invalid_float_tests, test_trailing_point_min)
{
    const string toml("trailing-point-min = -1.\n", 25);

    const vector<Error> expected = {
        { 21, 1, 22, "Invalid value: -1." },
    };

    assert_errors(toml, expected);
}
