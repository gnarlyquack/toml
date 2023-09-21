TEST(invalid_control_tests, test_rawstring_lf)
{
    const string toml("rawstring-lf = 'null\020'\n", 23);

    const vector<Error> expected = {
        { 20, 1, 21, "Unicode codepoint U+0010 is not allowed." },
    };

    assert_errors(toml, expected);
}
