TEST(invalid_spec_tests, test_string_4_0)
{
    const string toml("str4 = \"\"\"Here are two quotation marks: \"\". Simple enough.\"\"\"\nstr5 = \"\"\"Here are three quotation marks: \"\"\".\"\"\"  # INVALID\nstr5 = \"\"\"Here are three quotation marks: \"\"\\\".\"\"\"\nstr6 = \"\"\"Here are fifteen quotation marks: \"\"\\\"\"\"\\\"\"\"\\\"\"\"\\\"\"\"\\\".\"\"\"\n\n# \"This,\" she said, \"is just a pointless statement.\"\nstr7 = \"\"\"\"This,\" she said, \"is just a pointless statement.\"\"\"\"\n", 361);

    const vector<Error> expected = {
        { 107, 2, 46, "Expected the end of the line but got: .\"\"\"" },
        { 123, 3,  1, "Key 'str5' has already been defined on line 2, character 1." },
    };

    assert_errors(toml, expected);
}
