TEST(valid_inline_table_tests, test_multiline)
{
    const string toml{"tbl_multiline = { a = 1, b = \"\"\"\nmultiline\n\"\"\", c = \"\"\"and yet\nanother line\"\"\", d = 4 }\n", 88};

    const Table expected{{ "tbl_multiline", new TableValue({{ "a", new IntegerValue(1ULL) },{ "b", new StringValue("multiline\n") },{ "c", new StringValue("and yet\nanother line") },{ "d", new IntegerValue(4ULL) },}) },};

    assert_parsed(toml, expected);
}
