TEST(valid_spec_tests, test_inline_table_3)
{
    const string toml("[product]\ntype.name = \"Nail\"\n# type = { edible = false }  # INVALID\n", 68);

    const Table expected = {{ std::string("product", 7), Value(Table({{ std::string("type", 4), Value(Table({{ std::string("name", 4), Value(std::string("Nail", 4)) }})) }})) }};

    assert_parsed(toml, expected);
}
