TEST(invalid_string_tests, test_basic_unknown_escape)
{
    const string toml{"a = \"\\@\"\n", 9};

    const vector<Error> expected{{ 1, 6, "Invalid escape sequence." },};

    assert_errors(toml, expected);
}
