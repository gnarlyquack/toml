TEST(invalid_spec_tests, test_table_9_1)
{
    const string toml("[fruit]\napple.color = \"red\"\napple.taste.sweet = true\n\n# [fruit.apple]  # INVALID\n[fruit.apple.taste]  # INVALID\n\n[fruit.apple.texture]  # you can add sub-tables\nsmooth = true\n", 175);

    const vector<Error> expected = {
        { 94, 6, 14, "Key 'taste' has already been defined on line 3, character 7." },
    };

    assert_errors(toml, expected);
}
