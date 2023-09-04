TEST(invalid_control_tests, test_rawstring_del)
{
    const string toml{"rawstring-del = 'null\177'\n", 24};

    const vector<Error> expected{{ 1, 22, "Unicode codepoint U+007F is not allowed." },};

    assert_errors(toml, expected);
}
