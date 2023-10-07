TEST(invalid_spec_tests, test_inline_table_2_0)
{
    const string toml("[product]\ntype = { name = \"Nail\" }\ntype.edible = false  # INVALID\n", 66);

    const vector<Error> expected = {
        { 35, 3, 1, "Key 'type' has already been defined on line 2, character 1." },
    };

    assert_errors(toml, expected);
}
