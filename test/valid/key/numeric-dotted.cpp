TEST(valid_key_tests, test_numeric_dotted)
{
    const string toml("1.2 = 3\n", 8);

    const Table expected = {{ std::string("1", 1), Value(Table({{ std::string("2", 1), Value(INT64_C(3)) }})) }};

    assert_parsed(toml, expected);
}
