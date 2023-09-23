TEST(invalid_string_tests, test_multiline_no_close_2)
{
    const string toml("x=\"\"\"\n", 6);

    const vector<Error> expected = {
        { 6, 2, 1, "Unterminated string." },
    };

    assert_errors(toml, expected);
}
