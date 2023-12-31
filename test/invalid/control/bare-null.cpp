TEST(invalid_control_tests, test_bare_null)
{
    const string toml("bare-null = \"some value\" \000\n", 27);

    const vector<Error> expected = {
        { 25, 1, 26, "Unicode codepoint U+0000 is not allowed." },
    };

    assert_errors(toml, expected);
}
