TEST(invalid_string_tests, test_multiline_bad_escape_2)
{
    const string toml("# \\<Space> is not a valid escape.\nk = \"\"\"t\\ t\"\"\"\n", 49);

    const vector<Error> expected = {
        { 42, 2, 9, "Invalid escape sequence: \\ " },
    };

    assert_errors(toml, expected);
}
