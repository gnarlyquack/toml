TEST(invalid_key_tests, test_quoted_unclosed_1)
{
    const string toml("\"key = x\n", 9);

    const vector<Error> expected = {
        { 8, 1, 9, "Unclosed string." },
        { 8, 1, 9, "Missing '=' between key and value." },
        { 8, 1, 9, "Expected a value." },
    };

    assert_errors(toml, expected);
}
