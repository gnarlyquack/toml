TEST(invalid_control_tests, test_rawmulti_lf)
{
    const string toml("rawmulti-lf = '''null\020'''\n", 26);

    const vector<Error> expected = {
        { 21, 1, 22, "Unicode codepoint U+0010 is not allowed." },
    };

    assert_errors(toml, expected);
}
