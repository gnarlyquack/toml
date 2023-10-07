TEST(invalid_table_tests, test_append_to_array_with_dotted_keys)
{
    const string toml("[[a.b]]\n\n[a]\nb.y = 2\n", 21);

    const vector<Error> expected = {
        { 13, 4, 1, "Key 'b' has already been defined on line 1, character 5." },
    };

    assert_errors(toml, expected);
}
