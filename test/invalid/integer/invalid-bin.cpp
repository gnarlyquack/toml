TEST(invalid_integer_tests, test_invalid_bin)
{
    const string toml("invalid-bin = 0b0012\n", 21);

    const vector<Error> expected = {
        { 14, 1, 15, "Invalid binary number: 0b0012" },
    };

    assert_errors(toml, expected);
}
