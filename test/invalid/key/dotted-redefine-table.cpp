TEST(invalid_key_tests, test_dotted_redefine_table)
{
    const string toml{"# Defined a.b as int\na.b = 1\n# Tries to access it as table: error\na.b.c = 2\n", 76};

    const vector<Error> expected{{ 4, 3, "Key 'b' has already been defined on line 2, character 3." },};

    assert_errors(toml, expected);
}
