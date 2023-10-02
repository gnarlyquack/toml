TEST(invalid_integer_tests, test_incomplete_oct)
{
    const string toml("incomplete-oct = 0o\n", 20);

    const vector<Error> expected = {
        { 17, 1, 18, "Invalid octal number: 0o" },
    };

    assert_errors(toml, expected);
}
