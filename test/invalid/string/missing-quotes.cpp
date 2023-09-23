TEST(invalid_string_tests, test_missing_quotes)
{
    const string toml("name = value\n", 13);

    const vector<Error> expected = {
        { 7, 1, 8, "Invalid value: value" },
    };

    assert_errors(toml, expected);
}
