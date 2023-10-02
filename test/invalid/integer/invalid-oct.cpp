TEST(invalid_integer_tests, test_invalid_oct)
{
    const string toml("invalid-oct = 0o778\n", 20);

    const vector<Error> expected = {
        { 14, 1, 15, "Invalid octal number: 0o778" },
    };

    assert_errors(toml, expected);
}
