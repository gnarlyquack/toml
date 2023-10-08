TEST(invalid_string_tests, test_multiline_no_close)
{
    const string toml("invalid = \"\"\"\n    this will fail\n", 33);

    const vector<Error> expected = {
        { 33, 3, 1, "Unclosed string." },
    };

    assert_errors(toml, expected);
}
