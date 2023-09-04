TEST(valid_string_tests, test_multiline_quotes)
{
    const string toml{"# Make sure that quotes inside multiline strings are allowed, including right\n# after the opening '''/\"\"\" and before the closing '''/\"\"\"\n\nlit_one = ''''one quote''''\nlit_two = '''''two quotes'''''\nlit_one_space = ''' 'one quote' '''\nlit_two_space = ''' ''two quotes'' '''\n\none = \"\"\"\"one quote\"\"\"\"\ntwo = \"\"\"\"\"two quotes\"\"\"\"\"\none_space = \"\"\" \"one quote\" \"\"\"\ntwo_space = \"\"\" \"\"two quotes\"\" \"\"\"\n\nmismatch1 = \"\"\"aaa'''bbb\"\"\"\nmismatch2 = '''aaa\"\"\"bbb'''\n\n# Three opening \"\"\", then one escaped \", then two \"\" (allowed), and then three\n# closing \"\"\"\nescaped = \"\"\"lol\\\"\"\"\"\"\"\n", 566};

    const Table expected{{ "escaped", new StringValue("lol\"\"\"") },{ "lit_one", new StringValue("'one quote'") },{ "lit_one_space", new StringValue(" 'one quote' ") },{ "lit_two", new StringValue("''two quotes''") },{ "lit_two_space", new StringValue(" ''two quotes'' ") },{ "mismatch1", new StringValue("aaa'''bbb") },{ "mismatch2", new StringValue("aaa\"\"\"bbb") },{ "one", new StringValue("\"one quote\"") },{ "one_space", new StringValue(" \"one quote\" ") },{ "two", new StringValue("\"\"two quotes\"\"") },{ "two_space", new StringValue(" \"\"two quotes\"\" ") },};

    assert_parsed(toml, expected);
}
