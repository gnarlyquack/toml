TEST(invalid_inline_table_tests, test_nested_key_conflict)
{
    const string toml("tbl = { fruit = { apple.color = \"red\" }, fruit.apple.texture = { smooth = true } }\n\n", 84);

    const vector<Error> expected = {
        { 41, 1, 42, "Key 'fruit' has already been defined on line 1, character 9." },
    };

    assert_errors(toml, expected);
}
