TEST(invalid_spec_tests, test_string_7_0)
{
    const string toml("quot15 = '''Here are fifteen quotation marks: \"\"\"\"\"\"\"\"\"\"\"\"\"\"\"'''\n\napos15 = '''Here are fifteen apostrophes: ''''''''''''''''''  # INVALID\napos15 = \"Here are fifteen apostrophes: '''''''''''''''\"\n\n# 'That,' she said, 'is still pointless.'\nstr = ''''That,' she said, 'is still pointless.''''\n", 290);

    const vector<Error> expected = {
        { 113, 3, 48, "Expected the end of the line but got: '''''''''''''" },
        { 138, 4,  1, "Key 'apos15' has already been defined on line 3, character 1." },
    };

    assert_errors(toml, expected);
}
