TEST(valid_string_tests, test_escape_tricky)
{
    const string toml{"end_esc = \"String does not end here\\\" but ends here\\\\\"\nlit_end_esc = 'String ends here\\'\n\nmultiline_unicode = \"\"\"\n\\u00a0\"\"\"\n\nmultiline_not_unicode = \"\"\"\n\\\\u0041\"\"\"\n\nmultiline_end_esc = \"\"\"When will it end? \\\"\"\"...\"\"\\\" should be here\\\"\"\"\"\n\nlit_multiline_not_unicode = '''\n\\u007f'''\n\nlit_multiline_end = '''There is no escape\\'''\n", 328};

    const Table expected{{ "end_esc", Value::of_string("String does not end here\" but ends here\\") },{ "lit_end_esc", Value::of_string("String ends here\\") },{ "lit_multiline_end", Value::of_string("There is no escape\\") },{ "lit_multiline_not_unicode", Value::of_string("\\u007f") },{ "multiline_end_esc", Value::of_string("When will it end? \"\"\"...\"\"\" should be here\"") },{ "multiline_not_unicode", Value::of_string("\\u0041") },{ "multiline_unicode", Value::of_string("\u00A0") }};

    assert_parsed(toml, expected);
}
