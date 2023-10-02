TEST(invalid_integer_tests, test_incomplete_bin)
{
    const string toml("incomplete-bin = 0b\n", 20);

    const vector<Error> expected = {
        { 17, 1, 18, "Invalid binary number: 0b" },
    };

    assert_errors(toml, expected);
}
