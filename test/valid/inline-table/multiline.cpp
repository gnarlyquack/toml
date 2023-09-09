TEST(valid_inline_table_tests, test_multiline)
{
    const string toml{"tbl_multiline = { a = 1, b = \"\"\"\nmultiline\n\"\"\", c = \"\"\"and yet\nanother line\"\"\", d = 4 }\n", 88};

    const Table expected{{ "tbl_multiline", Value(Table({{ "a", Value(INT64_C(1)) },{ "b", Value(std::string("multiline\n")) },{ "c", Value(std::string("and yet\nanother line")) },{ "d", Value(INT64_C(4)) }})) }};

    assert_parsed(toml, expected);
}
