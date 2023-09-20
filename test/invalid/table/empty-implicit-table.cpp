TEST(invalid_table_tests, test_empty_implicit_table)
{
    const string toml("[naughty..naughty]\n", 19);

    const vector<Error> expected = {
        { 9, 1, 10, "Missing key." },
    };

    assert_errors(toml, expected);
}
