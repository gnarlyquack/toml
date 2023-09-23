TEST(invalid_string_tests, test_basic_multiline_quotes)
{
    const string toml("str5 = \"\"\"Here are three quotation marks: \"\"\".\"\"\"\n", 50);

    const vector<Error> expected = {
        { 45, 1, 46, "Expected the end of the line but got: .\"\"\"" },
    };

    assert_errors(toml, expected);
}
