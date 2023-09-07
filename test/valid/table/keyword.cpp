TEST(valid_table_tests, test_keyword)
{
    const string toml{"[true]\n\n[false]\n\n[inf]\n\n[nan]\n\n\n", 32};

    const Table expected{{ "true", Value::of_table({}) },{ "false", Value::of_table({}) },{ "inf", Value::of_table({}) },{ "nan", Value::of_table({}) }};

    assert_parsed(toml, expected);
}
