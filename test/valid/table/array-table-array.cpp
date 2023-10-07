TEST(valid_table_tests, test_array_table_array)
{
    const string toml("[[a]]\n    [[a.b]]\n        [a.b.c]\n            d = \"val0\"\n    [[a.b]]\n        [a.b.c]\n            d = \"val1\"\n", 108);

    const Table expected = {{ std::string("a", 1), Value(Array({Value(Table({{ std::string("b", 1), Value(Array({Value(Table({{ std::string("c", 1), Value(Table({{ std::string("d", 1), Value(std::string("val0", 4)) }})) }})),Value(Table({{ std::string("c", 1), Value(Table({{ std::string("d", 1), Value(std::string("val1", 4)) }})) }}))})) }}))})) }};

    assert_parsed(toml, expected);
}
