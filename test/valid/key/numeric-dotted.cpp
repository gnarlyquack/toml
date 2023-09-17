TEST(valid_key_tests, test_numeric_dotted)
{
    const string toml("1.2 = 3\n", 8);

    const Table expected = {{ "1", Value(Table({{ "2", Value(INT64_C(3)) }})) }};

    assert_parsed(toml, expected);
}
