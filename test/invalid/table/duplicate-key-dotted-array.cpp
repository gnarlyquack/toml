TEST(invalid_table_tests, test_duplicate_key_dotted_array)
{
    const string toml("[fruit]\napple.color = \"red\"\n\n[[fruit.apple]]\n", 45);

    const vector<Error> expected = {
        { 37, 4, 9, "Key 'apple' has already been defined on line 2, character 1." },
    };

    assert_errors(toml, expected);
}
