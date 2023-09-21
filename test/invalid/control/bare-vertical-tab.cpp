TEST(invalid_control_tests, test_bare_vertical_tab)
{
    const string toml("bare-vertical-tab = \v\n", 22);

    const vector<Error> expected = {
        { 20, 1, 21, "Unicode codepoint U+000B is not allowed." },
        // TODO remove this error
        { 20, 1, 21, "Invalid value: \v" },
    };

    assert_errors(toml, expected);
}
