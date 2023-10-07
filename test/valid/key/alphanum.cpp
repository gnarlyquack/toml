TEST(valid_key_tests, test_alphanum)
{
    const string toml("alpha = \"a\"\n123 = \"num\"\n000111 = \"leading\"\n10e3 = \"false float\"\none1two2 = \"mixed\"\nwith-dash = \"dashed\"\nunder_score = \"___\"\n34-11 = 23\n\n[2018_10]\n001 = 1\n\n[a-a-a]\n_ = false\n", 173);

    const Table expected = {{ std::string("000111", 6), Value(std::string("leading", 7)) },{ std::string("10e3", 4), Value(std::string("false float", 11)) },{ std::string("123", 3), Value(std::string("num", 3)) },{ std::string("2018_10", 7), Value(Table({{ std::string("001", 3), Value(INT64_C(1)) }})) },{ std::string("34-11", 5), Value(INT64_C(23)) },{ std::string("a-a-a", 5), Value(Table({{ std::string("_", 1), Value(false) }})) },{ std::string("alpha", 5), Value(std::string("a", 1)) },{ std::string("one1two2", 8), Value(std::string("mixed", 5)) },{ std::string("under_score", 11), Value(std::string("___", 3)) },{ std::string("with-dash", 9), Value(std::string("dashed", 6)) }};

    assert_parsed(toml, expected);
}
