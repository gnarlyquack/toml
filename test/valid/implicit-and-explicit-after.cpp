TEST(valid_tests, test_implicit_and_explicit_after)
{
    const string toml("[a.b.c]\nanswer = 42\n\n[a]\nbetter = 43\n", 37);

    const Table expected = {{ std::string("a", 1), Value(Table({{ std::string("b", 1), Value(Table({{ std::string("c", 1), Value(Table({{ std::string("answer", 6), Value(INT64_C(42)) }})) }})) },{ std::string("better", 6), Value(INT64_C(43)) }})) }};

    assert_parsed(toml, expected);
}
