TEST(invalid_inline_table_tests, test_linebreak_2)
{
    const string toml("t = {a=1,\nb=2}\n", 15);

    const vector<Error> expected = {
        {  9, 1, 10, "Missing closing '}' for inline table." },
        { 12, 2, 3, "Invalid decimal number: 2}" },
    };

    assert_errors(toml, expected);
}
