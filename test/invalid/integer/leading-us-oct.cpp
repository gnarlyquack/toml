TEST(invalid_integer_tests, test_leading_us_oct)
{
    const string toml("leading-us-oct = _0o1\n", 22);

    const vector<Error> expected = {
        { 17, 1, 18, "Invalid value: _0o1" },
    };

    assert_errors(toml, expected);
}
