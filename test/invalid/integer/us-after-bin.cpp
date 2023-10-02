TEST(invalid_integer_tests, test_us_after_bin)
{
    const string toml("us-after-bin = 0b_1\n", 20);

    const vector<Error> expected = {
        { 15, 1, 16, "Invalid binary number: 0b_1" },
    };

    assert_errors(toml, expected);
}
