TEST(invalid_float_tests, test_us_after_point)
{
    const string toml("us-after-point = 1._2\n", 22);

    const vector<Error> expected = {
        { 17, 1, 18, "Invalid value: 1._2" },
    };

    assert_errors(toml, expected);
}
