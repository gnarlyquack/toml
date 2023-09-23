TEST(invalid_string_tests, test_literal_multiline_quotes_2)
{
    const string toml("a = '''15 apostrophes: ''''''''''''''''''\n", 42);

    const vector<Error> expected = {
        { 28, 1, 29, "Expected the end of the line but got: '''''''''''''" },
    };

    assert_errors(toml, expected);
}
