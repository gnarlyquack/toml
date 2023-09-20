TEST(invalid_inline_table_tests, test_overwrite)
{
    const string toml("a.b=0\n# Since table \"a\" is already defined, it can't be replaced by an inline table.\na={}\n", 90);

    const vector<Error> expected = {
        { 85, 3, 1, "Key 'a' has already been defined on line 1, character 1." },
    };

    assert_errors(toml, expected);
}
