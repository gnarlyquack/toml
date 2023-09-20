TEST(invalid_table_tests, test_duplicate_key_table)
{
    const string toml("[fruit]\ntype = \"apple\"\n\n[fruit.type]\napple = \"yes\"\n", 51);

    const vector<Error> expected = {
        { 31, 4, 8, "Key 'type' has already been defined on line 2, character 1." },
    };

    assert_errors(toml, expected);
}
