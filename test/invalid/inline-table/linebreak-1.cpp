TEST(invalid_inline_table_tests, test_linebreak_1)
{
    const string toml{"# No newlines are allowed between the curly braces unless they are valid within\n# a value.\nsimple = { a = 1 \n}\n", 111};

    const vector<Error> expected{
        { 3, 18, "Unterminated inline table." },
        { 4, 1, "Invalid key: }" },
        { 4, 2, "Missing '=' between key and value." },
        { 4, 2, "Missing value." },
    };

    assert_errors(toml, expected);
}
