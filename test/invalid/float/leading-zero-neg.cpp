TEST(invalid_float_tests, test_leading_zero_neg)
{
    const string toml("leading-zero-neg = -03.14\n", 26);

    const vector<Error> expected = {
        { 20, 1, 21, "Number may not have leading zeros." },
    };

    assert_errors(toml, expected);
}
