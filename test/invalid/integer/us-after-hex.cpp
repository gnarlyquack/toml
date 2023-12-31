TEST(invalid_integer_tests, test_us_after_hex)
{
    const string toml("us-after-hex = 0x_1\n", 20);

    const vector<Error> expected = {
        { 15, 1, 16, "Invalid hexadecimal number: 0x_1" },
    };

    assert_errors(toml, expected);
}
