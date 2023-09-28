TEST(invalid_float_tests, test_leading_point)
{
    const string toml("leading-point = .12345\n", 23);

    const vector<Error> expected = {
        { 16, 1, 17, "Invalid value: .12345" },
    };

    assert_errors(toml, expected);
}
