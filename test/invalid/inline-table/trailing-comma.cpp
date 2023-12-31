TEST(invalid_inline_table_tests, test_trailing_comma)
{
    const string toml("# A terminating comma (also called trailing comma) is not permitted after the\n# last key/value pair in an inline table\nabc = { abc = 123, }\n", 140);

    const vector<Error> expected = {
        { 138, 3, 20, "Expected a key-value pair." },
    };

    assert_errors(toml, expected);
}
