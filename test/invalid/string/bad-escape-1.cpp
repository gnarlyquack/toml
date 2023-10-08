TEST(invalid_string_tests, test_bad_escape_1)
{
    const string toml("invalid-escape = \"This string has a bad \\a escape character.\"\n", 62);

    const vector<Error> expected = {
        { 40, 1, 41, "Invalid escape sequence: \\a" },
    };

    assert_errors(toml, expected);
}
