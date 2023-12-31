TEST(invalid_key_tests, test_duplicate)
{
    const string toml("# DO NOT DO THIS\nname = \"Tom\"\nname = \"Pradyun\"\n", 47);

    const vector<Error> expected = {
        { 30, 3, 1, "Key 'name' has already been defined on line 2, character 1." },
    };

    assert_errors(toml, expected);
}
