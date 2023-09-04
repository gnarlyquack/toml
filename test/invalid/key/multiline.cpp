TEST(invalid_key_tests, test_multiline)
{
    const string toml{"\"\"\"long\nkey\"\"\" = 1\n", 19};

    const vector<Error> expected{
        { 1, 3, "Missing '=' between key and value." },
        { 1, 8, "Unterminated string." },
        { 2, 1, "Invalid key: key\"\"\"" },
    };

    assert_errors(toml, expected);
}
