TEST(invalid_integer_tests, test_leading_zero_sign_3)
{
    const string toml("leading-zero-sign-3 = +0_1\n", 27);

    const vector<Error> expected = {
        { 23, 1, 24, "Number may not have leading zeros." },
    };

    assert_errors(toml, expected);
}
