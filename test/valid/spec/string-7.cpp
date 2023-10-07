TEST(valid_spec_tests, test_string_7)
{
    const string toml("quot15 = '''Here are fifteen quotation marks: \"\"\"\"\"\"\"\"\"\"\"\"\"\"\"'''\n\n# apos15 = '''Here are fifteen apostrophes: ''''''''''''''''''  # INVALID\napos15 = \"Here are fifteen apostrophes: '''''''''''''''\"\n\n# 'That,' she said, 'is still pointless.'\nstr = ''''That,' she said, 'is still pointless.''''\n", 292);

    const Table expected = {{ std::string("apos15", 6), Value(std::string("Here are fifteen apostrophes: '''''''''''''''", 45)) },{ std::string("quot15", 6), Value(std::string("Here are fifteen quotation marks: \"\"\"\"\"\"\"\"\"\"\"\"\"\"\"", 49)) },{ std::string("str", 3), Value(std::string("'That,' she said, 'is still pointless.'", 39)) }};

    assert_parsed(toml, expected);
}
