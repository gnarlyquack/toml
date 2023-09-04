TEST(valid_table_tests, test_array_table_array)
{
    const string toml{"[[a]]\n    [[a.b]]\n        [a.b.c]\n            d = \"val0\"\n    [[a.b]]\n        [a.b.c]\n            d = \"val1\"\n", 108};

    const Table expected{{ "a", new ArrayValue({new TableValue({{ "b", new ArrayValue({new TableValue({{ "c", new TableValue({{ "d", new StringValue("val0") },}) },}),new TableValue({{ "c", new TableValue({{ "d", new StringValue("val1") },}) },}),}) },}),}) },};

    assert_parsed(toml, expected);
}
