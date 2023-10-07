TEST(valid_table_tests, test_with_pound)
{
    const string toml("[\"key#group\"]\nanswer = 42\n", 26);

    const Table expected = {{ std::string("key#group", 9), Value(Table({{ std::string("answer", 6), Value(INT64_C(42)) }})) }};

    assert_parsed(toml, expected);
}
