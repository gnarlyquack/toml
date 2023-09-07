TEST(valid_table_tests, test_with_pound)
{
    const string toml{"[\"key#group\"]\nanswer = 42\n", 26};

    const Table expected{{ "key#group", Value::of_table({{ "answer", Value::of_integer(42ULL) }}) }};

    assert_parsed(toml, expected);
}
