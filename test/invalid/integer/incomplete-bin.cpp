TEST(invalid_integer_tests, test_incomplete_bin)
{
    const string toml("incomplete-bin = 0b\n", 20);

    const vector<Error> expected = {
        { 19, 1, 20, "Missing binary number." },
    };

    assert_errors(toml, expected);
}
