TEST(invalid_inline_table_tests, test_double_comma)
{
    const string toml("t = {x=3,,y=4}\n", 15);

    const vector<Error> expected = {
        { 9, 1, 10, "Expected a key-value pair." },
    };

    assert_errors(toml, expected);
}
