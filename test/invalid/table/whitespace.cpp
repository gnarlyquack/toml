TEST(invalid_table_tests, test_whitespace)
{
    const string toml("[invalid key]\n", 14);

    const vector<Error> expected = {
        { 9, 1, 10, "Expected closing ']' for table header." },
    };

    assert_errors(toml, expected);
}
