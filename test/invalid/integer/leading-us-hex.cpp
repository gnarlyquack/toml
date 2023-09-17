TEST(invalid_integer_tests, test_leading_us_hex)
{
    const string toml("leading-us-hex = _0o1\n", 22);

    const vector<Error> expected = {
        { 17, 1, 18, "Invalid decimal number: _0o1" },
    };

    assert_errors(toml, expected);
}
