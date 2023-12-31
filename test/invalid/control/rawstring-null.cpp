TEST(invalid_control_tests, test_rawstring_null)
{
    const string toml("rawstring-null = 'null\000'\n", 25);

    const vector<Error> expected = {
        { 22, 1, 23, "Unicode codepoint U+0000 is not allowed." },
    };

    assert_errors(toml, expected);
}
