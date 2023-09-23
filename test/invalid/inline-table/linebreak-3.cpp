TEST(invalid_inline_table_tests, test_linebreak_3)
{
    const string toml("t = {a=1\n,b=2}\n", 15);

    const vector<Error> expected = {
        {  8, 1, 9, "Missing closing '}' for inline table." },
        {  9, 2, 1, "Invalid expression: ,b=2}" },
    };

    assert_errors(toml, expected);
}
