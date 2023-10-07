TEST(valid_string_tests, test_unicode_escape)
{
    const string toml("answer4 = \"\\u03B4\"\nanswer8 = \"\\U000003B4\"\n", 42);

    const Table expected = {{ std::string("answer4", 7), Value(std::string("δ", 2)) },{ std::string("answer8", 7), Value(std::string("δ", 2)) }};

    assert_parsed(toml, expected);
}
