TEST(invalid_float_tests, test_leading_point_plus)
{
    const string toml("leading-point-plus = +.12345\n", 29);

    const vector<Error> expected = {
        { 21, 1, 22, "Invalid value: +.12345" },
    };

    assert_errors(toml, expected);
}
