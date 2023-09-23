TEST(invalid_string_tests, test_multiline_bad_escape_1)
{
    const string toml("k = \"\"\"t\\a\"\"\"\n\n", 15);

    const vector<Error> expected = {
        { 8, 1, 9, "Invalid escape sequence." },
    };

    assert_errors(toml, expected);
}
