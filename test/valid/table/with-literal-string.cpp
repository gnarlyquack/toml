TEST(valid_table_tests, test_with_literal_string)
{
    const string toml{"['a']\n[a.'\"b\"']\n[a.'\"b\"'.c]\nanswer = 42 \n", 41};

    const Table expected{{ "a", Value::of_table({{ "\"b\"", Value::of_table({{ "c", Value::of_table({{ "answer", Value::of_integer(42ULL) }}) }}) }}) }};

    assert_parsed(toml, expected);
}
