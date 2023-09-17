TEST(valid_string_tests, test_empty)
{
    const string toml("answer = \"\"\n", 12);

    const Table expected = {{ "answer", Value(std::string("")) }};

    assert_parsed(toml, expected);
}
