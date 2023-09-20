TEST(invalid_table_tests, test_equals_sign)
{
    const string toml("[name=bad]\n", 11);

    const vector<Error> expected = {
        { 1, 1, 2, "Invalid key: name=bad" },
    };

    assert_errors(toml, expected);
}
