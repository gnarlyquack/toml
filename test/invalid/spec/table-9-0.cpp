TEST(invalid_spec_tests, test_table_9_0)
{
    const string toml("[fruit]\napple.color = \"red\"\napple.taste.sweet = true\n\n[fruit.apple]  # INVALID\n# [fruit.apple.taste]  # INVALID\n\n[fruit.apple.texture]  # you can add sub-tables\nsmooth = true\n", 175);

    const vector<Error> expected = {
        { 61, 5, 8, "Key 'apple' has already been defined on line 2, character 1." },
    };

    assert_errors(toml, expected);
}
