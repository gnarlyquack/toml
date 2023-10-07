TEST(valid_table_tests, test_with_single_quotes)
{
    const string toml("['a']\n[a.'b']\n[a.'b'.c]\nanswer = 42 \n", 37);

    const Table expected = {{ std::string("a", 1), Value(Table({{ std::string("b", 1), Value(Table({{ std::string("c", 1), Value(Table({{ std::string("answer", 6), Value(INT64_C(42)) }})) }})) }})) }};

    assert_parsed(toml, expected);
}
