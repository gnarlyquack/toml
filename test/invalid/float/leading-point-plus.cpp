TEST(invalid_float_tests, test_leading_point_plus)
{
    const string toml("leading-point-plus = +.12345\n", 29);

    const vector<Error> expected = {
        { 22, 1, 23, "Missing whole part of decimal number." },
    };

    assert_errors(toml, expected);
}
