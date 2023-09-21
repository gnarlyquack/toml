TEST(invalid_key_tests, test_quoted_unclosed_2)
{
    const string toml("\"key\n", 5);

    const vector<Error> expected = {
        { 4, 1, 5, "Unterminated string." },
        { 4, 1, 5, "Missing '=' between key and value." },
        { 4, 1, 5, "Missing value." },
    };

    assert_errors(toml, expected);
}
