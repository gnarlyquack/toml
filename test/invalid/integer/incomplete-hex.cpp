TEST(invalid_integer_tests, test_incomplete_hex)
{
    const string toml("incomplete-hex = 0x\n", 20);

    const vector<Error> expected = {
        { 17, 1, 18, "Invalid hexadecimal number: 0x" },
    };

    assert_errors(toml, expected);
}
