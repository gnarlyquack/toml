TEST(invalid_integer_tests, test_trailing_us_hex)
{
    const string toml("trailing-us-hex = 0x1_\n", 23);

    const vector<Error> expected = {
        { 18, 1, 19, "Invalid hexadecimal number: 0x1_" },
    };

    assert_errors(toml, expected);
}
