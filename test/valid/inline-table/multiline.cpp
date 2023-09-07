TEST(valid_inline_table_tests, test_multiline)
{
    const string toml{"tbl_multiline = { a = 1, b = \"\"\"\nmultiline\n\"\"\", c = \"\"\"and yet\nanother line\"\"\", d = 4 }\n", 88};

    const Table expected{{ "tbl_multiline", Value::of_table({{ "a", Value::of_integer(1ULL) },{ "b", Value::of_string("multiline\n") },{ "c", Value::of_string("and yet\nanother line") },{ "d", Value::of_integer(4ULL) }}) }};

    assert_parsed(toml, expected);
}
