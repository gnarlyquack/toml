TEST(invalid_inline_table_tests, test_linebreak_4)
{
    const string toml("json_like = {\n          first = \"Tom\",\n          last = \"Preston-Werner\"\n}\n", 75);

    const vector<Error> expected = {
        { 13, 1, 14, "Missing closing '}' for inline table." },
        { 37, 2, 24, "Expected the end of the line but got: ," },
        { 73, 4, 1, "Invalid expression: }" },
    };

    assert_errors(toml, expected);
}
