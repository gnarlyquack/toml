TEST(invalid_control_tests, test_multi_del)
{
    const string toml{"multi-del = \"\"\"null\177\"\"\"\n", 24};

    const vector<Error> expected{{ 1, 20, "Unicode codepoint U+007F is not allowed." },};

    assert_errors(toml, expected);
}
