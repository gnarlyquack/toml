TEST(invalid_control_tests, test_rawmulti_us)
{
    const string toml{"rawmulti-us = '''null\037'''\n", 26};

    const vector<Error> expected{{ 1, 22, "Unicode codepoint U+001F is not allowed." },};

    assert_errors(toml, expected);
}
