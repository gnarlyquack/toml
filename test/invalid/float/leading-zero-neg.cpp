TEST(invalid_float_tests, test_leading_zero_neg)
{
    const string toml("leading-zero-neg = -03.14\n", 26);

    const vector<Error> expected = {
        { 20, 1, 21, "Leading zeros are not allowed in whole part of decimal number." },
    };

    assert_errors(toml, expected);
}
