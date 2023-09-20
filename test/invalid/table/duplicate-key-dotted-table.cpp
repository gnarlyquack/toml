TEST(invalid_table_tests, test_duplicate_key_dotted_table)
{
    const string toml("[fruit]\napple.color = \"red\"\n\n[fruit.apple] # INVALID\n", 53);

    const vector<Error> expected = {
        { 36, 4, 8, "Key 'apple' has already been defined on line 2, character 1." },
    };

    assert_errors(toml, expected);
}
