TEST(invalid_string_tests, test_literal_multiline_quotes_1)
{
    const string toml("a = '''6 apostrophes: ''''''\n\n", 30);

    const vector<Error> expected = {
        { 27, 1, 28, "Expected the end of the line but got: '" },
    };

    assert_errors(toml, expected);
}
