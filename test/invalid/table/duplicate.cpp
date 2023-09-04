TEST(invalid_table_tests, test_duplicate)
{
    const string toml{"[a]\nb = 1\n\n[a]\nc = 2\n", 21};

    const vector<Error> expected{{ 4, 2, "Key 'a' has already been defined on line 1, character 2." },};

    assert_errors(toml, expected);
}
