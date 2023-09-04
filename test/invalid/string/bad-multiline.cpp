TEST(invalid_string_tests, test_bad_multiline)
{
    const string toml{"multi = \"first line\nsecond line\"\n", 33};

    const vector<Error> expected{
        { 1, 20, "Unterminated string." },
        { 2, 8, "Missing '=' between key and value." },
        { 2, 8, "Invalid value: line\"" },
    };

    assert_errors(toml, expected);
}
