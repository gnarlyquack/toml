TEST(invalid_integer_tests, test_leading_zero_1)
{
    const string toml("leading-zero-1 = 01\n", 20);

    const vector<Error> expected = {
        { 17, 1, 18, "Number may not have leading zeros." },
    };

    assert_errors(toml, expected);
}
