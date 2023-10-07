TEST(valid_inline_table_tests, test_multiline)
{
    const string toml("tbl_multiline = { a = 1, b = \"\"\"\nmultiline\n\"\"\", c = \"\"\"and yet\nanother line\"\"\", d = 4 }\n", 88);

    const Table expected = {{ std::string("tbl_multiline", 13), Value(Table({{ std::string("a", 1), Value(INT64_C(1)) },{ std::string("b", 1), Value(std::string("multiline\n", 10)) },{ std::string("c", 1), Value(std::string("and yet\nanother line", 20)) },{ std::string("d", 1), Value(INT64_C(4)) }})) }};

    assert_parsed(toml, expected);
}
