TEST(valid_tests, test_implicit_and_explicit_before)
{
    const string toml{"[a]\nbetter = 43\n\n[a.b.c]\nanswer = 42\n", 37};

    const Table expected{{ "a", Value(Table({{ "b", Value(Table({{ "c", Value(Table({{ "answer", Value(INT64_C(42)) }})) }})) },{ "better", Value(INT64_C(43)) }})) }};

    assert_parsed(toml, expected);
}
