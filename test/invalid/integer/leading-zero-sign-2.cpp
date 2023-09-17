TEST(invalid_integer_tests, test_leading_zero_sign_2)
{
    const string toml("leading-zero-sign-2 = +01\n", 26);

    const vector<Error> expected = {
        { 23, 1, 24, "Leading zeros are not allowed in decimal number." },
    };

    assert_errors(toml, expected);
}
