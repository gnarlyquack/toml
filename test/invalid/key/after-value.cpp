TEST(invalid_key_tests, test_after_value)
{
    const string toml("first = \"Tom\" last = \"Preston-Werner\" # INVALID\n", 48);

    const vector<Error> expected = {
        { 14, 1, 15, "Expected the end of the line but got: last = \"Preston-Werner\"" },
    };

    assert_errors(toml, expected);
}
