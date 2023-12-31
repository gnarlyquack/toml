TEST(invalid_key_tests, test_partial_quoted)
{
    const string toml("partial\"quoted\" = 5\n", 20);

    const vector<Error> expected = {
        { 0, 1, 1, "Invalid key: partial\"quoted\"" },
    };

    assert_errors(toml, expected);
}
