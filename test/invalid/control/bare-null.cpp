TEST(invalid_control_tests, test_bare_null)
{
    const string toml("bare-null = \"some value\" \000\n", 27);

    const vector<Error> expected = {
        { 25, 1, 26, "Unicode codepoint U+0000 is not allowed." },
        // TODO remove this error(?)
        { 25, 1, 26, string("Expected the end of the line but got: \0", 39) },
    };

    assert_errors(toml, expected);
}
