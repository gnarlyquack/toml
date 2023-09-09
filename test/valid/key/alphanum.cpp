TEST(valid_key_tests, test_alphanum)
{
    const string toml{"alpha = \"a\"\n123 = \"num\"\n000111 = \"leading\"\n10e3 = \"false float\"\none1two2 = \"mixed\"\nwith-dash = \"dashed\"\nunder_score = \"___\"\n34-11 = 23\n\n[2018_10]\n001 = 1\n\n[a-a-a]\n_ = false\n", 173};

    const Table expected{{ "000111", Value(std::string("leading")) },{ "10e3", Value(std::string("false float")) },{ "123", Value(std::string("num")) },{ "2018_10", Value(Table({{ "001", Value(INT64_C(1)) }})) },{ "34-11", Value(INT64_C(23)) },{ "a-a-a", Value(Table({{ "_", Value(false) }})) },{ "alpha", Value(std::string("a")) },{ "one1two2", Value(std::string("mixed")) },{ "under_score", Value(std::string("___")) },{ "with-dash", Value(std::string("dashed")) }};

    assert_parsed(toml, expected);
}
