TEST(invalid_string_tests, test_bad_escape_2)
{
    const string toml("invalid-escape = \"This string has a bad \\  escape character.\"\n\n", 63);

    const vector<Error> expected = {
        { 40, 1, 41, "Invalid escape sequence: \\ " },
    };

    assert_errors(toml, expected);
}
