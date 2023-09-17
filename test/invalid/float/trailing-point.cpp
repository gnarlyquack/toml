TEST(invalid_float_tests, test_trailing_point)
{
    const string toml("trailing-point = 1.\n", 20);

    const vector<Error> expected = {
        { 19, 1, 20, "Missing fractional part of decimal number." },
    };

    assert_errors(toml, expected);
}
