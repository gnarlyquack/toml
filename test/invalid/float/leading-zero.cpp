TEST(invalid_float_tests, test_leading_zero)
{
    const string toml("leading-zero = 03.14\n", 21);

    const vector<Error> expected = {
        { 15, 1, 16, "Leading zeros are not allowed in whole part of decimal number." },
    };

    assert_errors(toml, expected);
}
