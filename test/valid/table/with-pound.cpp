TEST(valid_table_tests, test_with_pound)
{
    const string toml{"[\"key#group\"]\nanswer = 42\n", 26};

    const Table expected{{ "key#group", Value(Table({{ "answer", Value(INT64_C(42)) }})) }};

    assert_parsed(toml, expected);
}
