TEST(invalid_integer_tests, test_invalid_hex)
{
    const string toml("invalid-hex = 0xaafz\n", 21);

    const vector<Error> expected = {
        { 14, 1, 15, "Invalid hexadecimal number: 0xaafz" },
    };

    assert_errors(toml, expected);
}
