TEST(invalid_integer_tests, test_us_after_oct)
{
    const string toml("us-after-oct = 0o_1\n", 20);

    const vector<Error> expected = {
        { 17, 1, 18, "'_' must separate digits in octal number: 0o_1" },
    };

    assert_errors(toml, expected);
}
