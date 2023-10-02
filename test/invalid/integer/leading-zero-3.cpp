TEST(invalid_integer_tests, test_leading_zero_3)
{
    const string toml("leading-zero-3 = 0_0\n", 21);

    const vector<Error> expected = {
        { 17, 1, 18, "Number may not have leading zeros." },
    };

    assert_errors(toml, expected);
}
