TEST(invalid_float_tests, test_us_before_point)
{
    const string toml("us-before-point = 1_.2\n", 23);

    const vector<Error> expected = {
        { 18, 1, 19, "Invalid value: 1_.2" },
    };

    assert_errors(toml, expected);
}
