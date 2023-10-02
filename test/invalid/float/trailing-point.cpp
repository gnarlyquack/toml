TEST(invalid_float_tests, test_trailing_point)
{
    const string toml("trailing-point = 1.\n", 20);

    const vector<Error> expected = {
        { 17, 1, 18, "Invalid value: 1." },
    };

    assert_errors(toml, expected);
}
