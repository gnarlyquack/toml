TEST(invalid_inline_table_tests, test_linebreak_1)
{
    const string toml("# No newlines are allowed between the curly braces unless they are valid within\n# a value.\nsimple = { a = 1 \n}\n", 111);

    const vector<Error> expected = {
        { 108, 3, 18, "Missing closing '}' for inline table." },
        { 109, 4,  1, "Invalid expression: }" },
    };

    assert_errors(toml, expected);
}
