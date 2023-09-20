TEST(invalid_table_tests, test_text_after_table)
{
    const string toml("[error] this shouldn't be here\n", 31);

    const vector<Error> expected = {
        { 8, 1, 9, "Expected the end of the line but got: this shouldn't be here" },
    };

    assert_errors(toml, expected);
}
