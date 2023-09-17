TEST(valid_table_tests, test_array_table_array)
{
    const string toml("[[a]]\n    [[a.b]]\n        [a.b.c]\n            d = \"val0\"\n    [[a.b]]\n        [a.b.c]\n            d = \"val1\"\n", 108);

    const Table expected = {{ "a", Value(Array({Value(Table({{ "b", Value(Array({Value(Table({{ "c", Value(Table({{ "d", Value(std::string("val0")) }})) }})),Value(Table({{ "c", Value(Table({{ "d", Value(std::string("val1")) }})) }}))})) }}))})) }};

    assert_parsed(toml, expected);
}
