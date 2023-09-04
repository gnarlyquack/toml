TEST(valid_key_tests, test_equals_nospace)
{
    const string toml{"answer=42\n", 10};

    const Table expected{{ "answer", new IntegerValue(42ULL) },};

    assert_parsed(toml, expected);
}
