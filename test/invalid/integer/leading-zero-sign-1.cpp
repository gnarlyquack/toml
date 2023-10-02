TEST(invalid_integer_tests, test_leading_zero_sign_1)
{
    const string toml("leading-zero-sign-1 = -01\n", 26);

    const vector<Error> expected = {
        { 23, 1, 24, "Number may not have leading zeros." },
    };

    assert_errors(toml, expected);
}
