TEST(valid_table_tests, test_names)
{
    const string toml{"[a.b.c]\n[a.\"b.c\"]\n[a.'d.e']\n[a.' x ']\n[ d.e.f ]\n[ g . h . i ]\n[ j . \"\312\236\" . 'l' ]\n\n[x.1.2]\n", 90};

    const Table expected{{ "a", Value::of_table({{ " x ", Value::of_table({}) },{ "b", Value::of_table({{ "c", Value::of_table({}) }}) },{ "b.c", Value::of_table({}) },{ "d.e", Value::of_table({}) }}) },{ "d", Value::of_table({{ "e", Value::of_table({{ "f", Value::of_table({}) }}) }}) },{ "g", Value::of_table({{ "h", Value::of_table({{ "i", Value::of_table({}) }}) }}) },{ "j", Value::of_table({{ "ʞ", Value::of_table({{ "l", Value::of_table({}) }}) }}) },{ "x", Value::of_table({{ "1", Value::of_table({{ "2", Value::of_table({}) }}) }}) }};

    assert_parsed(toml, expected);
}
