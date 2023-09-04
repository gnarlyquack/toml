TEST(invalid_inline_table_tests, test_linebreak_4)
{
    const string toml{"json_like = {\n          first = \"Tom\",\n          last = \"Preston-Werner\"\n}\n", 75};

    const vector<Error> expected{
        { 1, 14, "Unterminated inline table." },
        { 2, 24, "Expected the end of the line but got: ," },
        { 4, 1, "Invalid key: }" },
        { 4, 2, "Missing '=' between key and value." },
        { 4, 2, "Missing value." },
    };

    assert_errors(toml, expected);
}
