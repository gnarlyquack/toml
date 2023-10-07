TEST(valid_spec_tests, test_table_3)
{
    const string toml("[a.b.c]            # this is best practice\n[ d.e.f ]          # same as [d.e.f]\n[ g .  h  . i ]    # same as [g.h.i]\n[ j . \"\312\236\" . 'l' ]  # same as [j.\"\312\236\".'l']\n", 160);

    const Table expected = {{ std::string("a", 1), Value(Table({{ std::string("b", 1), Value(Table({{ std::string("c", 1), Value(Table({})) }})) }})) },{ std::string("d", 1), Value(Table({{ std::string("e", 1), Value(Table({{ std::string("f", 1), Value(Table({})) }})) }})) },{ std::string("g", 1), Value(Table({{ std::string("h", 1), Value(Table({{ std::string("i", 1), Value(Table({})) }})) }})) },{ std::string("j", 1), Value(Table({{ std::string("ʞ", 2), Value(Table({{ std::string("l", 1), Value(Table({})) }})) }})) }};

    assert_parsed(toml, expected);
}
