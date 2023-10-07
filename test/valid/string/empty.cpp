TEST(valid_string_tests, test_empty)
{
    const string toml("answer = \"\"\n", 12);

    const Table expected = {{ std::string("answer", 6), Value(std::string("", 0)) }};

    assert_parsed(toml, expected);
}
