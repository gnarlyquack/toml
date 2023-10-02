TEST(invalid_float_tests, test_double_point_1)
{
    const string toml("double-point-1 = 0..1\n", 22);

    const vector<Error> expected = {
        { 17, 1, 18, "Invalid value: 0..1" },
    };

    assert_errors(toml, expected);
}
