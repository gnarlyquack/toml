TEST(valid_key_tests, test_equals_nospace)
{
    const string toml("answer=42\n", 10);

    const Table expected = {{ std::string("answer", 6), Value(INT64_C(42)) }};

    assert_parsed(toml, expected);
}
