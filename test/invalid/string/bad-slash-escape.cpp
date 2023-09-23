TEST(invalid_string_tests, test_bad_slash_escape)
{
    const string toml("invalid-escape = \"This string has a bad \\/ escape character.\"\n", 62);

    const vector<Error> expected = {
        { 40, 1, 41, "Invalid escape sequence." },
    };

    assert_errors(toml, expected);
}
