TEST(invalid_float_tests, test_leading_point_neg)
{
    const string toml("leading-point-neg = -.12345\n", 28);

    const vector<Error> expected = {
        { 20, 1, 21, "Invalid value: -.12345" },
    };

    assert_errors(toml, expected);
}
