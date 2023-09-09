TEST(valid_table_tests, test_with_literal_string)
{
    const string toml{"['a']\n[a.'\"b\"']\n[a.'\"b\"'.c]\nanswer = 42 \n", 41};

    const Table expected{{ "a", Value(Table({{ "\"b\"", Value(Table({{ "c", Value(Table({{ "answer", Value(INT64_C(42)) }})) }})) }})) }};

    assert_parsed(toml, expected);
}
