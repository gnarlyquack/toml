TEST(invalid_key_tests, test_duplicate_keys)
{
    const string toml{"dupe = false\ndupe = true\n", 25};

    const vector<Error> expected{{ 2, 1, "Key 'dupe' has already been defined on line 1, character 1." },};

    assert_errors(toml, expected);
}
