TEST(valid_key_tests, test_numeric_dotted)
{
    const string toml{"1.2 = 3\n", 8};

    const Table expected{{ "1", Value::of_table({{ "2", Value::of_integer(3ULL) }}) }};

    assert_parsed(toml, expected);
}
