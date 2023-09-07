TEST(valid_table_tests, test_array_table_array)
{
    const string toml{"[[a]]\n    [[a.b]]\n        [a.b.c]\n            d = \"val0\"\n    [[a.b]]\n        [a.b.c]\n            d = \"val1\"\n", 108};

    const Table expected{{ "a", Value::of_array({Value::of_table({{ "b", Value::of_array({Value::of_table({{ "c", Value::of_table({{ "d", Value::of_string("val0") }}) }}),Value::of_table({{ "c", Value::of_table({{ "d", Value::of_string("val1") }}) }})}) }})}) }};

    assert_parsed(toml, expected);
}
