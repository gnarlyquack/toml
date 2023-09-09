TEST(valid_tests, test_implicit_groups)
{
    const string toml{"[a.b.c]\nanswer = 42\n", 20};

    const Table expected{{ "a", Value(Table({{ "b", Value(Table({{ "c", Value(Table({{ "answer", Value(INT64_C(42)) }})) }})) }})) }};

    assert_parsed(toml, expected);
}
