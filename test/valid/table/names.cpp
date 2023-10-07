TEST(valid_table_tests, test_names)
{
    const string toml("[a.b.c]\n[a.\"b.c\"]\n[a.'d.e']\n[a.' x ']\n[ d.e.f ]\n[ g . h . i ]\n[ j . \"\312\236\" . 'l' ]\n\n[x.1.2]\n", 90);

    const Table expected = {{ std::string("a", 1), Value(Table({{ std::string(" x ", 3), Value(Table({})) },{ std::string("b", 1), Value(Table({{ std::string("c", 1), Value(Table({})) }})) },{ std::string("b.c", 3), Value(Table({})) },{ std::string("d.e", 3), Value(Table({})) }})) },{ std::string("d", 1), Value(Table({{ std::string("e", 1), Value(Table({{ std::string("f", 1), Value(Table({})) }})) }})) },{ std::string("g", 1), Value(Table({{ std::string("h", 1), Value(Table({{ std::string("i", 1), Value(Table({})) }})) }})) },{ std::string("j", 1), Value(Table({{ std::string("ʞ", 2), Value(Table({{ std::string("l", 1), Value(Table({})) }})) }})) },{ std::string("x", 1), Value(Table({{ std::string("1", 1), Value(Table({{ std::string("2", 1), Value(Table({})) }})) }})) }};

    assert_parsed(toml, expected);
}
