TEST(valid_tests, test_implicit_and_explicit_after)
{
    const string toml{"[a.b.c]\nanswer = 42\n\n[a]\nbetter = 43\n", 37};

    const Table expected{{ "a", Value::of_table({{ "b", Value::of_table({{ "c", Value::of_table({{ "answer", Value::of_integer(42ULL) }}) }}) },{ "better", Value::of_integer(43ULL) }}) }};

    assert_parsed(toml, expected);
}
