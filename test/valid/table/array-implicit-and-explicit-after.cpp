TEST(valid_table_tests, test_array_implicit_and_explicit_after)
{
    const string toml("[[a.b]]\nx = 1\n\n[a]\ny = 2\n", 25);

    const Table expected = {{ std::string("a", 1), Value(Table({{ std::string("b", 1), Value(Array({Value(Table({{ std::string("x", 1), Value(INT64_C(1)) }}))})) },{ std::string("y", 1), Value(INT64_C(2)) }})) }};

    assert_parsed(toml, expected);
}
