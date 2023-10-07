TEST(valid_spec_tests, test_array_of_tables_0)
{
    const string toml("[[products]]\nname = \"Hammer\"\nsku = 738594937\n\n[[products]]  # empty table within the array\n\n[[products]]\nname = \"Nail\"\nsku = 284758393\n\ncolor = \"gray\"\n", 151);

    const Table expected = {{ std::string("products", 8), Value(Array({Value(Table({{ std::string("name", 4), Value(std::string("Hammer", 6)) },{ std::string("sku", 3), Value(INT64_C(738594937)) }})),Value(Table({})),Value(Table({{ std::string("color", 5), Value(std::string("gray", 4)) },{ std::string("name", 4), Value(std::string("Nail", 4)) },{ std::string("sku", 3), Value(INT64_C(284758393)) }}))})) }};

    assert_parsed(toml, expected);
}
