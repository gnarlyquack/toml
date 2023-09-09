TEST(valid_table_tests, test_names)
{
    const string toml{"[a.b.c]\n[a.\"b.c\"]\n[a.'d.e']\n[a.' x ']\n[ d.e.f ]\n[ g . h . i ]\n[ j . \"\312\236\" . 'l' ]\n\n[x.1.2]\n", 90};

    const Table expected{{ "a", Value(Table({{ " x ", Value(Table({})) },{ "b", Value(Table({{ "c", Value(Table({})) }})) },{ "b.c", Value(Table({})) },{ "d.e", Value(Table({})) }})) },{ "d", Value(Table({{ "e", Value(Table({{ "f", Value(Table({})) }})) }})) },{ "g", Value(Table({{ "h", Value(Table({{ "i", Value(Table({})) }})) }})) },{ "j", Value(Table({{ "ʞ", Value(Table({{ "l", Value(Table({})) }})) }})) },{ "x", Value(Table({{ "1", Value(Table({{ "2", Value(Table({})) }})) }})) }};

    assert_parsed(toml, expected);
}
