TEST(invalid_spec_tests, test_keys_2)
{
    const string toml("= \"no key name\"  # INVALID\n\"\" = \"blank\"     # VALID but discouraged\n'' = 'blank'     # VALID but discouraged\n", 109);

    const vector<Error> expected = {
        {  0, 1, 1, "Invalid expression: = \"no key name\"" },
        { 68, 3, 1, "Key '' has already been defined on line 2, character 1." },
    };

    assert_errors(toml, expected);
}
