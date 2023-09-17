TEST(invalid_integer_tests, test_incomplete_hex)
{
    const string toml("incomplete-hex = 0x\n", 20);

    const vector<Error> expected = {
        { 19, 1, 20, "Missing hexadecimal number." },
    };

    assert_errors(toml, expected);
}
