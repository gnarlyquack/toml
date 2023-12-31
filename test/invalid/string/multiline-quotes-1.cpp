TEST(invalid_string_tests, test_multiline_quotes_1)
{
    const string toml("a = \"\"\"6 quotes: \"\"\"\"\"\"\n", 24);

    const vector<Error> expected = {
        { 22, 1, 23, "Expected the end of the line but got: \"" },
    };

    assert_errors(toml, expected);
}
