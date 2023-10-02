TEST(invalid_integer_tests, test_positive_oct)
{
    const string toml("positive-oct = +0o99\n", 21);

    const vector<Error> expected = {
        { 15, 1, 16, "Invalid octal number: +0o99" },
    };

    assert_errors(toml, expected);
}
