TEST(valid_spec_tests, test_table_1)
{
    const string toml("[table-1]\nkey1 = \"some string\"\nkey2 = 123\n\n[table-2]\nkey1 = \"another string\"\nkey2 = 456\n", 88);

    const Table expected = {{ std::string("table-1", 7), Value(Table({{ std::string("key1", 4), Value(std::string("some string", 11)) },{ std::string("key2", 4), Value(INT64_C(123)) }})) },{ std::string("table-2", 7), Value(Table({{ std::string("key1", 4), Value(std::string("another string", 14)) },{ std::string("key2", 4), Value(INT64_C(456)) }})) }};

    assert_parsed(toml, expected);
}
