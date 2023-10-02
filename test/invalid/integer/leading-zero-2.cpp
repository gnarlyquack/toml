TEST(invalid_integer_tests, test_leading_zero_2)
{
    const string toml("leading-zero-2 = 00\n", 20);

    const vector<Error> expected = {
        { 17, 1, 18, "Number may not have leading zeros." },
    };

    assert_errors(toml, expected);
}
