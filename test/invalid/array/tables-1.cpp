TEST(invalid_array_tests, test_tables_1)
{
    const string toml("# INVALID TOML DOC\nfruit = []\n\n[[fruit]] # Not allowed\n", 55);

    const vector<Error> expected = {
        { 33, 4, 3, "Key 'fruit' has already been defined on line 2, character 1." },
    };

    assert_errors(toml, expected);
}
