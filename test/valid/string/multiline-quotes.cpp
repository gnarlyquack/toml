TEST(valid_string_tests, test_multiline_quotes)
{
    const string toml{"# Make sure that quotes inside multiline strings are allowed, including right\n# after the opening '''/\"\"\" and before the closing '''/\"\"\"\n\nlit_one = ''''one quote''''\nlit_two = '''''two quotes'''''\nlit_one_space = ''' 'one quote' '''\nlit_two_space = ''' ''two quotes'' '''\n\none = \"\"\"\"one quote\"\"\"\"\ntwo = \"\"\"\"\"two quotes\"\"\"\"\"\none_space = \"\"\" \"one quote\" \"\"\"\ntwo_space = \"\"\" \"\"two quotes\"\" \"\"\"\n\nmismatch1 = \"\"\"aaa'''bbb\"\"\"\nmismatch2 = '''aaa\"\"\"bbb'''\n\n# Three opening \"\"\", then one escaped \", then two \"\" (allowed), and then three\n# closing \"\"\"\nescaped = \"\"\"lol\\\"\"\"\"\"\"\n", 566};

    const Table expected{{ "escaped", Value::of_string("lol\"\"\"") },{ "lit_one", Value::of_string("'one quote'") },{ "lit_one_space", Value::of_string(" 'one quote' ") },{ "lit_two", Value::of_string("''two quotes''") },{ "lit_two_space", Value::of_string(" ''two quotes'' ") },{ "mismatch1", Value::of_string("aaa'''bbb") },{ "mismatch2", Value::of_string("aaa\"\"\"bbb") },{ "one", Value::of_string("\"one quote\"") },{ "one_space", Value::of_string(" \"one quote\" ") },{ "two", Value::of_string("\"\"two quotes\"\"") },{ "two_space", Value::of_string(" \"\"two quotes\"\" ") }};

    assert_parsed(toml, expected);
}
