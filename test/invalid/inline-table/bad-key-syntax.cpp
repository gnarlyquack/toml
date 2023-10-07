TEST(invalid_inline_table_tests, test_bad_key_syntax)
{
    const string toml("tbl = { a = 1, [b] }\n", 21);

    const vector<Error> expected = {
        { 15, 1, 16, "Invalid key: [b]" },
        { 19, 1, 20, "Missing '=' between key and value." },
        // FIXME This should be a missing value
        { 19, 1, 20, "Invalid value: }" },
        { 20, 1, 21, "Missing closing '}' for inline table." },
    };

    assert_errors(toml, expected);
}
