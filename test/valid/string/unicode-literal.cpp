TEST(valid_string_tests, test_unicode_literal)
{
    const string toml("answer = \"\316\264\"\n", 14);

    const Table expected = {{ std::string("answer", 6), Value(std::string("δ", 2)) }};

    assert_parsed(toml, expected);
}
